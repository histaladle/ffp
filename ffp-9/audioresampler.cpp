extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}
#include "audioresampler.h"

AudioResampler::AudioResampler()
{

}

bool AudioResampler::open(AVCodecParameters *par)
{
    QMutexLocker locker(&mutex);
    if(!par)
    {
        return false;
    }
    int re=0;

    if(!swrctx)
    {
        swrctx=swr_alloc();
    }

    swrctx=swr_alloc_set_opts(
                swrctx,
                av_get_default_channel_layout(par->channels),
                AV_SAMPLE_FMT_S16,
                par->sample_rate,
                av_get_default_channel_layout(par->channels),
                (AVSampleFormat)par->format,
                par->sample_rate,0,NULL);
//    avcodec_parameters_free(&par);
    re=swr_init(swrctx);
    if(re)
    {
        return false;
    }
    return true;
}

int AudioResampler::resample(AVFrame *frm, unsigned char *pcm)
{
    QMutexLocker locker(&mutex);
    if(!frm)
    {
        return 0;
    }
    if(!pcm)
    {
        av_frame_free(&frm);
        return 0;
    }

    int chn=0;
    unsigned char *data[2]={pcm,NULL};
    chn=swr_convert(
                swrctx,data,
                frm->nb_samples,
                (const unsigned char**)frm->data,
                frm->nb_samples);
    if(chn<=0)
    {
        return chn;
    }

    int l=chn*frm->channels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    av_frame_free(&frm);
    return l;
}

void AudioResampler::close()
{
    QMutexLocker locker(&mutex);
    if(swrctx)
    {
        swr_free(&swrctx);
    }
}
