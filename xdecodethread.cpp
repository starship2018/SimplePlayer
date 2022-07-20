#include "xdecodethread.h"
#include "xdecode.h"
XDecodeThread::XDecodeThread()
{
    if(!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread()
{
    threadExit = true;
    if(decode) delete decode;
    wait();
}

void XDecodeThread::push(AVPacket *pkt)
{
    if(!pkt) return;
    while(!threadExit){
        QMutexLocker locker(&m_mutex);
        if(packs.size() < maxList){
            packs.push_back(pkt);
            break;
        }
        msleep(1);
    }
}

void XDecodeThread::clear()
{
    QMutexLocker locker(&m_mutex);
    decode->clear();
    while(!packs.empty()){
        AVPacket* pkt = packs.front();
        XFreePacket(&pkt);
        packs.pop_front();
    }
}

void XDecodeThread::close()
{
    clear();
    threadExit = true;
    decode->close();
    delete decode;
    decode = nullptr;
}


AVPacket *XDecodeThread::pop()
{
    QMutexLocker locker(&m_mutex);
    if(packs.empty()){
        return nullptr;
    }
    AVPacket* pkt = packs.front();
    packs.pop_front();
    return pkt;
}
