#include "FrameFilter.h"

FrameFilter::FrameFilter(int t, int r, int g, int b, bool d)
    : thickness(t), red(r), green(g), blue(b), decorated(d)
{
}

void FrameFilter::apply(Image& img)
{
    if (!img.imageData || img.width <= 2 * thickness || img.height <= 2 * thickness)
        return;

    for (int x = 0; x < img.width; ++x) {
        for (int y = 0; y < img.height; ++y) {
            bool frame = (x < thickness || x >= img.width - thickness ||
                          y < thickness || y >= img.height - thickness);
            if (frame) {
                img.setPixel(x, y, 0, static_cast<unsigned char>(red));
                img.setPixel(x, y, 1, static_cast<unsigned char>(green));
                img.setPixel(x, y, 2, static_cast<unsigned char>(blue));
            }
            if (decorated) {
                int inner = thickness + 8;
                if ((x == inner || x == img.width - inner - 1) && (y >= inner && y <= img.height - inner - 1)) {
                    img.setPixel(x, y, 0, 255);
                    img.setPixel(x, y, 1, 255);
                    img.setPixel(x, y, 2, 255);
                } else if ((y == inner || y == img.height - inner - 1) && (x >= inner && x <= img.width - inner - 1)) {
                    img.setPixel(x, y, 0, 255);
                    img.setPixel(x, y, 1, 255);
                    img.setPixel(x, y, 2, 255);
                }
            }
        }
    }
}
