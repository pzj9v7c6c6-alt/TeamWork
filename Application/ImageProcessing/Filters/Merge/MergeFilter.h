#pragma once
#include "../../../Interfaces/IImageFilter.h"
#include <string>

class MergeFilter : public IImageFilter {

private:
    std::string secondImagePath;

public:
    explicit MergeFilter(const std::string& secondImagePath);

    void apply(Image& img) override;
};
