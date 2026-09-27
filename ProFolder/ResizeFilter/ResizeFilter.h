#pragma once
#include "Image_Class.h"
class Filter{
public:
virtual void apply(Image& img)=0;
virtual ~Filter()=default;
};

class ResizeFilter:public Filter{
    private:
    int newWidth;
    int newHight;
    public:
    ResizeFilter(int w,int h);
    void apply(Image &img)override;
};