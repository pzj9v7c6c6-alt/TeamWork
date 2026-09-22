#include "pch.h"
#include "RotateFilter.h"

RotateFilter::RotateFilter(int angle) : angle(angle) 
{
    
}

void RotateFilter::apply(Image& img)
{
    int oldWidth = img.width;
    int oldHeight = img.height;

   
    if (angle == 90) {
       
        Image tempImg(oldHeight, oldWidth);

        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
              
                int newX = oldHeight - 1 - y;
                int newY = x;

                // نسخ اللوان من المكان القديم للمكان الجديد
                for (int c = 0; c < 3; ++c) {
                    unsigned char color = img.getPixel(x, y, c);
                    tempImg.setPixel(newX, newY, c, color);
                }
            }
        }
        
        img = tempImg;
    }
   
    else if (angle == 180) {
        
        Image tempImg(oldWidth, oldHeight);

        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
                
                int newX = oldWidth  - 1 - x;
                int newY = oldHeight - 1 - y;

                for (int c = 0; c < 3; ++c) {
                    unsigned char color = img.getPixel(x, y, c);
                    tempImg.setPixel(newX, newY, c, color);
                }
            }
        }
        img = tempImg;
    }
   
    else if (angle == 270) {
       
        Image tempImg(oldHeight, oldWidth);

        for (int x = 0; x < oldWidth; ++x) {
            for (int y = 0; y < oldHeight; ++y) {
                
                int newX = y;
                int newY = oldWidth - 1 - x;

                for (int c = 0; c < 3; ++c) {
                    unsigned char color = img.getPixel(x, y, c);
                    tempImg.setPixel(newX, newY, c, color);
                }
            }
        }
        img = tempImg;
    }
}