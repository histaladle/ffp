#ifndef AUDIOPLAYTHREAD_H
#define AUDIOPLAYTHREAD_H

#include <QObject>
#include <QThread>
class AudioPlayer;
class AudioPlayThread : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayThread(QObject *parent = 0);
    ~AudioPlayThread();
private:
    QThread *thread=nullptr;
    AudioPlayer *aplayer=nullptr;
signals:
    void inited();
    void bufferPtsChanged(long long pts);
public slots:
    void onInited();
    void onByteSubmitted(QByteArray bytes);
    void onOpenSubmitted(int rate, int channels);
    void onCloseSubmitted();
};

#endif // AUDIOPLAYTHREAD_H
