#ifndef XPLAY_H
#define XPLAY_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <xvideowidget.h>
namespace Ui {
class XPlay;
}

class showLabel;
class XPlay : public QWidget
{
    Q_OBJECT

public:
    explicit XPlay(QWidget *parent = nullptr);
    ~XPlay();
    XVideoWidget* videoWgt = nullptr;
    void timerEvent(QTimerEvent* e);

    void resizeEvent(QResizeEvent* e);

    void mouseDoubleClickEvent(QMouseEvent* e);

    void PlayorPause();

    virtual void closeEvent(QCloseEvent* e);

    void changeEvent(QEvent* e);

    //void keyPressEvent(QKeyEvent* e);
public slots:
    void refreshShowLabelLayout();

private:



    QPushButton* openBtn = nullptr;
    QPushButton* pauseBtn = nullptr;
    QSlider* slider = nullptr;
    bool isSliderPress = false;
    void setPause(bool isPause);
    showLabel* showL = nullptr;
    QWidget* screen = nullptr;
    QGridLayout* _l = nullptr;
};

#endif // XPLAY_H
