#pragma once
#include "../../../Interfaces/IImageFilter.h"

class OilPaintingFilter : public IImageFilter {
private:
    int radius;
    int intensityLevels;

public:
    OilPaintingFilter(int radius = 4, int levels = 20);

    int getRadius() const { return radius; }
    int getIntensityLevels() const { return intensityLevels; }
    void setParameters(int r, int l) { radius = r; intensityLevels = l; }

    void apply(Image& img) override;
};
