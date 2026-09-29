#include "RotateFilter.h"
#include <cmath>
#include <algorithm>

RotateFilter::RotateFilter(int angle) : angle(angle) 
{
}

void RotateFilter::apply(Image& img)
{
    if (!img.imageData || img.width <= 0 || img.height <= 0) return;

    int oldWidth = img.width;
    int oldHeight = img.height;

    int normAngle = ((angle % 360) + 360) % 360;
    if (normAngle == 0) return;

    if (normAngle == 90) {
        Image tempImg(oldHeight, oldWidth);
        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
                int newX = oldHeight - 1 - y;
                int newY = x;
                for (int c = 0; c < 3; ++c) {
                    tempImg.setPixel(newX, newY, c, img.getPixel(x, y, c));
                }
            }
        }
        img = tempImg;
    }
    else if (normAngle == 180) {
        Image tempImg(oldWidth, oldHeight);
        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
                int newX = oldWidth  - 1 - x;
                int newY = oldHeight - 1 - y;
                for (int c = 0; c < 3; ++c) {
                    tempImg.setPixel(newX, newY, c, img.getPixel(x, y, c));
                }
            }
        }
        img = tempImg;
    }
    else if (normAngle == 270) {
        Image tempImg(oldHeight, oldWidth);
        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
                int newX = y;
                int newY = oldWidth - 1 - x;
                for (int c = 0; c < 3; ++c) {
                    tempImg.setPixel(newX, newY, c, img.getPixel(x, y, c));
                }
            }
        }
        img = tempImg;
    }
    else {
        // Arbitrary angle rotation (0 - 360)
        double rad = normAngle * 3.14159265358979323846 / 180.0;
        double cosA = std::cos(rad);
        double sinA = std::sin(rad);

        int newWidth  = static_cast<int>(std::round(std::abs(oldWidth * cosA) + std::abs(oldHeight * sinA)));
        int newHeight = static_cast<int>(std::round(std::abs(oldWidth * sinA) + std::abs(oldHeight * cosA)));
        if (newWidth <= 0 || newHeight <= 0) return;

        Image tempImg(newWidth, newHeight);
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                tempImg.setPixel(x, y, 0, 255);
                tempImg.setPixel(x, y, 1, 255);
                tempImg.setPixel(x, y, 2, 255);
            }
        }

        double cxNew = (newWidth - 1) / 2.0;
        double cyNew = (newHeight - 1) / 2.0;
        double cxOld = (oldWidth - 1) / 2.0;
        double cyOld = (oldHeight - 1) / 2.0;

        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                double dx = x - cxNew;
                double dy = y - cyNew;
                double srcX =  dx * cosA + dy * sinA + cxOld;
                double srcY = -dx * sinA + dy * cosA + cyOld;

                int sx = static_cast<int>(std::round(srcX));
                int sy = static_cast<int>(std::round(srcY));

                if (sx >= 0 && sx < oldWidth && sy >= 0 && sy < oldHeight) {
                    for (int c = 0; c < 3; ++c) {
                        tempImg.setPixel(x, y, c, img.getPixel(sx, sy, c));
                    }
                }
            }
        }
        img = tempImg;
    }
}