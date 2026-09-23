#pragma once
#include "../../../Interfaces/IImageFilter.h" 

class BrightnessFilter : public IImageFilter
{
private:
    int adjustmentValue;

public:

    BrightnessFilter(int value);

    void apply(Image& image) override;
};

class LightenFilter : public BrightnessFilter {
public:
    LightenFilter();
};


class DarkenFilter : public BrightnessFilter {
public:
    DarkenFilter();
};