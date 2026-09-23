#pragma once
#include "../../../Interfaces/IImageFilter.h"

class InvertFilter : public IImageFilter
{
public:
    InvertFilter();
    void apply(Image& image) override;
};