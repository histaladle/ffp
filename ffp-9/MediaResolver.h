#ifndef MEDIARESOLVER_H
#define MEDIARESOLVER_H

#include <QRunnable>
#include <QMutex>
#include "mediademux.h"
#include "mediadecoder.h"
#include "audioresampler.h"
#include "audioplayer.h"
#include <QThreadPool>

class DemuxPoll;
class VideoDecoderPoll;
class AudioDecoderPoll;

class MediaResolver
{
public:
    MediaResolver();

    bool open(char *url);
    void close();
    void start();
    bool valid=false;

    MediaDemux *demux=NULL;
    MediaDecoder *adecoder=NULL;
    MediaDecoder *vdecoder=NULL;


    QList<AVPacket*> apkts;
    QList<AVPacket*> vpkts;
    QList<AVFrame*> aframes;
    QList<AVFrame*> vframes;

    QThreadPool demuxRunner;
    QThreadPool audioRunner;
    QThreadPool videoRunner;

    DemuxPoll *dpoll;
    AudioDecoderPoll *adpoll;
    VideoDecoderPoll *vdpoll;

    QMutex afsmutex;
    QMutex vfsmutex;

    QMutex dmutex;
    QMutex amutex;
    QMutex vmutex;

    const int MAX_VIDEO_RUN_NUM=1;
    const int MAX_AUDIO_RUN_NUM=1;

    const int MAX_AUDIO_FRAME=10;

    long long lastapts=0;
};

class DemuxPoll : public QRunnable
{
public:
    DemuxPoll();
    MediaResolver *resolver=nullptr;
    void run();
private:
    int _command;
    QMutex commandMtx;
    int _status;
    QMutex statusMtx;
public:
    int command()
    {
        int c;
        commandMtx.lock();
        c=_command;
        commandMtx.unlock();
        return c;
    }
    int command(int cmd)
    {
        commandMtx.lock();
        _command=cmd;
        commandMtx.unlock();
        return cmd;
    }
    int status()
    {
        int s;
        statusMtx.lock();
        s=_status;
        statusMtx.unlock();
        return s;
    }
};

class VideoDecoderPoll : public QRunnable
{
public:
    VideoDecoderPoll();
    MediaResolver *parser=nullptr;
    AVPacket *pkt=nullptr;
    void run();
private:
    int _command;
    QMutex commandMtx;
    int _status;
    QMutex statusMtx;
public:
    int command()
    {
        int c;
        commandMtx.lock();
        c=_command;
        commandMtx.unlock();
        return c;
    }
    int command(int cmd)
    {
        commandMtx.lock();
        _command=cmd;
        commandMtx.unlock();
        return cmd;
    }
    int status()
    {
        int s;
        statusMtx.lock();
        s=_status;
        statusMtx.unlock();
        return s;
    }
};

class AudioDecoderPoll : public QRunnable
{
public:
    AudioDecoderPoll();
    MediaResolver *parser=nullptr;
    AVPacket *pkt=nullptr;
    void run();
private:
    int _command;
    QMutex commandMtx;
    int _status;
    QMutex statusMtx;
public:
    int command()
    {
        int c;
        commandMtx.lock();
        c=_command;
        commandMtx.unlock();
        return c;
    }
    int command(int cmd)
    {
        commandMtx.lock();
        _command=cmd;
        commandMtx.unlock();
        return cmd;
    }
    int status()
    {
        int s;
        statusMtx.lock();
        s=_status;
        statusMtx.unlock();
        return s;
    }
};

#endif // MEDIARESOLVER_H
