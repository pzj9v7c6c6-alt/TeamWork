#include "MyFilters_API.h"
#include "Image_Class.h"
#include<iostream>
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
void BlurFilter::apply(Image& img)
{


    
        Image result = img; 

        for (int i = 10; i < img.width - 10; ++i)
        {
            for (int j = 10; j < img.height - 10; ++j)
            {
                int SumR = 0, SumG = 0, SumB = 0;

                for (int x = -10; x <= 10; ++x)
                {
                    for (int y = -10; y <= 10; ++y)
                    {
                        SumR += img(i + x, j + y, 0);
                        SumG += img(i + x, j + y, 1);
                        SumB += img(i + x, j + y, 2);
                    }
                }

                result(i, j, 0) = SumR / 441;
                result(i, j, 1) = SumG / 441;
                result(i, j, 2) = SumB / 441;
            }
        }

        img = result;
    }
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
  FrameFilter::FrameFilter(int t,int r,int g,int b,bool d){
    thickness=t;
    red=r;
    green=g;
    blue=b;
    decorated=d;
  }
void FrameFilter::apply(Image&img){
    for (int x = 0; x< img.width; ++x)
    {
       for (int y = 0; y < img.height; y++)
       {
        bool frame=x<thickness||x>=img.width-thickness||y<thickness||y>=img.height-thickness;
        if(frame){
            img(x,y,0)=red;
            img(x,y,1)=green;
            img(x,y,2)=blue;
        }
        if(decorated){
            int inner=thickness+10;
            if(x==inner||x==img.width-inner-1||
            y==inner||y==img.height-inner-1){
                img(x,y,0)=red;
            img(x,y,1)=green;
            img(x,y,2)=blue;
            }
            
        }
       }
       
    }
// int main(){
//     string filename;
//     cout << "Pls enter  image name : ";
//     cin >> filename;
//         Image image(filename);

// FrameFilter f(20,0,255,0,true);
// f.apply(image);
// image.saveImage(filename);
// cout<<"Successfuly\n";
// }

