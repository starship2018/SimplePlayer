#-------------------------------------------------
#
# Project created by QtCreator 2022-06-20T17:10:52
#
#-------------------------------------------------

QT       += core gui opengl openglextensions multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD/ffmpeg/include \
               $$PWD/SDL/include

LIBS += -L$$PWD/ffmpeg/lib/ -lavutil -lavformat -lavcodec -lavdevice -lavfilter -lpostproc -lswresample -lswscale \
        -L$$PWD/VLD/lib/Win64/ -lvld \
        -L$$PWD/SDL/lib/x64/ -lSDL2 \


TARGET = XPlay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        xplay.cpp \
        xdemux.cpp \
        xdecode.cpp \
        xvideowidget.cpp \
        xresample.cpp \
        xaudioplay.cpp \
        xaudiothread.cpp \
        xvideothread.cpp \
        xdemuxthread.cpp \
        xdecodethread.cpp \
    showlabel.cpp

HEADERS += \
        xplay.h \
    xdemux.h \
    xdecode.h \
    xvideowidget.h \
    xresample.h \
    xaudioplay.h \
    xaudiothread.h \
    xvideothread.h \
    ivideocall.h \
    xdemuxthread.h \
    xdecodethread.h \
    showlabel.h

FORMS += \
        xplay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
