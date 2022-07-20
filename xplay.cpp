#include <QFileDialog>
#include <QSizePolicy>
#include <QMessageBox>
#include <QWindowStateChangeEvent>
#include "xplay.h"
#include "ui_xplay.h"
#include "xdemuxthread.h"
#include "showlabel.h"


static XDemuxThread dt;

XPlay::XPlay(QWidget *parent) :
    QWidget(parent)
{
    dt.Start();
    this->setWindowTitle("FFmpeg SDL Player");
    setWindowFlag(Qt::Window);
    QGridLayout* l = new QGridLayout(this);
    setLayout(l);
    screen = new QWidget(this);
    screen->setStyleSheet("QWidget{background-color:rgb(0,0,0);}");

    QSizePolicy sp;
    sp.setHorizontalPolicy(QSizePolicy::Preferred);
    sp.setVerticalPolicy(QSizePolicy::Preferred);
    screen->setSizePolicy(sp);
    l->addWidget(screen,0,0,8,8);
    this->setGeometry(0,0,500,300);

    showL = new showLabel(&dt,this);
    showL->setText("");
    showL->setStyleSheet("QLabel{background-color:rgb(0,0,200);}");
    showL->setSizePolicy(sp);

    connect(showL,&showLabel::refreshShowLabel,this,&XPlay::refreshShowLabelLayout);
    _l = new QGridLayout(this);
    screen->setLayout(_l);
    _l->addWidget(showL);
    _l->setMargin(0);
    qDebug()<<"screen->width()"<<screen->width();

    slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    connect(slider,&QSlider::sliderPressed,this,[&](){
        isSliderPress = true;
    });
    connect(slider,&QSlider::sliderReleased,this,[&](){
        isSliderPress = false;
        double pos = 0.00;
        pos = (double)slider->value() / slider->maximum();
        dt.seek(pos);
    });
    openBtn = new QPushButton("open",this);
    pauseBtn = new QPushButton("pause",this);
    connect(pauseBtn,&QPushButton::clicked,this,&XPlay::PlayorPause);
    l->addWidget(slider,8,2,1,1);
    l->addWidget(openBtn,8,0,1,1);
    l->addWidget(pauseBtn,8,1,1,1);
    connect(openBtn,&QPushButton::clicked,this,[&](){
        QString name = QFileDialog::getOpenFileName(this);
        //name = "http://vodkgeyttp8.vod.126.net/cloudmusic/IWAxMSRkYSQgIDkhICAhMA==/mv/5965351/1636f60704d57e8ce3a188b243d83fb8.mp4?wsSecret=18754baaa13e2e89088a23b5d6587d2b&wsTime=1658303305";
        if(name.isNull()) return;
        this->setWindowTitle(name.split("/").last());
        if(!dt.open(name.toLocal8Bit(),showL->winId())){
            QMessageBox::warning(this,"error","open failed!");
        }
        setPause(dt.isPause);
    });
    startTimer(40);
}

XPlay::~XPlay()
{
}


// 刷新进度条
void XPlay::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    if(isSliderPress) return;
    long long total = dt.totalMs;
    if(dt.totalMs > 0){
        double pos = (double)dt.pts / total;
        slider->setValue(pos * slider->maximum());
    }
}

// 缩放窗口的同时维持视频Label维持16：9比例，可以根据解码信息调整比例，部分视频的比例可能不是16：9
void XPlay::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    if(screen->width() > lrint(screen->height()/9*16)){
        // 高度不够
        showL->setMaximumHeight(screen->geometry().height());  // 高度顶到最大
        showL->setMaximumWidth(lrint(screen->geometry().height() / 9 * 16));  // 按比例调整宽度
    }else if(screen->width() < lrint(screen->height()/9*16)){
        // 宽度不够
        showL->setMaximumWidth(screen->geometry().width());    // 宽度顶到最大
        showL->setMaximumHeight(lrint(screen->geometry().width() / 16 * 9));   // 按比例调整高度
    }
    dt.resizeShowScreen();
}

void XPlay::mouseDoubleClickEvent(QMouseEvent *e)
{
//    if(isFullScreen()){
//        this->showNormal();
//    }else{
//        this->showFullScreen();
//    }
}

// 暂停
void XPlay::PlayorPause()
{
    bool isPause = !dt.isPause;
    setPause(isPause);
    dt.setPause(isPause);
}

// 关闭事件
void XPlay::closeEvent(QCloseEvent *e)
{
    dt.close();
}

// 刷新QLabel位置
void XPlay::refreshShowLabelLayout()
{
    _l->removeWidget(showL);
    showL->setParent(this);
    _l->addWidget(showL);
    update();
}


void XPlay::setPause(bool isPause)
{
    if(isPause) {
        pauseBtn->setText("play");
    }else{
        pauseBtn->setText("pause");
    }
}
