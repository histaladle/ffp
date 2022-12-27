#include "ffwidget.h"
#include <QElapsedTimer>
extern "C"{
#include <libavutil/frame.h>
}
FFWidget::FFWidget(QWidget *parent)
    :QOpenGLWidget(parent)
{
    resolver=new MediaResolver();
    resam=new AudioResampler();
    aplayer=new AudioPlayer(this);
}

void FFWidget::open(QString filename)
{
    if(updater)
    {
        updater->deleteLater();
    }
    updater=new QTimer();
    resolver->close();

    bool f=true;
    QByteArray bs;
    bs.append(filename);
    char path[1024]{0};
    for(int i=0;i<bs.size();i++)
    {
        path[i]=bs[i];
    }
    f&=resolver->open(path);
    AVStream *astr=resolver->demux->getAudioStream();
    AVStream *vstr=resolver->demux->getVideoStream();
    f&=resam->open(astr->codecpar);
    setVideoSize(vstr->codecpar->width, vstr->codecpar->height);
    aplayer->open(astr->codecpar->sample_rate, astr->codecpar->channels);
    audioCached=false;
    if(!f)
    {
        return;
    }
    emit durationRead(resolver->demux->getTotalMs());
    resolver->start();

    while(resolver->adpoll->status()<=0)
    {}

    connect(updater,SIGNAL(timeout()),
            this,SLOT(submitAudioFrame()));

    updater->setInterval(1);
    updater->setSingleShot(true);
    updater->start();
}

void FFWidget::submitAudioFrame()
{
    int rslen;
    uchar pcm[1024*1024]{0};
    QByteArray bs;
    AVFrame *afrm=nullptr;
    long long pts=0;
    if(audioCached) {
        if(aplayer->bytesFree()>=audioCachePcm.size()) {
            aplayer->write(audioCachePcm);
            pts=aplayer->getBufferPts();
            onAudioBufferPtsChanged(pts);
            audioCached=false;
            audioFramePts=audioCachePts;
        }
    }
    else {
        resolver->afsmutex.lock();
        while(!resolver->aframes.isEmpty()) {
            afrm=resolver->aframes.takeFirst();
            long long apts=afrm->pts;
            rslen=resam->resample(afrm,pcm);
            if(aplayer->bytesFree()>=rslen) {
                aplayer->write(reinterpret_cast<char*>(pcm),rslen);
                pts=aplayer->getBufferPts();
                audioFramePts=apts;
                onAudioBufferPtsChanged(pts);
            }
            else {
                audioCachePts=apts;
                audioCachePcm.clear();
                audioCachePcm.append(reinterpret_cast<char*>(pcm),rslen);
                audioCached=true;
                break;
            }
        }
        resolver->afsmutex.unlock();
    }
    updater->start();
}

void FFWidget::updateVideoFrame(AVFrame *frame)
{
    if(!frame)
    {
        return;
    }
    bool match;

    match=false;
    match|=(frame->width*frame->height<=0);
    match|=frame->width!=texWidth;
    match|=frame->height!=texHeight;
    match|=(!yTexData);
    match|=(!uTexData);
    match|=(!vTexData);

    if(match)
    {
        return;
    }

    for(int i=0;i<texHeight;i++)
    {
        memcpy(yTexData+texWidth*i,
               frame->data[0]+frame->linesize[0]*i, texWidth);
    }
    for(int i=0;i<texHeight/2;i++)
    {
        memcpy(uTexData+texWidth/2*i,
               frame->data[1]+frame->linesize[1]*i, texWidth/2);

        memcpy(vTexData+texWidth/2*i,
               frame->data[2]+frame->linesize[2]*i, texWidth/2);
    }

    av_frame_free(&frame);

    update();
    vfrmintv++;
    int clk1=clock();
    clkintv+=clk1-lastclk; if(clkintv>=1000) {speed=vfrmintv*1000/clkintv; vfrmintv=0; clkintv=0; qDebug() << "fps" << speed;}
    lastclk=clk1;
}

void FFWidget::onAudioBufferPtsChanged(long long pts)
{
    double aftb=resolver->demux->getAudioTimeBase();
    double vftb=resolver->demux->getVideoTimeBase();
    long long apts;
    AVFrame *vfrm=nullptr;
    AVFrame *frm=nullptr;
    QList<AVFrame*> vfrms;

    apts=audioFramePts-pts;

    resolver->vfsmutex.lock();
    while(resolver->vframes.size())
    {
        vfrm=resolver->vframes.takeFirst();
        if(vfrm->pts*vftb > apts*aftb+0.005)
        {
            resolver->vframes.prepend(vfrm);
            break;
        }
        vfrms.append(vfrm);
    }
    resolver->vfsmutex.unlock();
    if(vfrms.isEmpty())
    {
        updater->start();
        return;
    }
    for(int i=0;i<vfrms.size();i++)
    {
        frm=vfrms[i];
        updateVideoFrame(frm);
    }

    emit playProccessed(apts*aftb*1000);

    updater->start();
}

void FFWidget::seek(double ratio)
{
    if(!resolver->demux)
    {
        return;
    }
    AVPacket *pkt;

    resolver->dmutex.lock();
    qDebug() << "debug11" << resolver->demux->seek(ratio);
    resolver->dmutex.unlock();

    resolver->afsmutex.lock();
    resolver->adecoder->clear();
    while(resolver->apkts.size()>0)
    {
        pkt=resolver->apkts.takeFirst();
        av_packet_free(&pkt);
    }
    resolver->afsmutex.unlock();
}

void FFWidget::close()
{
    resolver->close();
}

void FFWidget::initializeGL()
{
    initializeOpenGLFunctions();

    program=new QOpenGLShaderProgram(this);

    QOpenGLShader *vsh=nullptr;
    vsh=new QOpenGLShader(QOpenGLShader::Vertex,this);
    qDebug() << "v"
             << vsh->compileSourceFile(":/glsl/vshader.glsl");

    QOpenGLShader *fsh=nullptr;
    fsh=new QOpenGLShader(QOpenGLShader::Fragment,this);
    qDebug() << "f"
             << fsh->compileSourceFile(":/glsl/fshader.glsl");

    program->addShader(vsh);
    program->addShader(fsh);
    program->link();
    program->bind();

    GLfloat vertices[] = {
        -1.0f,-1.0f,
         1.0f,-1.0f,
        -1.0f, 1.0f,
         1.0f, 1.0f,
    };

    GLfloat texcoords[]={
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
    };

    vBuffer.create();
    vBuffer.bind();
    vBuffer.allocate(2*8*sizeof(GLfloat));
    vBuffer.write(0,vertices,8*sizeof(GLfloat));
    vBuffer.write(8*sizeof(GLfloat),texcoords,8*sizeof(GLfloat));

    GLuint loc;
    loc=program->attributeLocation("qt_Vertex");
    program->setAttributeBuffer(loc,GL_FLOAT,0,2,0);
    program->enableAttributeArray(loc);

    loc=program->attributeLocation("qt_TexCoord");
    program->setAttributeBuffer(loc,GL_FLOAT,8*sizeof(GLfloat),2,0);
    program->enableAttributeArray(loc);

    yTex=new QOpenGLTexture(QOpenGLTexture::Target2D);
    uTex=new QOpenGLTexture(QOpenGLTexture::Target2D);
    vTex=new QOpenGLTexture(QOpenGLTexture::Target2D);

    setVideoSize(texWidth,texHeight);
}

void FFWidget::paintGL()
{
    program->setUniformValue("qt_YTex",1);
    yTex->setData(QOpenGLTexture::Red,QOpenGLTexture::UInt8,yTexData);
    yTex->bind(1);

    program->setUniformValue("qt_UTex",2);
    uTex->setData(QOpenGLTexture::Red,QOpenGLTexture::UInt8,uTexData);
    uTex->bind(2);

    program->setUniformValue("qt_VTex",3);
    vTex->setData(QOpenGLTexture::Red,QOpenGLTexture::UInt8,vTexData);
    vTex->bind(3);
    vBuffer.bind();
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
//    qDebug() << "paint";
}

void FFWidget::resizeGL(int w, int h)
{

}

void FFWidget::setVideoSize(int w, int h)
{
    this->texWidth=w;
    this->texHeight=h;

    delete[] yTexData;
    yTexData=NULL;

    delete[] uTexData;
    uTexData=NULL;

    delete[] vTexData;
    vTexData=NULL;

    yTexData=new char[texWidth*texHeight]{0};
    uTexData=new char[texWidth*texHeight/4]{0};
    vTexData=new char[texWidth*texHeight/4]{0};

    if(yTex)
    {
        yTex->destroy();
    }
    yTex->setMagnificationFilter(QOpenGLTexture::Linear);
    yTex->setMinificationFilter(QOpenGLTexture::Linear);
    yTex->setFormat(QOpenGLTexture::R8_UNorm);
    yTex->setSize(texWidth,texHeight);
    yTex->allocateStorage(QOpenGLTexture::Red,QOpenGLTexture::UInt8);
    qDebug() << "y tex create" << yTex->create();

    if(uTex)
    {
        uTex->destroy();
    }
    uTex->setMagnificationFilter(QOpenGLTexture::Linear);
    uTex->setMinificationFilter(QOpenGLTexture::Linear);
    uTex->setFormat(QOpenGLTexture::R8_UNorm);
    uTex->setSize(texWidth/2,texHeight/2);
    uTex->allocateStorage(QOpenGLTexture::Red,QOpenGLTexture::UInt8);
    qDebug() << "u tex create" << uTex->create();

    if(vTex)
    {
        vTex->destroy();
    }
    vTex->setMinificationFilter(QOpenGLTexture::Linear);
    vTex->setMagnificationFilter(QOpenGLTexture::Linear);
    vTex->setFormat(QOpenGLTexture::R8_UNorm);
    vTex->setSize(texWidth/2,texHeight/2);
    vTex->allocateStorage(QOpenGLTexture::Red,QOpenGLTexture::UInt8);
    qDebug() << "v tex create" << vTex->create();

    update();
}

FFWidget::~FFWidget()
{
    if(updater)
    {
        updater->stop();
    }
}
