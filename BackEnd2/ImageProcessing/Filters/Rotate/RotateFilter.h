#pragma once
#include "../../../Interfaces/IImageFilter.h"

class RotateFilter : public IImageFilter {
private:
    int angle; 

public:
    
    RotateFilter(int angle);

    
    void apply(Image& img) override;
};