#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include "xvideothread.h"
#include "xdecode.h"
extern "C"
{
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

//#define TEST

XVideoThread::XVideoThread()
{
    // 1.初始化SDL！
    if(SDL_Init(SDL_INIT_VIDEO) < 0 ){
        qDebug()<<"SDL could not initialized with error : "<<SDL_GetError();
    }
    qDebug()<<"SDL init succeed";
}

XVideoThread::~XVideoThread()
{
    if(img_convert_ctx) sws_freeContext(img_convert_ctx);
    if(sdlRender) SDL_DestroyRenderer(sdlRender);
    if(sdlTexture) SDL_DestroyTexture(sdlTexture);
    if(sdlWind) SDL_DestroyWindow(sdlWind);
    wait();
}

bool XVideoThread::open(AVCodecParameters *para,WId call,int src_w,int src_h)
{
    //qDebug()<<"src wid*hgt "<<src_w<<"*"<<src_h;
    clear();
    if(!para) return false;
    QMutexLocker locker(&m_vMutex);
    this->src_w = src_w;
    this->src_h = src_h;

    showID = call;
    synpts = 0;
    if(call){
        // videoWgt根据解码器获取的媒体信息宽高自行修改宽高以适应播放！
        show = QWidget::find(call);
        dst_w = show->geometry().width();
        dst_h = show->geometry().height();
    }else{
        QMessageBox::warning(nullptr,"Error","Play Widget Error!");
    }
    int ret = true;
    if(!decode->open(para)){
        qDebug()<<"video decode open failed!";
        ret = false;
    }

    if(sdlWind) SDL_DestroyWindow(sdlWind);
    // 2.创建用于显示的窗口或者依附于已经存在的窗口！
    sdlWind = SDL_CreateWindowFrom(reinterpret_cast<void*>(call));
    if(!sdlWind){
        qDebug()<<"SDL_CreateWindowFrom failed!";
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // 3.创建渲染器，将图像或者视频帧汇聚到2中的窗口之上！SDL_RENDERER_ACCELERATED为启动硬件加速！
    sdlRender = SDL_CreateRenderer(sdlWind,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // 4.设置显示矩形框的大小和位置
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = dst_w;
    sdlRect.h = dst_h;

    // 5.通过渲染器创建纹理对象 / 纹理渲染画幅的大小，最终成像的大小！
    sdlTexture = SDL_CreateTexture(sdlRender,SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,src_w,src_h);
    // 6.初始化图像转化器。前6个参数分别是输入图像源的宽、高、像素格式、输出图像（显示窗口）的宽、高、像素格式！
//    img_convert_ctx = sws_getContext(src_w, src_h, (AVPixelFormat)decode->codec->pix_fmt,
//            dst_w, dst_h,AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    // 7.分配缩放后数据的存储空间
//    av_image_alloc(pYUVFrame_data, pYUVFrame_linesize,dst_w, dst_h, AV_PIX_FMT_YUV420P, 16);

    qDebug()<<"video thread open succeed!";
    return ret;
}

void XVideoThread::close()
{
    XDecodeThread::close();
    // 退出sdl
    SDL_Quit();
}

bool XVideoThread::repaintPts(AVPacket *pkt, long long seekpts)
{
    QMutexLocker locker(&m_vMutex);
    bool ret = decode->send(pkt);
    if(!ret) return true;
    AVFrame* frame = decode->receive();
    if(!frame) return false;
    // 到达位置
    if(decode->pts >= seekpts){
        //if(call) call->Repaint(frame);
        return true;
    }
    XFreeFrame(&frame);
    return false;
}


void XVideoThread::run()
{
    while(!threadExit){
        QMutexLocker locker(&m_vMutex);
        // 传进来的音频pts小于视频pts，视频sleep等待音频跟上！
        if(this->isPause){
            msleep(5);
            continue;
        }
        //qDebug() << "synpts = audio thread pts = " << synpts << " video thread pts =" << decode->pts;
        if(decode->pts < 0){
            continue;
        }
        if( /*synpts > 0 &&*/ synpts < decode->pts){
            // 必须进行同步sleep，否则视频帧会以极快的速度解码完毕，还剩音频在后面慢慢播放！
            // 这里注释掉synpts > 0 是因为可以在视频帧一开始解码时就sleep等待音频帧，而不是视频帧先解码，等到音频帧开始解码再启动等待同步！
            // 去除了视频一开始1s内无声音且产生的同步卡顿问题！

            // 视频启动播放时，synpts会出现负值？
            // 答：decode.pts - QAudioOutput.getByteFree()得到的！被减数为0，减数不为0，就会产生负值！
            msleep(1);
            continue;
        }
        AVPacket* pkt = pop();
        if(!decode->send(pkt)){
            msleep(1);
            continue;
        }
        //一次send 多次recv
        while(!threadExit){
            AVFrame* frame = decode->receive();
            if(!frame) break;
            SDL_UpdateYUVTexture(sdlTexture, nullptr/*&sdlRect*/,
                                frame->data[0], frame->linesize[0],
                                frame->data[1], frame->linesize[1],
                                frame->data[2], frame->linesize[2]);
            SDL_RenderClear(sdlRender);
            SDL_RenderCopy(sdlRender, sdlTexture, nullptr, &sdlRect);
            SDL_RenderPresent(sdlRender);
            SDL_UpdateWindowSurface(sdlWind);
            av_frame_free(&frame);
            //SDL_Delay(40);
            // 原生OpenGL渲染方式，可以取代SDL
            //if(call) call->Repaint(frame);
        }
    }
    qDebug()<<"video thread terminated!";
}


void XVideoThread::setPause(bool isPause)
{
    QMutexLocker locker(&m_vMutex);
    this->isPause = isPause;
}

void XVideoThread::resize()
{
    QMutexLocker locker(&m_vMutex);
    if(!sdlTexture) {
        return;
    }else{
        dst_w = show->geometry().width();
        dst_h = show->geometry().height();
        sdlRect.w = dst_w;
        sdlRect.h = dst_h;
        qDebug()<<"new window size = "<<dst_w<<"*"<<dst_h;
        // 重建SDL_Window
        if(sdlWind) {
           SDL_DestroyWindow(sdlWind);
           sdlWind = SDL_CreateWindowFrom(reinterpret_cast<void*>(showID));
        }
        // 重建SDL_Renderer 必要项，不然显示不完全
        if(sdlRender){
            SDL_DestroyRenderer(sdlRender);
            sdlRender = SDL_CreateRenderer(sdlWind,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        }

        // 重建SDL_Texture  必要项，不然黑屏
        if(sdlTexture){
            SDL_DestroyTexture(sdlTexture);
            sdlTexture = SDL_CreateTexture(sdlRender,SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,src_w,src_h);
        }
    }
}
