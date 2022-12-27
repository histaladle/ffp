#include "audioplayer.h"
#include <QThread>
AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{

}

bool AudioPlayer::open(int rate, int channels)
{
    close();
//    qDebug() << "debug1" << rate << channels;
    QMutexLocker locker(&mutex);

    afmt.setSampleRate(rate);
    afmt.setSampleSize(16);
    afmt.setSampleType(QAudioFormat::UnSignedInt);
    afmt.setByteOrder(QAudioFormat::LittleEndian);
    afmt.setChannelCount(channels);
    afmt.setCodec("audio/pcm");

    aout=new QAudioOutput(afmt);
    aio=aout->start();

    if(aio)
    {
        return true;
    }
    return false;
}

qint64 AudioPlayer::write(QByteArray buf, bool block)
{
    QMutexLocker locker(&mutex);
    int len=buf.size();
    if(len<=0||!aio||!aout)
    {
        return 0;
    }

    qint64 wlen=0;

    if(block)
    {
        delay.start();
        while(aout->bytesFree()<len)
        {
            if(delay.msec()>MAX_WRITE_TIME)
            {
                break;
            }
        }
        if(delay.elapsed()<MAX_WRITE_TIME)
        {
            wlen=aio->write(buf);
        }

        return wlen;
    }
    else
    {
        if(aout->bytesFree()>=len)
        {
            wlen=aio->write(buf);
            return wlen;
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

qint64 AudioPlayer::write(char *buf, int len, bool block)
{
    QMutexLocker locker(&mutex);
    if(!buf||len<=0||!aio||!aout)
    {
        return 0;
    }

    qint64 wlen=0;

    if(block)
    {
        delay.start();
        while(aout->bytesFree()<len)
        {
            if(delay.msec()>MAX_WRITE_TIME)
            {
                break;
            }
        }
        if(delay.elapsed()<MAX_WRITE_TIME)
        {
            wlen=aio->write(buf,len);
        }

        return wlen;
    }
    else
    {
        if(aout->bytesFree()>=len)
        {
            wlen=aio->write(buf,len);
            return wlen;
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

void AudioPlayer::close()
{
    QMutexLocker locker(&mutex);
    if(aio)
    {
        aio->close();
        aio=NULL;
    }
    if(aout)
    {
        aout->stop();
        aout->deleteLater();
        aout=NULL;
    }
}

long long AudioPlayer::getBufferMs()
{
    double secsize=0;
    secsize=afmt.sampleRate()*afmt.sampleSize()/8*afmt.channelCount();
    double size=0;
    size=aout->bufferSize()-aout->bytesFree();

    long long pts=0;
    if(secsize)
    {
        pts=size/secsize*1000;
    }
    return pts;
}

long long AudioPlayer::getBufferPts()
{
    double secsize=0;
    secsize=afmt.sampleRate()*afmt.sampleSize()/8*afmt.channelCount();
    double size=0;
    size=aout->bufferSize()-aout->bytesFree();

    long long pts=0;
    if(secsize)
    {
        pts=size*afmt.sampleRate()/secsize;
    }
    return pts;
}

int AudioPlayer::bytesFree()
{
    int n=0;
    if(aout) {
        n=aout->bytesFree();
    }
    return n;
}

AudioPlayer::~AudioPlayer()
{
    if(aout)
    {
        aout->deleteLater();
    }
    if(aio)
    {
        aio->deleteLater();
    }
}
