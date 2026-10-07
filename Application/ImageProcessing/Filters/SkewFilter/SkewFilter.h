#pragma once
#include "../../../Interfaces/IImageFilter.h"

class SkewFilter : public IImageFilter {
private:
    float skewX; // horizontal skew factor
    float skewY; // vertical skew factor

public:
    SkewFilter(float skewX = 0.3f, float skewY = 0.0f);

    float getSkewX() const { return skewX; }
    float getSkewY() const { return skewY; }
    void setSkew(float sx, float sy) { skewX = sx; skewY = sy; }

    void apply(Image& img) override;
};
