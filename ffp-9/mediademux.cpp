#include "mediademux.h"

bool MediaDemux::avInited=false;

MediaDemux::MediaDemux()
{
    if(!avInited)
    {
        av_register_all();
        avformat_network_init();

        avInited=true;
        qDebug() << "demux inited";
    }
}

bool MediaDemux::open(const char *url)
{
    close();
//    QMutexLocker locker(&mutex);
    AVDictionary *opt=NULL;
    av_dict_set(&opt,"rtsp_transport","tcp",0);
    av_dict_set(&opt,"max_delay","500",0);

//    AVFormatContext *ic=NULL;

    int re=0;
    re=avformat_open_input(&ic,url,NULL,&opt);

    if(re!=0)
    {
        char errbuf[1024];
        av_strerror(re,errbuf,sizeof(errbuf));
        qDebug() << "open failed" << errbuf;
        return false;
    }
    else
    {
        qDebug() << "openned" << ic->duration/AV_TIME_BASE;
        avformat_find_stream_info(ic,NULL);

        for(unsigned int i=0;i<ic->nb_streams;i++)
        {
            AVStream *str=ic->streams[i];
            if(str->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
            {
                audioIdx=i;
                qDebug() << i << "audio:";
                qDebug() << "sample rate" << str->codecpar->sample_rate;
                qDebug() << "format" << str->codecpar->format;
                qDebug() << "channels" << str->codecpar->channels;
                qDebug() << "codec id" << str->codecpar->codec_id;
                qDebug() << "frame size" << str->codecpar->frame_size;
                qDebug() << "audio time base" << str->time_base.num
                         << str->time_base.den;
                qDebug() << "duration" << str->duration;
                continue;
            }
            if(str->codecpar->codec_type==AVMEDIA_TYPE_VIDEO)
            {
                videoIdx=i;
                qDebug() << i << "video:";
                qDebug() << "sample rate" << str->codecpar->sample_rate;
                qDebug() << "codec id" << str->codecpar->codec_id;
                qDebug() << "format" << str->codecpar->format;
                qDebug() << "width" << str->codecpar->width;
                qDebug() << "height" << str->codecpar->height;
                qDebug() << "fps" << str->avg_frame_rate.num
                         << str->avg_frame_rate.den;
                qDebug() << "video time base" << str->time_base.num
                         << str->time_base.den;
                qDebug() << "duration" << str->duration;
                continue;
            }
        }
    }
    av_dict_free(&opt);
    return true;
}

AVPacket* MediaDemux::read()
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return NULL;
    }
    AVPacket *pkt=av_packet_alloc();
    int re;
    re=av_read_frame(ic,pkt);
    if(re<0)
    {
        qDebug() << "read pkt failed";
        av_packet_free(&pkt) ;
    }

    return pkt;
}

AVStream* MediaDemux::getAudioStream()
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return NULL;
    }
    if(audioIdx<0)
    {
        return NULL;
    }
    return ic->streams[audioIdx];
}

AVStream* MediaDemux::getVideoStream()
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return NULL;
    }
    if(videoIdx<0)
    {
        return NULL;
    }
    return ic->streams[videoIdx];
}

bool MediaDemux::seek(double ratio)
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return false;
    }
    long long pos;
    int re;

    avformat_flush(ic);
    do
    {
        if(ic->streams[videoIdx]->duration>0)
        {
            pos=ratio*ic->streams[videoIdx]->duration;
            break;
        }
        if(ic->duration>0)
        {
            pos=ratio*ic->duration;
            break;
        }
    }while(0);

    re=av_seek_frame(ic,videoIdx,pos,
                     AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
    if(re<0)
    {
        return false;
    }
    return true;
}

void MediaDemux::clear()
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return;
    }

    avformat_flush(ic);
}

void MediaDemux::close()
{
//    QMutexLocker locker(&mutex);

    if(!ic)
    {
        return;
    }

    avformat_close_input(&ic);
    audioIdx=-1;
    videoIdx=-1;
}

double MediaDemux::getAudioTimeBase()
{
    if(audioIdx<0)
    {
        return 0.0;
    }
    return r2d(ic->streams[audioIdx]->time_base);
}

double MediaDemux::getVideoTimeBase()
{
    if(videoIdx)
    {
        return 0.0;
    }
    return r2d(ic->streams[videoIdx]->time_base);
}

long long MediaDemux::getTotalMs()
{
    if(!ic)
    {
        return -1;
    }
    return ic->duration/(AV_TIME_BASE/1000);
}

int MediaDemux::getAudioIndex()
{
    return audioIdx;
}

int MediaDemux::getVideoIndex()
{
    return videoIdx;
}
