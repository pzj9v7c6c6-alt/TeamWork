#include "pch.h"
#include "WanoPurpleFilter.h"

WanoPurpleFilter::WanoPurpleFilter()
{
}

void WanoPurpleFilter::apply(Image& image)
{

    for (int _x = 0; _x < image.width; ++_x) {
        for (int _y = 0; _y < image.height; ++_y) {


            r = image.getPixel(_x, _y, 0);
            g = image.getPixel(_x, _y, 1);
            b = image.getPixel(_x, _y, 2);

            r = r * 1.2 + 30;
            g = g * 0.7;
            b = b * 1.3 + 40;

            if (g < 0)g = 0;
            if (b > 255)b = 255;
            if (r > 255)r = 255;

            image.setPixel(_x, _y, 0, r);
            image.setPixel(_x, _y, 1, g);
            image.setPixel(_x, _y, 2, b);



        }
    }

}

