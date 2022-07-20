#include "xdemux.h"
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/codec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}

static double r2d(AVRational r){
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
    static bool isFirst = true;
    QMutexLocker locker(&m_mutex);
    if(isFirst){
        // 初始化封装库
        av_register_all();
        // 初始化网络库
        avformat_network_init();
        isFirst  = false;
    }
}

XDemux::~XDemux()
{
    avformat_close_input(&ic);
}

bool XDemux::open(const char *url)
{
    close();
    AVDictionary* opts = nullptr;
//    av_dict_set(&opts,"rtsp_transport","tcp",0);
//    av_dict_set(&opts,"max_delay","500",0);
    QMutexLocker locker(&m_mutex);
    int ret = avformat_open_input(&ic,url,nullptr,&opts); // 0表示自动选择
    if(ret!=0){
        char buf[1024] = {0};
        av_strerror(ret,buf,sizeof (buf) - 1);
        qDebug()<<"open "<<url<<" failed!" << buf;
        return false;
    }
    qDebug()<<"open "<<url<<" succeed!";
    // 获取流信息
    ret = avformat_find_stream_info(ic,nullptr);
    // 总时长-秒
    totalS = ic->duration / (AV_TIME_BASE);
    qDebug()<<"total second"<<totalS;
    //打印视频流详细信息
    av_dump_format(ic,0,url,0);

    // 获取视频流
    videoStream = av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    AVStream* as = ic->streams[videoStream];
    qDebug() << "=======================================================";
    qDebug() << videoStream << "视频信息";
    qDebug() << "codec_id = " << as->codecpar->codec_id;
    qDebug() << "format = " << as->codecpar->format;
    qDebug() << "width=" << as->codecpar->width;
    width = as->codecpar->width;
    qDebug() << "height=" << as->codecpar->height;
    height = as->codecpar->height;
    //帧率 fps 分数转换
    qDebug() << "video fps = " << r2d(as->avg_frame_rate);
    qDebug() << "=======================================================";
    qDebug() << audioStream << "音频信息";
    audioStream = av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
    as = ic->streams[audioStream];
    qDebug() << "codec_id = " << as->codecpar->codec_id;
    qDebug() << "format = " << as->codecpar->format;
    qDebug() << "sample_rate = " << as->codecpar->sample_rate;
    sampleRate = as->codecpar->sample_rate;
    //AVSampleFormat
    qDebug() << "channels = " << as->codecpar->channels;
    channels = as->codecpar->channels;
    //一帧数据？？ 单通道样本数
    qDebug() << "frame_size = " << as->codecpar->frame_size;
    return true;

}

AVPacket *XDemux::read()
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return nullptr;
    AVPacket* pkt = av_packet_alloc(); // 分配对象内存空间
    int ret = av_read_frame(ic,pkt);  // 读取一帧，向pkt中写入数据！
    if(ret != 0){
        av_packet_free(&pkt);
        return nullptr;
    }
    // pts转换为ms
    pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    //qDebug() << pkt->pts << " "<<flush;
    return pkt;
}

AVCodecParameters *XDemux::copyVParam()
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return nullptr;
    // 在DLL中申请pa空间（无数据）
    AVCodecParameters* pa = avcodec_parameters_alloc();
    // 将ic中的数据拷贝至pa中！
    avcodec_parameters_copy(pa,ic->streams[videoStream]->codecpar);
    return pa;
}

AVCodecParameters *XDemux::copyAParam()
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return nullptr;
    // 在DLL中申请pa空间（无数据）
    AVCodecParameters* pa = avcodec_parameters_alloc();
    // 将ic中的数据拷贝至pa中！
    avcodec_parameters_copy(pa,ic->streams[audioStream]->codecpar);
    return pa;
}

AVPacket* XDemux::readVideo()
{
    if(!ic) return nullptr;
    AVPacket* pkt = nullptr;
    for (int i = 0;i<20;i++) {
        pkt = read();
        if(!pkt) break;
        if(pkt->stream_index ==videoStream) break;
        av_packet_free(&pkt);
    }
    return pkt;
}

bool XDemux::seek(double pos)
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return false;
    // 清理读取缓冲
    avformat_flush(ic);
    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int ret = av_seek_frame(ic,videoStream,seekPos,AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    if(ret < 0) return false;
    return true;
}

bool XDemux::isAudio(AVPacket *pkt)
{
    if(!pkt) return false;
    if(pkt->stream_index == videoStream){
        return false;
    }
    return true;
}

void XDemux::clear()
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return;
    // 清理读取缓冲
    avformat_flush(ic);
}

void XDemux::close()
{
    QMutexLocker locker(&m_mutex);
    if(!ic) return;
    avformat_close_input(&ic);
    totalS = 0;
}
