#include "showlabel.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QMutexLocker>
#include <QMutex>

showLabel::showLabel(XDemuxThread* t,QWidget* parent):QLabel(parent),dt(t)
{
    this->parent = parent;
    // 禁用QLabel自身的刷新，因为Qt自身的渲染引擎和SDL会产生冲突，窗口在缩放时会导致闪烁卡顿！
    setUpdatesEnabled(false);
}

void showLabel::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    dt->resizeShowScreen();
}

// 双击进入/退出全屏
void showLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if(!isFullScreen){
        // 进入全屏
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        showFullScreen();  //
        isFullScreen = true;
        dt->resizeShowScreen();
    }else{
        // 退出全屏
        setWindowFlags(Qt::Dialog);
        setParent(parent);
        showNormal();
        isFullScreen = false;
        dt->resizeShowScreen();
        show();
        emit refreshShowLabel();
    }
}

