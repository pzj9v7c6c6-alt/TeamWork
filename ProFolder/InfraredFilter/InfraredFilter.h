#pragma once
#include "Image_Class.h"

class Filter{
public:
virtual void apply(Image& img)=0;
virtual ~Filter()=default;
};

class InfraredFilter:public Filter{
    public:
void apply(Image&img)override;
};