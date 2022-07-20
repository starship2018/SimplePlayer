#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H
#include <QThread>
#include <QMutex>
#include <QLabel>
#include "ivideocall.h"

class XDemux;
class XVideoThread;
class XAudioThread;
class XDemuxThread : public QThread
{
public:
    XDemuxThread();
    virtual ~XDemuxThread();
    virtual bool open(const char* url,WId call);
    void resizeShowScreen();
    // 启动所有线程！
    virtual void Start();
    virtual void run();
    virtual void close();
    virtual void clear();
    bool isPause = false;
    void setPause(bool isPause);
    long long pts = 0;
    long long totalMs = 0;
    void seek(double pos);
protected:
    QMutex m_mutex;
    bool threadExit = false;
    XDemux* demux = nullptr;
    XVideoThread* vt = nullptr;
    XAudioThread* at = nullptr;
};

#endif // XDEMUXTHREAD_H
