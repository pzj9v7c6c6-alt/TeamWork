#pragma once
#include "../../../Interfaces/IImageFilter.h"

class ResizeFilter : public IImageFilter {
private:
    int newWidth;
    int newHeight;
public:
    ResizeFilter(int w, int h);
    void apply(Image& img) override;
};