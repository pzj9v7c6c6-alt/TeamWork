#pragma once
#include "../../../Interfaces/IImageFilter.h" 

class BlackAndWhiteFilter : public IImageFilter
{
public:
    BlackAndWhiteFilter();

    void apply(Image& img) override;

};

