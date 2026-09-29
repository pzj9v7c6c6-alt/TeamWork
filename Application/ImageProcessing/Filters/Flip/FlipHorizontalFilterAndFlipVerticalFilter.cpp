#include "FlipHorizontalFilterAndFlipVerticalFilter.h"

void FlipHorizontalFilter::apply(Image& image)
{
    if (!image.imageData) return;

    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width / 2; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                unsigned char left  = image.getPixel(x, y, c);
                unsigned char right = image.getPixel(image.width - 1 - x, y, c);
                image.setPixel(x, y, c, right);
                image.setPixel(image.width - 1 - x, y, c, left);
            }
        }
    }
}

void FlipVerticalFilter::apply(Image& image)
{
    if (!image.imageData) return;

    for (int x = 0; x < image.width; ++x) {
        for (int y = 0; y < image.height / 2; ++y) {
            for (int c = 0; c < image.channels; ++c) {
                unsigned char top    = image.getPixel(x, y, c);
                unsigned char bottom = image.getPixel(x, image.height - 1 - y, c);
                image.setPixel(x, y, c, bottom);
                image.setPixel(x, image.height - 1 - y, c, top);
            }
        }
    }
}