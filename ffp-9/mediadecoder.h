#ifndef MEDIADECODER_H
#define MEDIADECODER_H

#include <QMutex>
#include <QDebug>

extern "C"
{
#include <libavcodec/avcodec.h>
}
class MediaDecoder
{
public:
    MediaDecoder();
public:
    bool open(AVCodecParameters *par);
    void close();
    void clear();
    bool enqueue(AVPacket *pkt);
    AVFrame* dequeue();
    void flush();
protected:
    AVCodecContext *cdctx=NULL;
    QMutex mutex;
};

#endif // MEDIADECODER_H
