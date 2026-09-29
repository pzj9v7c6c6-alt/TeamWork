
#include "Image_Class.h"
#include<iostream>
#include<string>

#include "InfraredFilter.h"
#include "ResizeFilter.h"

#include "RotateFilter.h"
#include "BlackAndWhiteFilter.h"

#include "CropFilter.h"
#include "InvertFilter.h"

#include "Grayfilter.h"
#include "FlipHorizontalFilterAndFlipVerticalFilter.h"
#include <utility>



using namespace std;

void InfraredFilter::apply(Image & img)
{
for (int i = 0; i < img.width; ++i)
{
    for (int j = 0; j < img.height; ++j)
    {
        unsigned char g=img(i,j,1);
        unsigned char b=img(i,j,2);
        img(i,j,0)=255;
        img(i,j,1)=255-g;
        img(i,j,2)=255-b;
    }
    
}

}

//--------------------------------------------------------

ResizeFilter::ResizeFilter(int w,int h){
        newWidth=w;
        newHight=h;
    }
  void ResizeFilter::apply(Image&img){
    Image Result(newWidth,newHight);
    for (int x = 0; x < newWidth; ++x)
    {
        for (int y = 0; y < newHight; ++y)
        {
            int oldx=x*img.width/newWidth;
            int oldy=y*img.height/newHight;
            for (int c = 0; c < img.channels;++c)
            {
                Result(x,y,c)=img(oldx,oldy,c);
            }
            

        }
        
    }
    img=Result;
  }


//--------------------------------------------------------

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


//--------------------------------------------------------


BlackAndWhiteFilter::BlackAndWhiteFilter() 
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

//------------------------------------------------------

CropFilter::CropFilter(int x, int y, int width, int height)
    : _x(x), _y(y), NewWidth(width), NewHeight(height)
{
}

void CropFilter::apply(Image& image)
{
    if (_x < 0 || _y < 0 || NewHeight <= 0 || NewWidth <= 0) return;
    if (NewHeight > image.height || NewWidth > image.width) return;
    if (_x + NewWidth > image.width || _y + NewHeight > image.height) return;
    Image newImage = Image(NewWidth, NewHeight);

    for (int x = 0; x < NewWidth; ++x) {
        for (int y = 0; y < NewHeight; ++y) {
            for (int c = 0; c < image.channels; ++c) {
                int OX = _x + x;
                int OY = _y + y;
                int pix = image.getPixel(OX, OY, c);
                newImage.setPixel(x, y, c, pix);


            }
        }
    }

    image = newImage;

}

//----------------------------------------------------------


InvertFilter::InvertFilter()
{
}

void InvertFilter::apply(Image& image)
{

    for (int _x = 0; _x < image.width; ++_x) {
        for (int _y = 0; _y < image.height; ++_y) {
            for (int _c = 0; _c < image.channels;++_c) {
                int pix = image.getPixel(_x, _y, _c);
                pix = 255 - pix;
                image.setPixel(_x, _y, _c, pix);

            }
        }
    }

}


//----------------------------------------------------------


Grayfilter::Grayfilter() {}

void Grayfilter::apply(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            unsigned char red   = image(i, j, 0);
            unsigned char green = image(i, j, 1);
            unsigned char blue  = image(i, j, 2);

            unsigned char gray  = (red + green + blue) / 3;

            image(i, j, 0) = gray;
            image(i, j, 1) = gray;
            image(i, j, 2) = gray;
        }
    }
}


//----------------------------------------------------------




FlipHorizontalFilter::FlipHorizontalFilter() {}

void FlipHorizontalFilter::apply(Image& image) {
    for (int i = 0; i < image.width / 2; i++) {
        for (int j = 0; j < image.height; j++) {
            for (int c = 0; c < 3; c++) {
                swap(image(i, j, c), image(image.width - 1 - i, j, c));
            }
        }
    }
}

FlipVerticalFilter::FlipVerticalFilter() {}

void FlipVerticalFilter::apply(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height / 2; j++) {
            for (int c = 0; c < 3; c++) {
                swap(image(i, j, c), image(i, image.height - 1 - j, c));
            }
        }
    }
}



//----------------------------------------------------------

int main() {
    string inputFile, outputFile;
    int choice;

    cout << "===== IMAGE FILTER MENU =====\n";
    cout << "1. Infrared Filter\n";
    cout << "2. Resize Filter\n";
    cout << "3. Rotate Filter\n";
    cout << "4. Black and White Filter\n";
    cout << "5. Crop Filter\n";
    cout << "6. Invert Filter\n";
    cout << "7. Gray Filter\n";
    cout << "8. Flip Horizontal\n";
    cout << "9. Flip Vertical\n";
    cout << "Enter your choice: ";
    cin >> choice;

    cout << "Enter input image filename: ";
    cin >> inputFile;

    Image image(inputFile);

    switch (choice) {
    case 1: {
        InfraredFilter filter;
        filter.apply(image);
        break;
    }

    case 2: {
        int w, h;
        cout << "Enter new width and height: ";
        cin >> w >> h;

        ResizeFilter filter(w, h);
        filter.apply(image);
        break;
    }

    case 3: {
        int angle;
        cout << "Enter angle (90, 180, or 270): ";
        cin >> angle;

        RotateFilter filter(angle);
        filter.apply(image);
        break;
    }

    case 4: {
        BlackAndWhiteFilter filter;
        filter.apply(image);
        break;
    }

    case 5: {
        int x, y, w, h;
        cout << "Enter x, y, width, height: ";
        cin >> x >> y >> w >> h;

        CropFilter filter(x, y, w, h);
        filter.apply(image);
        break;
    }

    case 6: {
        InvertFilter filter;
        filter.apply(image);
        break;
    }

    case 7: {
        Grayfilter filter;
        filter.apply(image);
        break;
    }

    case 8: {
        FlipHorizontalFilter filter;
        filter.apply(image);
        break;
    }

    case 9: {
        FlipVerticalFilter filter;
        filter.apply(image);
        break;
    }

    default:
        cout << "Invalid choice!\n";
        return 1;
    }

    cout << "Enter output image filename: ";
    cin >> outputFile;

    image.saveImage(outputFile);

    cout << "Filter applied and image saved successfully!\n";

    return 0;
}