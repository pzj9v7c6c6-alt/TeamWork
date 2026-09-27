#include "WanoFilter.h"
#include <algorithm> // علشان يشتغل min

using namespace std;

WanoFilter::WanoFilter() {}

void WanoFilter::apply(Image& image) {
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            int red   = image(i, j, 0) + 40;
            int green = image(i, j, 1) + 20;
            int blue  = image(i, j, 2);

            image(i, j, 0) = min(255, red);
            image(i, j, 1) = min(255, green);
            image(i, j, 2) = min(255, blue);
        }
    }
}