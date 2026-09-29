#pragma once

#include "Interfaces/IImageFilter.h"

class BlurFilter : public IImageFilter {
public:
    BlurFilter() = default;
    ~BlurFilter() override = default;

    void apply(Image& img) override;
};