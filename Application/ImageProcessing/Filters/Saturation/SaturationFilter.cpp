#include "pch.h"
#include "SaturationFilter.h"

SaturationFilter::SaturationFilter(int value)
    : m_value(value)
{
}

void SaturationFilter::apply(Image& img)
{
    // factor: 0 at -100 (greyscale), 1 at 0 (unchanged), 2 at +100 (vivid)
    double factor = 1.0 + (m_value / 100.0);
    if (factor < 0.0) factor = 0.0;

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            double r = img.getPixel(x, y, 0);
            double g = img.getPixel(x, y, 1);
            double b = img.getPixel(x, y, 2);

            // Luminance (greyscale value)
            double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;

            // Interpolate between grey and original colour
            double nr = lum + factor * (r - lum);
            double ng = lum + factor * (g - lum);
            double nb = lum + factor * (b - lum);

            auto clamp = [](double v) -> unsigned char {
                if (v > 255.0) return 255;
                if (v <   0.0) return 0;
                return static_cast<unsigned char>(v);
            };

            img.setPixel(x, y, 0, clamp(nr));
            img.setPixel(x, y, 1, clamp(ng));
            img.setPixel(x, y, 2, clamp(nb));
        }
    }
}
