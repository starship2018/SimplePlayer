#include <QApplication>
#include <QtWidgets/QApplication>
#include "xplay.h"
//#include "vld.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XPlay w;
    w.show();
    return a.exec();
}
