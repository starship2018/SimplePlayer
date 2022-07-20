#ifndef XDECODE_H
#define XDECODE_H
#include <QMutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;

struct AVFrame;
void XFreePacket(AVPacket** pkt);
void XFreeFrame(AVFrame** frame);
class XDecode
{
public:
    XDecode();
    ~XDecode();
    // 打开解码器，不管成功与否都要释放para空间
    virtual bool open(AVCodecParameters* para);
    virtual void close();
    virtual void clear();
    // 当前解码到的pts
    long long pts = 0;
    // 发送到解码线程，不管成功与否都要释放pkt空间（对象和媒体内容）
    virtual bool send(AVPacket* pkt);

    // 获取解码后数据，一次send可能需要多次recv，获取缓冲中的数据，send null -> recv multitimes
    // 每次复制一份，由调用者释放av_frame_free
    virtual AVFrame* receive();
    AVCodecContext* codec = nullptr;
private:

    QMutex m_mutex;
};

#endif // XDECODE_H
