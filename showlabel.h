#ifndef SHOWLABEL_H
#define SHOWLABEL_H

#include <QLabel>
#include <QKeyEvent>
#include "xdemuxthread.h"

class showLabel : public QLabel
{
    Q_OBJECT
public:
    showLabel(XDemuxThread* dt,QWidget* parent);
signals:
    void refreshShowLabel();
protected:
    void resizeEvent(QResizeEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;

private:
    XDemuxThread* dt = nullptr;
    QWidget* parent = nullptr;
    bool isFullScreen = false;
};

#endif // SHOWLABEL_H
