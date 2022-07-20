#include "xaudioplay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
class CXAudioPlay : public XAudioPlay{
public:
    QAudioOutput* output = nullptr;
    QIODevice* io = nullptr;
    QMutex m_mutex;

    virtual bool write(const unsigned char* data,int datasize){
        if(!data || datasize <= 0) return false;
        QMutexLocker locker(&m_mutex);
        if(!output || !io) return false;
        int size = io->write((char*)data,datasize);
        if(size != datasize) return false;
        return true;
    }

    virtual long long getNoPlayMs(){
        QMutexLocker locker(&m_mutex);
        if(!output) return 0;
        long long pts = 0;
        // 还没播放的字节数
        double size = output->bufferSize() - output->bytesFree();
        // 1s音频字节大小
        double secSize = sampleRate * (sampleSize / 8) * channels;
        if(secSize <= 0){
            pts = 0;
        }else {
            pts = size / secSize * 1000;
        }
        return pts;
    }

    virtual int getfree(){
        QMutexLocker locker(&m_mutex);
        if(!output) return 0;
        int free = output->bytesFree();
        return free;
    }
    virtual bool open(){
        close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleSize(sampleSize);
        fmt.setChannelCount(channels);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        QMutexLocker locker(&m_mutex);
        output = new QAudioOutput(fmt);
        io = output->start(); //开始播放
        if(io) return true;
        return false;
    }
    virtual bool close(){
        QMutexLocker locker(&m_mutex);
        if(io){
            io->close();
            io = nullptr;
        }
        if(output){
            output->stop();
            delete output;
            output = nullptr;
        }
        return true;
    }

    virtual void setPause(bool isPause)
    {
       QMutexLocker locker(&m_mutex);
       if(!output) return;
       if(isPause){
           output->suspend();
       }else{
           output->resume();
       }
    }

    void clear()
    {
        QMutexLocker locker(&m_mutex);
        if(io) io->reset();
    }

};

XAudioPlay::XAudioPlay()
{

}




XAudioPlay::~XAudioPlay()
{

}

XAudioPlay *XAudioPlay::get()
{
    static CXAudioPlay play;
    return &play;
}
