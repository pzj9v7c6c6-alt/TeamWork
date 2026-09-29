#pragma once
#include "../../../Interfaces/IImageFilter.h"

class FlipHorizontalFilter : public IImageFilter
{
public:
    FlipHorizontalFilter();
    void apply(Image& image) override;
};

class FlipVerticalFilter : public IImageFilter
{
public:
    FlipVerticalFilter();
    void apply(Image& image) override;
};
