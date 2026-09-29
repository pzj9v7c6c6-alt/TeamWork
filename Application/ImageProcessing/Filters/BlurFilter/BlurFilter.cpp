#include "BlurFilter.h"
#include <algorithm>

void BlurFilter::apply(Image& img)
{
    if (img.width <= 4 || img.height <= 4 || !img.imageData) return;

    Image result = img;
    const int radius = 3;
    const int area = (2 * radius + 1) * (2 * radius + 1);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            int sumR = 0, sumG = 0, sumB = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                int py = std::clamp(y + dy, 0, img.height - 1);
                for (int dx = -radius; dx <= radius; ++dx) {
                    int px = std::clamp(x + dx, 0, img.width - 1);
                    sumR += img.getPixel(px, py, 0);
                    sumG += img.getPixel(px, py, 1);
                    sumB += img.getPixel(px, py, 2);
                }
            }
            result.setPixel(x, y, 0, static_cast<unsigned char>(sumR / area));
            result.setPixel(x, y, 1, static_cast<unsigned char>(sumG / area));
            result.setPixel(x, y, 2, static_cast<unsigned char>(sumB / area));
        }
    }

    img = result;
}
