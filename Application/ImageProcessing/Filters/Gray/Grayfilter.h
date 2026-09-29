#pragma once
#include "../../../../Interfaces/IImageFilter.h"
class Grayfilter : public IImageFilter
{
public:
    Grayfilter();
    void apply(Image& img) override;
};
