#include "pch.h"
#include "BlackAndWhiteFilter.h"


FlipHorizontalFilterAndFlipVerticalFilter::BlackAndWhiteFilter() 
{
   
}

void BlackAndWhiteFilter::apply(Image& image) 
{
    
    

   
    for (int x = 0; x < image.width; ++x) {
        for (int y = 0; y < image.height; ++y) {

           
            unsigned char r = image.getPixel(x, y, 0);
            unsigned char g = image.getPixel(x, y, 1);
            unsigned char b = image.getPixel(x, y, 2);

            
            int avg = (r + g + b) / 3;

           
            unsigned char newValue = (avg >= 128) ? 255 : 0;

            
            image.setPixel(x, y, 0, newValue);
            image.setPixel(x, y, 1, newValue);
            image.setPixel(x, y, 2, newValue);
        }
    }
}


//   بيجيب المتوسط علشان مدى سطوع البكسل 
// لي 128 علشان  الوان البكسل من 255 ف المنوسط 128 