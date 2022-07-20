#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H
#include <QThread>
#include <QMutex>
#include "xdecodethread.h"
struct AVCodecParameters;
struct SDL_AudioSpec;
class XAudioPlay;
class XResample;
class XAudioThread : public XDecodeThread
{
public:
    XAudioThread();
    virtual ~XAudioThread();
    void run();
    // 当前音频播放的pts
    long long pts = 0;
    virtual bool open(AVCodecParameters* para,int sampleRate,int channels);
    virtual void close();
    virtual void clear();
    void setPause(bool isPause);
protected:
    XAudioPlay* ap = nullptr;
    XResample* res = nullptr;
    QMutex m_aMutex;

    bool isPause = false;
};

#endif // XAUDIOTHREAD_H
