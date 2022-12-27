#ifndef AUDIORESAMPLER_H
#define AUDIORESAMPLER_H

#include <QMutex>
#include <QDebug>
struct AVCodecParameters;
struct AVCodecContext;
struct SwrContext;

class AudioResampler
{
public:
    AudioResampler();
public:
    bool open(AVCodecParameters *par);
    void close();
    int resample(AVFrame *frm,unsigned char *pcm);
protected:
    QMutex mutex;
    SwrContext *swrctx=NULL;
};

#endif // AUDIORESAMPLER_H
