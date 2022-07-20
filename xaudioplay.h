#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H


class XAudioPlay
{
public:
    XAudioPlay();
    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;
    // 打开音频播放,不管成功与否都要清理！
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual void clear() = 0;
    // 播放音频
    virtual bool write(const unsigned char* data,int datasize) = 0;
    virtual int getfree() = 0;
    virtual void setPause(bool isPause) = 0;
    virtual ~XAudioPlay();
    static XAudioPlay* get();
    // 返回缓冲中还没有播放的时间ms
    virtual long long  getNoPlayMs() = 0;
};

#endif // XAUDIOPLAY_H
