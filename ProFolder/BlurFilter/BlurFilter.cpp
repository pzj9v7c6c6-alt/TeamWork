#include "MyFilters_API.h"
#include "Image_Class.h"
#include<iostream>
using namespace std;
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

