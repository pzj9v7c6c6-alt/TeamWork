#pragma once

#include "Interfaces/IImageFilter.h"

class FlipHorizontalFilter : public IImageFilter {
public:
    FlipHorizontalFilter() = default;
    ~FlipHorizontalFilter() override = default;

    void apply(Image& image) override;
};

class FlipVerticalFilter : public IImageFilter {
public:
    FlipVerticalFilter() = default;
    ~FlipVerticalFilter() override = default;

    void apply(Image& image) override;
};