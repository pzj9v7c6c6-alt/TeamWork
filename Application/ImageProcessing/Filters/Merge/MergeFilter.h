#pragma once
#include "../../../Interfaces/IImageFilter.h"
#include <string>

class MergeFilter : public IImageFilter
{
private:
    std::string secondImagePath;

public:
    MergeFilter(const std::string& secondImagePath);
    void apply(Image& image) override;
};
