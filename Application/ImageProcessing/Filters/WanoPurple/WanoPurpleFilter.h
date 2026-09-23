#pragma once
#include "../../../Interfaces/IImageFilter.h"

class WanoPurpleFilter : public IImageFilter
{
private:
    int r, g, b;
public:
    WanoPurpleFilter();

    void apply(Image& image) override;
};