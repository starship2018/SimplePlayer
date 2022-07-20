#ifndef XDEMUX_H
#define XDEMUX_H
#include <QMutex>
#include <QDebug>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class XDemux
{
public:
    XDemux();
    ~XDemux();
    virtual bool open(const char* url);
    // 空间需要调用者释放，释放AVPacket对象空间和数据空间
    virtual AVPacket* read();
    // 获取视频参数 返回的空间需要清理 avcodec_parameters_free
    AVCodecParameters* copyVParam();
    // 获取音频参数 返回的空间需要清理 avcodec_parameters_free
    AVCodecParameters* copyAParam();
    // 只读视频，音频丢弃空间释放
    AVPacket* readVideo();
    // seek position 0.0~1.0
    virtual bool seek(double pos);
    bool isAudio(AVPacket* pkt);
    // 清空读取缓存
    virtual void clear();
    virtual void close();

    // 媒体总时长！
    int totalS = 0;
    int width = 0;
    int height = 0;
    int sampleRate = 0;
    int channels = 0;
protected:
    AVFormatContext* ic = nullptr;
    // 音视频索引
    int videoStream = 0;
    int audioStream = 1;
private:
    QMutex m_mutex;
};

#endif // XDEMUX_H
