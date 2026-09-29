#include "Grayfilter.h"

void Grayfilter::apply(Image& image) {
    if (!image.imageData) return;

    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            unsigned char red   = image.getPixel(x, y, 0);
            unsigned char green = image.getPixel(x, y, 1);
            unsigned char blue  = image.getPixel(x, y, 2);

            unsigned char gray  = static_cast<unsigned char>((red * 299 + green * 587 + blue * 114) / 1000);

            image.setPixel(x, y, 0, gray);
            image.setPixel(x, y, 1, gray);
            image.setPixel(x, y, 2, gray);
        }
    }
}