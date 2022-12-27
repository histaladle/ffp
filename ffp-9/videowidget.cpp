#include "videowidget.h"

extern "C"{
#include <libavutil/frame.h>
}

VideoWidget::VideoWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

void VideoWidget::initializeGL()
{
    initializeOpenGLFunctions();

    program=new QOpenGLShaderProgram(this);

    QOpenGLShader *vshader = NULL;
    vshader = new QOpenGLShader(QOpenGLShader::Vertex,this);
    qDebug() << "v"
             << vshader->compileSourceFile(":/glsl/vshader.glsl");

    QOpenGLShader *fshader= NULL;
    fshader = new QOpenGLShader(QOpenGLShader::Fragment,this);
    qDebug() << "f"
             << fshader->compileSourceFile(":/glsl/fshader.glsl");

    program->addShader(vshader);
    program->addShader(fshader);
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

//    playTim.setInterval(40);
//    connect(&playTim,SIGNAL(timeout()),this,SLOT(update()));
    qDebug() << "gl inited";
//    playTim.start();
}

void VideoWidget::paintGL()
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
    qDebug() << "paint";
}

void VideoWidget::resizeGL(int w, int h)
{

}

void VideoWidget::setVideoSize(int w, int h)
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

void VideoWidget::updateFrame(AVFrame *frm)
{
    if(!frm)
    {
        return;
    }

    bool match;

    match=false;
    match|=(frm->width*frm->height<=0);
    match|=frm->width!=texWidth;
    match|=frm->height!=texHeight;
    match|=(!yTexData);
    match|=(!uTexData);
    match|=(!vTexData);

    if(match)
    {
        return;
    }

//    memcpy(yTexData,frm->data[0],texWidth*texHeight);
//    memcpy(uTexData,frm->data[1],texWidth*texHeight/4);
//    memcpy(vTexData,frm->data[2],texWidth*texHeight/4);

    for(int i=0;i<texHeight;i++)
    {
        memcpy(yTexData+texWidth*i,
               frm->data[0]+frm->linesize[0]*i, texWidth);
    }
    for(int i=0;i<texHeight/2;i++)
    {
        memcpy(uTexData+texWidth/2*i,
               frm->data[1]+frm->linesize[1]*i, texWidth/2);

        memcpy(vTexData+texWidth/2*i,
               frm->data[2]+frm->linesize[2]*i, texWidth/2);
    }

    av_frame_free(&frm);

    update();
}

void VideoWidget::onFrameUpdated(AVFrame *frm)
{
    QMutexLocker locker(&mutex);
    updateFrame(frm);
}

void VideoWidget::onVideoSizeChanged(int w, int h)
{
    setVideoSize(w,h);
}
