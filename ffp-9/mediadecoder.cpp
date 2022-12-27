#include "mediadecoder.h"

MediaDecoder::MediaDecoder()
{

}

bool MediaDecoder::open(AVCodecParameters *par)
{
    close();

    if(!par)
    {
        return false;
    }
    int re=0;

    AVCodec *cd=avcodec_find_decoder(par->codec_id);
    if(!cd)
    {
//        avcodec_parameters_free(&par);
        qDebug() << "codec can't find" << par->codec_id;
        return false;
    }

    QMutexLocker locker(&mutex);
    cdctx=avcodec_alloc_context3(cd);

    avcodec_parameters_to_context(cdctx,par);

//    avcodec_parameters_free(&par);

    re=avcodec_open2(cdctx,NULL,NULL);
    qDebug() << "decoder time base" << cdctx->time_base.num
             << cdctx->time_base.den;
    if(re)
    {
        avcodec_free_context(&cdctx);
//        avcodec_parameters_free(&par);
        qDebug() << "codec open failed";
        return false;
    }
    qDebug() << "debug1" << cdctx->gop_size
             << cdctx->max_b_frames << cdctx->bit_rate
             << cdctx->sample_rate;
    return true;
}

void MediaDecoder::clear()
{
    QMutexLocker locker(&mutex);
    if(cdctx)
    {
        avcodec_flush_buffers(cdctx);
    }
}

void MediaDecoder::close()
{
    QMutexLocker locker(&mutex);
    if(cdctx)
    {
        avcodec_close(cdctx);
        avcodec_free_context(&cdctx);
    }
}

bool MediaDecoder::enqueue(AVPacket *pkt)
{
    QMutexLocker locker(&mutex);
    if(!pkt || pkt->size<=0 || !pkt->data)
    {
        return false;
    }
    if(!cdctx)
    {
        qDebug() << "debug2";
        return false;
    }
    int re=0;
    re=avcodec_send_packet(cdctx,pkt);
    av_packet_unref(pkt);
    av_packet_free(&pkt);
    if(re)
    {
        char errbuf[1024];
        av_strerror(re,errbuf,1024);
        qDebug() << "decoder send error" << errbuf;
        return false;
    }
    return true;
}

AVFrame* MediaDecoder::dequeue()
{
    QMutexLocker locker(&mutex);
    if(!cdctx)
    {
        qDebug() << "codec context null";
        return NULL;
    }
    int re=0;
    AVFrame *frm=av_frame_alloc();
    re=avcodec_receive_frame(cdctx,frm);
    if(re)
    {
        av_frame_free(&frm);
        char errbuf[1024];
        av_strerror(re,errbuf,1024);
//        qDebug() << "decoder recv error" << errbuf;
        return NULL;
    }
    return frm;
}

void MediaDecoder::flush()
{
    int re=0;
    re=avcodec_send_packet(cdctx,NULL);
    if(re)
    {
        char errbuf[1024];
        av_strerror(re,errbuf,1024);
        qDebug() << "decoder send error" << errbuf;
    }
}
