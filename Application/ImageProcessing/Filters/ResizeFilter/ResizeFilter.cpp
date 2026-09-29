#include "ResizeFilter.h"

ResizeFilter::ResizeFilter(int w, int h)
    : newWidth(w), newHeight(h) {}

void ResizeFilter::apply(Image& img) {
    if (newWidth <= 0 || newHeight <= 0) return;
    if (img.width <= 0 || img.height <= 0) return;

    Image result(newWidth, newHeight);
    for (int x = 0; x < newWidth; ++x) {
        for (int y = 0; y < newHeight; ++y) {
            int oldx = x * img.width / newWidth;
            int oldy = y * img.height / newHeight;
            for (int c = 0; c < img.channels; ++c) {
                result(x, y, c) = img(oldx, oldy, c);
            }
        }
    }
    img = result;
}
