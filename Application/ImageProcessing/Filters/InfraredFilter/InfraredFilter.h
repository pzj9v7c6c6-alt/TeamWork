#pragma once

#include "Interfaces/IImageFilter.h"

class InfraredFilter : public IImageFilter {
public:
    InfraredFilter() = default;
    ~InfraredFilter() override = default;

    void apply(Image& img) override;
};