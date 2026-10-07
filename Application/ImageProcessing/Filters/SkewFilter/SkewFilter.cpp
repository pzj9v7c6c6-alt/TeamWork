#include "SkewFilter.h"
#include <cmath>
#include <algorithm>

SkewFilter::SkewFilter(float skewX, float skewY)
    : skewX(skewX), skewY(skewY)
{
}

void SkewFilter::apply(Image& img)
{
    if (!img.imageData || img.width <= 0 || img.height <= 0) return;

    if (std::abs(skewX) < 1e-6f && std::abs(skewY) < 1e-6f) return;

    int oldWidth = img.width;
    int oldHeight = img.height;

    // Calculate coordinates of the 4 transformed corners
    float cornersX[4] = { 0.0f, static_cast<float>(oldWidth - 1), 0.0f, static_cast<float>(oldWidth - 1) };
    float cornersY[4] = { 0.0f, 0.0f, static_cast<float>(oldHeight - 1), static_cast<float>(oldHeight - 1) };

    float minX = 1e9f, maxX = -1e9f;
    float minY = 1e9f, maxY = -1e9f;

    for (int i = 0; i < 4; ++i) {
        float tx = cornersX[i] + skewX * cornersY[i];
        float ty = cornersY[i] + skewY * cornersX[i];
        if (tx < minX) minX = tx;
        if (tx > maxX) maxX = tx;
        if (ty < minY) minY = ty;
        if (ty > maxY) maxY = ty;
    }

    int newWidth = std::max(1, static_cast<int>(std::round(maxX - minX)) + 1);
    int newHeight = std::max(1, static_cast<int>(std::round(maxY - minY)) + 1);

    Image tempImg(newWidth, newHeight);

    // Initialize canvas with white background (consistent with RotateFilter)
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            tempImg.setPixel(x, y, 0, 255);
            tempImg.setPixel(x, y, 1, 255);
            tempImg.setPixel(x, y, 2, 255);
        }
    }

    // Determinant for 2D shear inverse mapping:
    // [ x' ] = [ 1      skewX ] [ x ]
    // [ y' ]   [ skewY  1     ] [ y ]
    // D = 1 - skewX * skewY
    float det = 1.0f - skewX * skewY;
    if (std::abs(det) < 1e-5f) {
        det = (det >= 0.0f ? 1e-5f : -1e-5f);
    }

    for (int dy = 0; dy < newHeight; ++dy) {
        for (int dx = 0; dx < newWidth; ++dx) {
            float xp = dx + minX;
            float yp = dy + minY;

            float srcX = (xp - skewX * yp) / det;
            float srcY = (yp - skewY * xp) / det;

            if (srcX >= 0.0f && srcX <= oldWidth - 1 && srcY >= 0.0f && srcY <= oldHeight - 1) {
                int x0 = static_cast<int>(std::floor(srcX));
                int y0 = static_cast<int>(std::floor(srcY));
                int x1 = std::min(x0 + 1, oldWidth - 1);
                int y1 = std::min(y0 + 1, oldHeight - 1);

                float fx = srcX - x0;
                float fy = srcY - y0;

                for (int c = 0; c < 3; ++c) {
                    float p00 = img.getPixel(x0, y0, c);
                    float p10 = img.getPixel(x1, y0, c);
                    float p01 = img.getPixel(x0, y1, c);
                    float p11 = img.getPixel(x1, y1, c);

                    float val = (1.0f - fx) * (1.0f - fy) * p00 +
                                fx * (1.0f - fy) * p10 +
                                (1.0f - fx) * fy * p01 +
                                fx * fy * p11;

                    int clamped = std::clamp(static_cast<int>(std::round(val)), 0, 255);
                    tempImg.setPixel(dx, dy, c, static_cast<unsigned char>(clamped));
                }
            }
        }
    }

    img = tempImg;
}
