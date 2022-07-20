#include "xaudiothread.h"
#include "xdecode.h"
#include "xresample.h"
#include "xaudioplay.h"

#include <QDebug>
XAudioThread::XAudioThread()
{
    if(!res) res = new XResample();
    if(!ap) ap = XAudioPlay::get();
}

XAudioThread::~XAudioThread()
{
    wait();
}

void XAudioThread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
    while(!threadExit){
        QMutexLocker locker(&m_aMutex);
        if(isPause){
            locker.unlock();
            msleep(5);
            continue;
        }
        AVPacket* pkt = pop();
        if(!decode->send(pkt)){
            msleep(1);
            continue;
        }
        while (!threadExit) {
            AVFrame* frame = decode->receive();
            if(!frame) break;
            pts = decode->pts - ap->getNoPlayMs();
            //qDebug()<<"audio thread pts = "<<pts;
            // 重采样
            int size = res->resample(frame,pcm);
            // 播放音频
            while (!threadExit) {
                if(size <= 0) break;
                if(ap->getfree() < size || isPause){
                    // 一帧音频帧的写入也需要耗时，这会导致暂停不及时
                    msleep(1);
                    continue;
                }
                ap->write(pcm,size);
                break;
            }
        }

    }
    delete[] pcm;
    qDebug()<<"audio thread terminated!";
}

bool XAudioThread::open(AVCodecParameters *para,int sampleRate,int channels)
{
    clear();
    if(!para) return false;
    QMutexLocker locker(&m_aMutex);
    pts = 0;
    bool re = true;
    // 重采样器设置
    if(!res->open(para,false)){
        qDebug()<<"XResample open failed!";
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if(!ap->open()) {
        qDebug()<<"XAudioPlay open failed!";
        re =  false;
    }
    // 通用解码器设置
    if(!decode->open(para)) {
        qDebug()<<"XDecode open failed!";
        re = false;
    }
    qDebug()<<"audio thread open succeed!";
    return re;
}

void XAudioThread::close()
{
    XDecodeThread::close();
    if(res){
        res->close();
        delete  res;
        QMutexLocker locker(&m_aMutex);
        res = nullptr;
    }
    if(ap){
        ap->close();
        QMutexLocker locker(&m_aMutex);
        ap = nullptr;
    }
    //threadExit = true;
}

void XAudioThread::clear()
{
    XDecodeThread::clear();
    QMutexLocker locker(&m_mutex);
    if(ap) ap->clear();
}

void XAudioThread::setPause(bool isPause)
{
    //QMutexLocker locker(&m_aMutex);????
    this->isPause = isPause;
    if(ap) ap->setPause(isPause);
}
