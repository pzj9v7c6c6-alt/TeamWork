#include "pch.h"
#include "ContrastFilter.h"
#include <cmath>
#include <algorithm>

ContrastFilter::ContrastFilter(int value)
    : m_value(value)
{
}

void ContrastFilter::apply(Image& img)
{
    // Map -100..+100 to a contrast factor.
    // At 0  -> factor = 1.0  (no change)
    // At +100 -> factor ~= 3.0 (high contrast)
    // At -100 -> factor ~= 0.33 (low contrast / grey)
    double factor;
    if (m_value >= 0) {
        factor = 1.0 + (m_value / 100.0) * 2.0; // 1.0 .. 3.0
    } else {
        factor = 1.0 + (m_value / 100.0) * 0.66667; // 1.0 .. 0.333
    }

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            for (int c = 0; c < img.channels; ++c) {
                double pix = static_cast<double>(img.getPixel(x, y, c));
                // Shift to centre, scale, shift back
                pix = (pix - 128.0) * factor + 128.0;
                if (pix > 255.0) pix = 255.0;
                if (pix <   0.0) pix =   0.0;
                img.setPixel(x, y, c, static_cast<unsigned char>(pix));
            }
        }
    }
}
