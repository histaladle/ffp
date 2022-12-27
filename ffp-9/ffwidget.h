#ifndef FFWIDGET_H
#define FFWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QDebug>
#include <QOpenGLTexture>
#include <QFile>
#include <QTimer>
#include <QMutex>
#include <QThreadPool>
#include <QPainter>
#include "MediaResolver.h"

struct AVFrame;

class FFWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    FFWidget(QWidget *parent=nullptr);
    ~FFWidget();
    void open(QString filename);
    void close();
    void seek(double ratio);

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);
private:
    int texWidth=320;
    int texHeight=240;
    QOpenGLShaderProgram *program=NULL;
    QOpenGLBuffer vBuffer;
//    GLuint yTexId=0,uTexId=0,vTexId=0;
//    GLuint yTexLoc=0,uTexLoc=0,vTexLoc=0;

    QOpenGLTexture *yTex=NULL,*uTex=NULL,*vTex=NULL;
    char *yTexData=NULL,*uTexData=NULL,*vTexData=NULL;

    QFile file;


    MediaResolver *resolver=nullptr;
    AudioResampler *resam=nullptr;
    AudioPlayer *aplayer=nullptr;
    long long audioFramePts=0;

    void setVideoSize(int w, int h);

    QTimer *updater=nullptr;
    QList<AVFrame*> vframes;

    int lastclk=0;
    int vfrmintv=0;
    int clkintv=0;
    int speed=0;
private:
    bool audioCached=false;
    long long audioCachePts=0;
    QByteArray audioCachePcm;
signals:
    void durationRead(long long dur);
    void playProccessed(long long ms);
private slots:
    void onAudioBufferPtsChanged(long long pts);
    void submitAudioFrame();
    void updateVideoFrame(AVFrame *frame);
};

#endif // FFWIDGET_H
