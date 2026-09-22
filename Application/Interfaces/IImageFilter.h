#pragma once
#include "Models/Image_Class.h"
class IImageFilter {
public:
    virtual void apply(Image& img) = 0;
    virtual ~IImageFilter() = default;
};