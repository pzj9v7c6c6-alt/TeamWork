#pragma once
#include "../../../Interfaces/IImageFilter.h"

class TVScanlinesFilter : public IImageFilter {
private:
    float darknessFactor; 
    int lineSpacing;     

public:
   
    TVScanlinesFilter(float darknessFactor = 0.6f, int lineSpacing = 2);

    void apply(Image& img) override;
};