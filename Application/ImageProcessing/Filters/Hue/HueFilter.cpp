#include "pch.h"
#include "HueFilter.h"
#include <cmath>
#include <algorithm>

HueFilter::HueFilter(int degrees)
    : m_degrees(degrees)
{
}

// Helper: RGB -> HSV (all in [0,1])
static void rgbToHsv(double r, double g, double b,
                     double& h, double& s, double& v)
{
    double cmax = std::max({r, g, b});
    double cmin = std::min({r, g, b});
    double delta = cmax - cmin;

    v = cmax;
    s = (cmax < 1e-9) ? 0.0 : (delta / cmax);

    if (delta < 1e-9) {
        h = 0.0;
    } else if (cmax == r) {
        h = std::fmod((g - b) / delta, 6.0);
    } else if (cmax == g) {
        h = (b - r) / delta + 2.0;
    } else {
        h = (r - g) / delta + 4.0;
    }
    h = h * 60.0; // degrees
    if (h < 0.0) h += 360.0;
}

// Helper: HSV -> RGB (all in [0,1])
static void hsvToRgb(double h, double s, double v,
                     double& r, double& g, double& b)
{
    if (s < 1e-9) {
        r = g = b = v;
        return;
    }
    h = std::fmod(h, 360.0);
    if (h < 0.0) h += 360.0;
    double hh = h / 60.0;
    int    i  = static_cast<int>(hh);
    double ff = hh - i;
    double p  = v * (1.0 - s);
    double q  = v * (1.0 - s * ff);
    double t  = v * (1.0 - s * (1.0 - ff));

    switch (i) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
    }
}

void HueFilter::apply(Image& img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            double r = img.getPixel(x, y, 0) / 255.0;
            double g = img.getPixel(x, y, 1) / 255.0;
            double b = img.getPixel(x, y, 2) / 255.0;

            double h, s, v;
            rgbToHsv(r, g, b, h, s, v);

            h += m_degrees;

            double nr, ng, nb;
            hsvToRgb(h, s, v, nr, ng, nb);

            auto clamp = [](double c) -> unsigned char {
                if (c > 1.0) c = 1.0;
                if (c < 0.0) c = 0.0;
                return static_cast<unsigned char>(c * 255.0 + 0.5);
            };

            img.setPixel(x, y, 0, clamp(nr));
            img.setPixel(x, y, 1, clamp(ng));
            img.setPixel(x, y, 2, clamp(nb));
        }
    }
}
