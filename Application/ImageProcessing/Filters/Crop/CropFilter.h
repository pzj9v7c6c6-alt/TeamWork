#pragma once
#include "../../../Interfaces/IImageFilter.h"

class CropFilter : public IImageFilter
{
private:
    int _x, _y, NewWidth, NewHeight;

public:
    CropFilter(int x, int y, int width, int height);
    void apply(Image& image) override;
};