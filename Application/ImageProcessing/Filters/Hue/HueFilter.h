#pragma once
#include "../../../Interfaces/IImageFilter.h"

/**
 * @brief Rotates the hue of each pixel in the image.
 * degrees: rotation in the range -180 to +180.
 */
class HueFilter : public IImageFilter
{
private:
    int m_degrees; // -180 .. +180

public:
    explicit HueFilter(int degrees);
    void apply(Image& img) override;
};
