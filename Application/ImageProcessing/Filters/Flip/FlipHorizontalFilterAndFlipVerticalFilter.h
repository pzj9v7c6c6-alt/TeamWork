#ifndef FLIP_HORIZONTAL_FILTER_AND_FLIP_VERTICAL_FILTER_H
#define FLIP_HORIZONTAL_FILTER_AND_FLIP_VERTICAL_FILTER_H

#include "ImageFilter.h"

class FlipHorizontalFilter : public Filter {
public:
    FlipHorizontalFilter();
    void apply(Image& image) override;
};

class FlipVerticalFilter : public Filter {
public:
    FlipVerticalFilter();
    void apply(Image& image) override;
};

#endif // FLIP_HORIZONTAL_FILTER_AND_FLIP_VERTICAL_FILTER_H
