#include "OilPaintingFilter.h"
#include <algorithm>
#include <cstring>

OilPaintingFilter::OilPaintingFilter(int radius, int levels)
    : radius(radius > 0 ? radius : 4), intensityLevels(levels > 0 ? levels : 20)
{
    if (intensityLevels > 256) intensityLevels = 256;
}

void OilPaintingFilter::apply(Image& img)
{
    if (!img.imageData || img.width <= 0 || img.height <= 0) return;

    int width = img.width;
    int height = img.height;
    int channels = img.channels;

    Image result(width, height);

    int count[256];
    int sumR[256];
    int sumG[256];
    int sumB[256];

    int rowStride = width * channels;

    for (int y = 0; y < height; ++y) {
        int yMin = std::max(0, y - radius);
        int yMax = std::min(height - 1, y + radius);

        for (int x = 0; x < width; ++x) {
            int xMin = std::max(0, x - radius);
            int xMax = std::min(width - 1, x + radius);

            std::memset(count, 0, intensityLevels * sizeof(int));
            std::memset(sumR, 0, intensityLevels * sizeof(int));
            std::memset(sumG, 0, intensityLevels * sizeof(int));
            std::memset(sumB, 0, intensityLevels * sizeof(int));

            for (int ny = yMin; ny <= yMax; ++ny) {
                const unsigned char* rowPtr = img.imageData + ny * rowStride;
                for (int nx = xMin; nx <= xMax; ++nx) {
                    const unsigned char* p = rowPtr + nx * channels;
                    unsigned char r = p[0];
                    unsigned char g = p[1];
                    unsigned char b = p[2];

                    int curIntensity = ((r + g + b) * intensityLevels) / (3 * 256);
                    if (curIntensity >= intensityLevels) curIntensity = intensityLevels - 1;

                    count[curIntensity]++;
                    sumR[curIntensity] += r;
                    sumG[curIntensity] += g;
                    sumB[curIntensity] += b;
                }
            }

            int maxCount = 0;
            int maxIndex = 0;
            for (int i = 0; i < intensityLevels; ++i) {
                if (count[i] > maxCount) {
                    maxCount = count[i];
                    maxIndex = i;
                }
            }

            if (maxCount > 0) {
                result.setPixel(x, y, 0, static_cast<unsigned char>(sumR[maxIndex] / maxCount));
                result.setPixel(x, y, 1, static_cast<unsigned char>(sumG[maxIndex] / maxCount));
                result.setPixel(x, y, 2, static_cast<unsigned char>(sumB[maxIndex] / maxCount));
            } else {
                result.setPixel(x, y, 0, img.getPixel(x, y, 0));
                result.setPixel(x, y, 1, img.getPixel(x, y, 1));
                result.setPixel(x, y, 2, img.getPixel(x, y, 2));
            }
        }
    }

    img = result;
}
