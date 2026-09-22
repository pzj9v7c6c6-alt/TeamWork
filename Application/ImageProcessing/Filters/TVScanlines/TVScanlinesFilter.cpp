#include "pch.h"
#include "TVScanlinesFilter.h"
TVScanlinesFilter::TVScanlinesFilter(float darknessFactor, int lineSpacing)
    : darknessFactor(darknessFactor), lineSpacing(lineSpacing)
{
    
    if (this->darknessFactor < 0.0f) this->darknessFactor = 0.0f;
    if (this->darknessFactor > 1.0f) this->darknessFactor = 1.0f;

  
    if (this->lineSpacing < 2) this->lineSpacing = 2;
}

void TVScanlinesFilter::apply(Image& img) {
    int width = img.width;
    int height = img.height;

    
    for (int y = 0; y < height; ++y) {

        
        if (y % lineSpacing == 0) {

           
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < 3; ++c) { 

                    unsigned char originalColor = img.getPixel(x, y, c);

                    
                    unsigned char dimmedColor = static_cast<unsigned char>(originalColor * darknessFactor);

                    img.setPixel(x, y, c, dimmedColor);
                }
            }
        }
    }
}