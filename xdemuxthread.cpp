#include "xdemuxthread.h"
#include "xdemux.h"
#include "xvideothread.h"
#include "xaudiothread.h"
XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
    wait();
}

bool XDemuxThread::open(const char *url, WId call)
{
    if(url == 0 || url[0] == '\0') return false;
    QMutexLocker locker(&m_mutex);
    if(!demux) demux = new XDemux();
    if(!vt) vt = new XVideoThread();
    if(!at) at = new XAudioThread();
    // 打开解封装
    bool re = demux->open(url);
    if(!re){
        qDebug()<<"demux open failed!";
        return false;
    }
    // 打开音视频解码器和处理线程
    if(!vt->open(demux->copyVParam(),call,demux->width,demux->height)) re = false;
    if(!at->open(demux->copyAParam(),demux->sampleRate,demux->channels)) re = false;
    totalMs = demux->totalS * 1000;
    return re;
}

void XDemuxThread::resizeShowScreen()
{
    if(vt) vt->resize();
}

void XDemuxThread::Start()
{
    QMutexLocker locker(&m_mutex);
    if (!demux) demux = new XDemux();
    if (!vt) vt = new XVideoThread();
    if (!at) at = new XAudioThread();
    QThread::start();
    // 启动当前线程(run)
    if(at) at->start();
    if(vt) vt->start();
}

void XDemuxThread::run()
{
    while(!threadExit){
        QMutexLocker locker(&m_mutex);
        if(isPause){
            msleep(5);
            continue;
        }
        if(!demux) {
            msleep(5);
            continue;
        }
        // 音视频同步
        if(vt && at){
            pts = at->pts;
            vt->synpts = at->pts;
        }
        AVPacket* pkt = demux->read();
        if(!pkt){
            msleep(500);
            continue;
        }
        // 判断数据是音频还是视频
        if(demux->isAudio(pkt)){
            if(at) at->push(pkt);
        }else{
            if(vt) vt->push(pkt);
        }
    }
    qDebug()<<"demux thread terminated!";
}

void XDemuxThread::close()
{
    QMutexLocker locker(&m_mutex);
    threadExit = true;
    if(vt) vt->close();
    if(at) at->close();
    delete vt;
    delete at;
    vt = nullptr;
    at = nullptr;
}

void XDemuxThread::clear()
{
    QMutexLocker locker(&m_mutex);
    if(demux) demux->clear();
    if(at) at->clear();
    if(vt) vt->clear();
}

void XDemuxThread::setPause(bool isPause)
{
    QMutexLocker locker(&m_mutex);
    this->isPause = isPause;
    if(at) at->setPause(isPause);
    if(vt) vt->setPause(isPause);
}

void XDemuxThread::seek(double pos)
{
    // 清理缓存
    clear();
    // 暂停
    bool status = this->isPause;
    setPause(true);
    //
    QMutexLocker locker(&m_mutex);
    if(demux) {
        demux->seek(pos);
    }
    long long seekPts = pos * demux->totalS * 1000;
    while(!threadExit){
        AVPacket* pkt = demux->readVideo();
        if(!pkt) break;
        if(vt->repaintPts(pkt,seekPts)){
            this->pts = seekPts;
            break;
        }
    }
    locker.unlock();
    if(!status) setPause(false); // 这里的setPause()会争夺m_mutex锁，和118行的锁冲突，存在重复加锁行为，产生死锁！
}
