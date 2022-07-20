#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H
#include <QThread>
#include <QMutex>
#include <QLabel>
#include "ivideocall.h"
#include "xdecodethread.h"
//解码和显示视频
extern "C"{
#include <SDL.h>
}

struct AVCodecParameters;
class XDecode;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;
class XVideoThread : public XDecodeThread
{
public:
    XVideoThread();
    ~XVideoThread();
    //打开，不管成功与否都清理
    virtual bool open(AVCodecParameters *para,WId call,int width,int height);
    virtual void close();
    virtual bool repaintPts(AVPacket* pkt,long long seekpts);
    void run();
    long long synpts = 0;  // 同步时间由外部传入
    void setPause(bool isPause);
    void resize();
protected:
    QMutex m_vMutex;
    long long pts = 0;   // 播放时间
    bool isPause = false;

private:
    SDL_Window* sdlWind = nullptr;
    SDL_Renderer* sdlRender = nullptr;
    SDL_Texture* sdlTexture = nullptr;
    SDL_Rect sdlRect;
    QWidget* show = nullptr;
    //-- 分辨率调整
    struct SwsContext* img_convert_ctx = nullptr;
    uint8_t* pYUVFrame_data[4] = {nullptr};
    int pYUVFrame_linesize[4] = {0};
    AVFrame* newFrame = nullptr;
    int dst_w, dst_h;
    int src_w, src_h;
    WId showID;
};

#endif // XVIDEOTHREAD_H
