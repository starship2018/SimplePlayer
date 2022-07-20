#ifndef IVIDEOCALL_H
#define IVIDEOCALL_H
#include "qwindowdefs.h"
struct AVFrame;
class IVideoCall
{
public:
    virtual void Init(int width,int height) = 0;
    virtual void Repaint(AVFrame* frame) = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual WId getWindID() = 0;
};

#endif // IVIDEOCALL_H
