#pragma once
#include "../../../Interfaces/IImageFilter.h"

/**
 * @brief Adjusts image contrast.
 * value > 0  increases contrast, value < 0 decreases contrast.
 * Range: -100 to +100 (mapped to a factor internally).
 */
class ContrastFilter : public IImageFilter
{
private:
    int m_value; // -100 .. +100

public:
    explicit ContrastFilter(int value);
    void apply(Image& img) override;
};
