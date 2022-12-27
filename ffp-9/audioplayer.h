#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QDebug>
#include <QTime>
class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    AudioPlayer(QObject *parent=Q_NULLPTR);
    ~AudioPlayer();
public:
    bool open(int rate, int channels);
    void close();
    qint64 write(char *buf,int len,bool block=true);
    qint64 write(QByteArray buf,bool block=true);
    long long getBufferMs();
    long long getBufferPts();
    int bytesFree();
private:

    QAudioFormat afmt;
    QAudioOutput *aout=NULL;
    QIODevice *aio=NULL;

    QMutex mutex;
    QTime delay;

    const int MAX_WRITE_TIME=1000;
};

#endif // AUDIOPLAYER_H
