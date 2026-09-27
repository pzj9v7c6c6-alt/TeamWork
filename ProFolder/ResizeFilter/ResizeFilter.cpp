#include "MyFilters_API.h"
#include "Image_Class.h"
#include<iostream>
using namespace std;
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

  // int main(){
//     string filename;
//     cout << "Pls enter  image name : ";
//     cin >> filename;
//         Image image(filename);

// FrameFilter f(500,500);
// f.apply(image);
// image.saveImage(filename);
// cout<<"Successfuly\n";
// }

