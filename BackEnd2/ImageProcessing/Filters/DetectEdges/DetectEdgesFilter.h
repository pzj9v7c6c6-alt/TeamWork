#pragma once
#include "../../../Interfaces/IImageFilter.h"
#include "../BlackAndWhite/BlackAndWhiteFilter.h"
#include <cmath> 

class DetectEdgesFilter : public IImageFilter {
private:
    
    int sensitivitylevel;
    BlackAndWhiteFilter bwFilter;
public:
  
    DetectEdgesFilter(int sensitivitylevel = 50);

  
    void apply(Image& img) override;
};