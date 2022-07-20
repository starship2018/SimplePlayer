#include "xresample.h"
#include <QDebug>
extern "C"{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
XResample::XResample()
{

}

bool XResample::open(AVCodecParameters *para,bool isClear)
{
    QMutexLocker locker(&m_mutex);
    if(!para) return false;
    //如果actx为NULL会分配空间
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(2),	//输出格式
                              (AVSampleFormat)outFormat,			//输出样本格式 1 AV_SAMPLE_FMT_S16
                              para->sample_rate,					//输出采样率
                              av_get_default_channel_layout(para->channels),//输入格式
                              (AVSampleFormat)para->format,
                              para->sample_rate,
                              0, 0);
    if(isClear) avcodec_parameters_free(&para);
    int ret = swr_init(actx);
    if (ret != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        qDebug() << "swr_init  failed! :" << buf;
        return false;
    }
    return true;
}

bool XResample::close()
{
    QMutexLocker locker(&m_mutex);
    return false;
}

int XResample::resample(AVFrame *indata, unsigned char *d)
{
    if(!indata) return 0;
    if(!d){
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx,
                         data,
                         indata->nb_samples,		//输出
                         (const uint8_t**)indata->data,
                         indata->nb_samples	//输入
                         );
    if (re <= 0)return re;
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    av_frame_free(&indata);
    return outSize;
}
