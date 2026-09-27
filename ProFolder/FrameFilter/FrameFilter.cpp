#include "MyFilters_API.h"
#include "Image_Class.h"
#include<iostream>
using namespace std;
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

