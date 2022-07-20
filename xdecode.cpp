#include "xdecode.h"
#include <QDebug>
extern "C"
{
#include<libavcodec/avcodec.h>
#include<libavutil/pixfmt.h>
}

void XFreePacket(AVPacket** pkt){
    if(!pkt || !(*pkt)) return;
    av_packet_free(pkt);
}

void XFreeFrame(AVFrame** frame){
    if(!frame || !(*frame)) return;
    av_frame_free(frame);
}

XDecode::XDecode()
{

}

XDecode::~XDecode()
{

}

bool XDecode::open(AVCodecParameters *para)
{
    if(!para) return false;
    // 根据解码器参数AVCodecParameters 中的解码器ID来获取解码器
    AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
    if(!vcodec){
        // 获取失败
        avcodec_parameters_free(&para);
        qDebug()<<"can't find the codec id " << para->codec_id;
        return false;
    }
    qDebug()<< "find the AVCodec " << para->codec_id;
    // 分配解码器上下文
    codec = avcodec_alloc_context3(vcodec);
    // 给解码器上下文配置参数
    avcodec_parameters_to_context(codec,para);
    avcodec_parameters_free(&para);
    // 设置解码线程数为 8
    codec->thread_count = 8;
    // 根据解码器上下文打开解码器
    int ret = avcodec_open2(codec,nullptr,nullptr);
    if(ret != 0){
        avcodec_free_context(&codec);
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        qDebug()<< "avcodec_open2  failed! :" << buf;
        return false;
    }
    qDebug() << " avcodec_open2 success!";
    return true;

}

void XDecode::close()
{
    QMutexLocker locker(&m_mutex);
    if(codec){
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    pts = 0;
}

void XDecode::clear()
{
    QMutexLocker locker(&m_mutex);
    pts = 0;
    if(codec){
        // 清理解码缓冲！
        avcodec_flush_buffers(codec);
    }
}

bool XDecode::send(AVPacket *pkt)
{
    if(!pkt || pkt->size <=0 || !pkt->data) return false;
    QMutexLocker locker(&m_mutex);
    if(!codec) return false;
    // 发送pkt到解码线程
    int ret = avcodec_send_packet(codec,pkt);
    locker.unlock();
    av_packet_free(&pkt);
    if(ret !=0) return false;
    return true;
}

//获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
//每次复制一份，由调用者释放 av_frame_free
AVFrame *XDecode::receive()
{
    QMutexLocker locker(&m_mutex);
    if(!codec) return nullptr;
    AVFrame* frame = av_frame_alloc();
    int ret = avcodec_receive_frame(codec,frame);
    if(ret !=0){
        av_frame_free(&frame);
        return nullptr;
    }
    pts = frame->pts;
    return frame;
}


