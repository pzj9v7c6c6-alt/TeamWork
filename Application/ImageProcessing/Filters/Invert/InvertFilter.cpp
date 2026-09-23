#include "pch.h"
#include "InvertFilter.h"

InvertFilter::InvertFilter()
{
}

void InvertFilter::apply(Image& image)
{

    for (int _x = 0; _x < image.width; ++_x) {
        for (int _y = 0; _y < image.height; ++_y) {
            for (int _c = 0; _c < image.channels;++_c) {
                int pix = image.getPixel(_x, _y, _c);
                pix = 255 - pix;
                image.setPixel(_x, _y, _c, pix);

            }
        }
    }

}