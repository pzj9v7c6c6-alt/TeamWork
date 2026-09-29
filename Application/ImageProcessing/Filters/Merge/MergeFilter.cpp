#include "MergeFilter.h"


MergeFilter::MergeFilter(const std::string& secondImagePath)
    : secondImagePath(secondImagePath) {
}

void MergeFilter::apply(Image& image) {
    Image secondImage(secondImagePath);

    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            int orig_x = i * secondImage.width / image.width;
            int orig_y = j * secondImage.height / image.height;

            for (int c = 0; c < 3; ++c) {
                image(i, j, c) =
                    (image(i, j, c) + secondImage(orig_x, orig_y, c)) / 2;
            }
        }
    }
}