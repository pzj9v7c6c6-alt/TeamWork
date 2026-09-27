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

// int main(){
//     string filename;
//     cout << "Pls enter  image name : ";
//     cin >> filename;
//         Image image(filename);

// FrameFilter f;
// f.apply(image);
// image.saveImage(filename);
// cout<<"Successfuly\n";
// }

