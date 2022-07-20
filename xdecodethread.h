#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H
#include <QThread>
#include <QMutex>
#include "xdecode.h"
struct AVPacket;
class XDecode;
class XDecodeThread : public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
    virtual void push(AVPacket* pkt);
    //清理队列
    virtual void clear();
    // 清理资源，停止线程
    virtual void close();
    //取出一帧数据，并出栈，如果没有返回NULL
    virtual AVPacket* pop();
    //最大队列
    int maxList = 100;
    bool threadExit = false;
    XDecode *decode = nullptr;
protected:
    QList <AVPacket *> packs;
    QMutex m_mutex;

};

#endif // XDECODETHREAD_H
