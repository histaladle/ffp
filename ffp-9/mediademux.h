#ifndef MEDIADEMUX_H
#define MEDIADEMUX_H

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
extern "C"{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}
#include "r2d.h"
class MediaDemux
{
public:
    MediaDemux();
public:
    bool open(const char *url);
    AVPacket *read();
    AVStream* getAudioStream();
    AVStream* getVideoStream();
    bool seek(double ratio);
    void clear();
    void close();
    int getAudioIndex();
    int getVideoIndex();
    double getAudioTimeBase();
    double getVideoTimeBase();
    long long getTotalMs();
protected:
    static bool avInited;
    AVFormatContext *ic=NULL;
    int audioIdx=-1,videoIdx=-1;
//    QMutex mutex;
};

#endif // MEDIADEMUX_H
