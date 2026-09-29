#include "FlipHorizontalFilterAndFlipVerticalFilter.h"
#include <utility>

using namespace std;

FlipHorizontalFilter::FlipHorizontalFilter() {}

void FlipHorizontalFilter::apply(Image& image) {
    for (int i = 0; i < image.width / 2; i++) {
        for (int j = 0; j < image.height; j++) {
            for (int c = 0; c < 3; c++) {
                swap(image(i, j, c), image(image.width - 1 - i, j, c));
            }
        }
    }
}

FlipVerticalFilter::FlipVerticalFilter() {}

void FlipVerticalFilter::apply(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height / 2; j++) {
            for (int c = 0; c < 3; c++) {
                swap(image(i, j, c), image(i, image.height - 1 - j, c));
            }
        }
    }
}
