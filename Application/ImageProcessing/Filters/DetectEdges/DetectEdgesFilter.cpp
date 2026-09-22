#include "pch.h"
#include "DetectEdgesFilter.h"
#include "DetectEdgesFilter.h"

DetectEdgesFilter::DetectEdgesFilter(int sensitivitylevel) : sensitivitylevel(sensitivitylevel) 
{
    
}

void DetectEdgesFilter::apply(Image& img) {
   
    bwFilter.apply(img);

    int width = img.width;
    int height = img.height;


    Image tempImg(width, height);

    
    for (int x = 0; x < width - 1; ++x) {
        for (int y = 0; y < height - 1; ++y) {

            unsigned char current = img.getPixel(x, y, 0);
            unsigned char right = img.getPixel(x + 1, y, 0);
            unsigned char down = img.getPixel(x, y + 1, 0);

            int diffX = std::abs(current - right);
            int diffY = std::abs(current - down);
            int totalDiff = diffX + diffY;

            if (totalDiff > sensitivitylevel) {
                tempImg.setPixel(x, y, 0, 0);
                tempImg.setPixel(x, y, 1, 0);
                tempImg.setPixel(x, y, 2, 0);
            }
            else {
                tempImg.setPixel(x, y, 0, 255);
                tempImg.setPixel(x, y, 1, 255);
                tempImg.setPixel(x, y, 2, 255);
            }
        }
    }

    img = tempImg;
}

