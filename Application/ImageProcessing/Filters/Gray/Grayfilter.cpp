#include "Grayfilter.h"

Grayfilter::Grayfilter() {}

void Grayfilter::apply(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            unsigned char red   = image(i, j, 0);
            unsigned char green = image(i, j, 1);
            unsigned char blue  = image(i, j, 2);

            unsigned char gray  = (red + green + blue) / 3;

            image(i, j, 0) = gray;
            image(i, j, 1) = gray;
            image(i, j, 2) = gray;
        }
    }
}