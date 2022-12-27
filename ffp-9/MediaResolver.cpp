#include "MediaResolver.h"

MediaResolver::MediaResolver() : dpoll(new DemuxPoll()),adpoll(new AudioDecoderPoll()),vdpoll(new VideoDecoderPoll())
{
    demux=new MediaDemux();
    adecoder=new MediaDecoder();
    vdecoder=new MediaDecoder();
}

bool MediaResolver::open(char *url)
{
    bool f=true;
    f&=demux->open(url);
    AVStream *astr=demux->getAudioStream();
    AVStream *vstr=demux->getVideoStream();
//    double aftb=demux->getAudioTimeBase();
//    double vftb=demux->getVideoTimeBase();

    f&=adecoder->open(astr->codecpar);
    f&=vdecoder->open(vstr->codecpar);

    valid=f;
    return f;
}

void MediaResolver::start()
{
    if(!valid)
    {
        return;
    }
    dpoll->resolver=this;
    dpoll->command(1);
    demuxRunner.start(dpoll);
}

void MediaResolver::close()
{
    dmutex.lock();
    dpoll->command(0);
    demuxRunner.waitForDone();
    dmutex.unlock();

    AVFrame *frm=nullptr;
    while(adpoll->status()>0)
    {}
    for(int i=0;i<aframes.size();i++)
    {
        frm=aframes[i];
        av_frame_free(&frm);
    }
    aframes.clear();

    while(vdpoll->status()>0)
    {}
    for(int i=0;i<vframes.size();i++)
    {
        frm=vframes[i];
        av_frame_free(&frm);
    }
    vframes.clear();
}

/*
 * DemuxRunnable
 */
DemuxPoll::DemuxPoll()
    :QRunnable()
    ,_command(0)
    ,_status(0)
{
    setAutoDelete(false);
}

void DemuxPoll::run()
{
    AVPacket *pkt=nullptr;
    int afn=0;
    if(command()<=0) return;
    statusMtx.lock(); _status=1; statusMtx.unlock();
    do{
        do{
            resolver->afsmutex.lock();
            afn=resolver->aframes.size();
            resolver->afsmutex.unlock();
        }while(afn >= resolver->MAX_AUDIO_FRAME);

        resolver->dmutex.lock();
        pkt=resolver->demux->read();
        if(!pkt||command()<=0)
        {
            while(resolver->adpoll->command()>0){}
            resolver->adpoll->parser=resolver;
            resolver->adpoll->pkt=Q_NULLPTR;
            resolver->adpoll->command(1);
            resolver->audioRunner.start(resolver->adpoll);

            while(resolver->vdpoll->command()>0){}
            resolver->vdpoll->parser=resolver;
            resolver->vdpoll->pkt=Q_NULLPTR;
            resolver->vdpoll->command(1);
            resolver->videoRunner.start(resolver->vdpoll);

            break;
        }
        else if(pkt->stream_index==resolver->demux->getAudioIndex())
        {
            while(resolver->adpoll->command()>0){}
            resolver->adpoll->parser=resolver;
            resolver->adpoll->pkt=pkt;
            resolver->adpoll->command(1);
            resolver->audioRunner.start(resolver->adpoll);
        }
        else if(pkt->stream_index==resolver->demux->getVideoIndex())
        {
            while(resolver->vdpoll->command()>0){}
            resolver->vdpoll->parser=resolver;
            resolver->vdpoll->pkt=pkt;
            resolver->vdpoll->command(1);
            resolver->videoRunner.start(resolver->vdpoll);
        }
        resolver->dmutex.unlock();
    }while(pkt);
    statusMtx.lock(); _status=0; statusMtx.unlock();
    command(0);
}

/*
 * AudioRunnable
 */
AudioDecoderPoll::AudioDecoderPoll()
    :QRunnable()
    ,_command(0)
    ,_status(0)
{
    setAutoDelete(false);
}

void AudioDecoderPoll::run()
{
    AVFrame *afrm=nullptr;
    QList<AVFrame*> afrms;
    if(command()<=0) return;
    statusMtx.lock(); _status=1; statusMtx.unlock();
    parser->amutex.lock();
    parser->adecoder->enqueue(pkt);
    do{
        afrm=parser->adecoder->dequeue();
        if(afrm)
        {
            afrms.append(afrm);
        }
    }while(afrm);
    parser->amutex.unlock();

    parser->afsmutex.lock();
    parser->aframes.append(afrms);
    parser->afsmutex.unlock();

    statusMtx.lock(); _status=0; statusMtx.unlock();
    command(0);
}

/*
 * VideoRunnable
 */
VideoDecoderPoll::VideoDecoderPoll()
    :QRunnable()
    ,_command(0)
    ,_status(0)
{
    setAutoDelete(false);
}

void VideoDecoderPoll::run()
{
    AVFrame *vfrm=nullptr;
    QList<AVFrame*> vfrms;
    if(command()<=0) return;
    statusMtx.lock(); _status=1; statusMtx.unlock();
    parser->vmutex.lock();
    parser->vdecoder->enqueue(pkt);
    do{
        vfrm=parser->vdecoder->dequeue();
        if(vfrm)
        {
            vfrms.append(vfrm);
        }
    }while(vfrm);
    parser->vmutex.unlock();

    parser->vfsmutex.lock();
    parser->vframes.append(vfrms);
    parser->vfsmutex.unlock();

    statusMtx.lock(); _status=0; statusMtx.unlock();
    command(0);
}
