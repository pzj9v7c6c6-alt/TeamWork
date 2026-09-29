#pragma once
#include "../../../Interfaces/IImageFilter.h"

class WanoFilter : public IImageFilter {
public:
    WanoFilter();
    void apply(Image& img) override;
};
