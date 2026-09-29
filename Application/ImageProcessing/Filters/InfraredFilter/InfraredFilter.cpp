#include "InfraredFilter.h"

void InfraredFilter::apply(Image& img)
{
    if (!img.imageData) return;

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char g = img.getPixel(x, y, 1);
            unsigned char b = img.getPixel(x, y, 2);
            img.setPixel(x, y, 0, 255);
            img.setPixel(x, y, 1, 255 - g);
            img.setPixel(x, y, 2, 255 - b);
        }
    }
}
