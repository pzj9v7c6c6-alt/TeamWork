#pragma once

#include "Interfaces/IImageFilter.h"

class Grayfilter : public IImageFilter {
public:
    Grayfilter() = default;
    ~Grayfilter() override = default;

    void apply(Image& img) override;
};