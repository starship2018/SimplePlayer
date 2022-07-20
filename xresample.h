#ifndef XRESAMPLE_H
#define XRESAMPLE_H
#include <QMutex>
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
class XResample
{
public:
    XResample();
    // 输出参数和输入参数一致，除了采样格式，输出格式为S16
    virtual bool open(AVCodecParameters* para, bool isClear = false);
    virtual bool close();
    // 返回重采样后大小
    virtual int resample(AVFrame* indata,unsigned char* data);
    //AV_SAMPLE_FMT_S16
    int outFormat = 1;
protected:
    QMutex m_mutex;
    SwrContext* actx = nullptr;
};

#endif // XRESAMPLE_H
