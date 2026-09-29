#pragma once
#include "../../../Interfaces/IImageFilter.h"

/**
 * @brief Adjusts image saturation.
 * value > 0 saturates colours, value < 0 desaturates (towards grey).
 * Range: -100 to +100.
 */
class SaturationFilter : public IImageFilter
{
private:
    int m_value; // -100 .. +100

public:
    explicit SaturationFilter(int value);
    void apply(Image& img) override;
};
