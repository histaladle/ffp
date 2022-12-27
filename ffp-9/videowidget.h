#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

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

struct AVFrame;

class VideoWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    VideoWidget(QWidget *parent=NULL);
public:
    void setVideoSize(int w, int h);
    void updateFrame(AVFrame *frm);
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

    QMutex mutex;
private slots:
    void onFrameUpdated(AVFrame *frm);
    void onVideoSizeChanged(int w,int h);
signals:
    void glInited();
};

#endif // VIDEOWIDGET_H
