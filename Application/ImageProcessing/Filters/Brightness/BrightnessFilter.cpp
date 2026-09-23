#include "pch.h"
#include "BrightnessFilter.h"

BrightnessFilter::BrightnessFilter(int val) {
    adjustmentValue = val;
}

void BrightnessFilter::apply(Image& image)
{

    for (int w = 0; w < image.width; ++w) {
        for (int h = 0; h < image.height; ++h) {
            for (int c = 0; c < image.channels; ++c) {
                int pix = image.getPixel(w, h, c);
                pix += adjustmentValue;

                if (pix > 255) pix = 255;
                else if (pix < 0) pix = 0;
                image.setPixel(w, h, c, static_cast<unsigned char>(pix));
            }
        }
    }
}

LightenFilter::LightenFilter() : BrightnessFilter(50) {}


DarkenFilter::DarkenFilter() : BrightnessFilter(-50) {}