#include "audioplaythread.h"
#include "audioplayer.h"
AudioPlayThread::AudioPlayThread(QObject *parent) : QObject(parent)
{
    thread=new QThread();
    moveToThread(thread);
    thread->start();
    connect(this,SIGNAL(inited()),this,SLOT(onInited()));
    emit inited();
}

void AudioPlayThread::onInited()
{
    aplayer=new AudioPlayer();
}

void AudioPlayThread::onByteSubmitted(QByteArray bytes)
{
    if(!aplayer)
    {
        return;
    }
    aplayer->write(bytes);

    long long pts=aplayer->getBufferPts();
    emit bufferPtsChanged(pts);
}

void AudioPlayThread::onOpenSubmitted(int rate, int channels)
{
    if(!aplayer)
    {
        return;
    }
    aplayer->open(rate, channels);
}

void AudioPlayThread::onCloseSubmitted()
{
    if(!aplayer)
    {
        return;
    }
    aplayer->close();
}

AudioPlayThread::~AudioPlayThread()
{
    delete aplayer;
}
