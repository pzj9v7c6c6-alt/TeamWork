#pragma once
#include "Image_Class.h"
class Filter{
public:
virtual void apply(Image& img)=0;
virtual ~Filter()=default;
};
class FrameFilter:public Filter{
    private:
    int thickness;
    int red,green,blue;
    bool decorated;
    public:
    FrameFilter(int t=20,int r=0,int g=100,int b=255,bool d=false);
    void apply(Image &img) override;
};
class ResizeFilter:public Filter{
    private:
    int newWidth;
    int newHight;
    public:
    ResizeFilter(int w,int h);
    void apply(Image &img)override;
};
class InfraredFilter:public Filter{
    public:
void apply(Image&img)override;
};
class BlurFilter:public Filter{
public:
void apply(Image & img)override;
};