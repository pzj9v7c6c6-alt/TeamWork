#pragma once

#include "Interfaces/IImageFilter.h"

class FrameFilter : public IImageFilter {
private:
    int thickness;
    int red, green, blue;
    bool decorated;

public:
    FrameFilter(int t = 20, int r = 37, int g = 99, int b = 235, bool d = true);
    ~FrameFilter() override = default;

    void apply(Image& img) override;
};