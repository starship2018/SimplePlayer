#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QCoreApplication>
#include <QFile>
#include <QGLShaderProgram>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include "ivideocall.h"
struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions,public IVideoCall
{
    Q_OBJECT
public:
    XVideoWidget(QWidget* parent);
    ~XVideoWidget();
    //不管成功与否都释放frame空间
    virtual void Repaint(AVFrame *frame);
    //
    virtual void Init(int width, int height);

    virtual int getWidth();
    virtual int getHeight();
    virtual WId getWindID();
protected:
    // 刷新显示
    void paintGL();
    // 初始化GL
    void initializeGL();
    // 窗口尺寸变化
    void resizeGL(int wid,int hgt);

private:
    QGLShaderProgram m_program;
    // shader中yuv变量地址
    GLuint unis[3] = {0};
    // opengl texture地址
    GLuint texs[3] = {0};

    //材质内存空间
    unsigned char *datas[3] = { 0 };

    int width = 240;
    int height = 128;

    FILE* fp = nullptr;
    QMutex m_mutex;
};

#endif // XVIDEOWIDGET_H

