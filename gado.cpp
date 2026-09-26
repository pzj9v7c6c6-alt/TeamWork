#include <iostream>
#include <bits/stdc++.h>
#include "Image_Class.h"
using namespace std;

class Filter {
public:
    virtual void apply(Image& image) = 0;
    virtual ~Filter() {}
};

class GrayFilter : public Filter {
public:
    void apply(Image& image) override {
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
};

class FlipHorizontalFilter : public Filter {
public:
    void apply(Image& image) override {
        for (int i = 0; i < image.width / 2; i++) {
            for (int j = 0; j < image.height; j++) {
                for (int c = 0; c < 3; c++) {
                    swap(image(i, j, c), image(image.width - 1 - i, j, c));
                }
            }
        }
    }
};

class FlipVerticalFilter : public Filter {
public:
    void apply(Image& image) override {
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height / 2; j++) {
                for (int c = 0; c < 3; c++) {
                    swap(image(i, j, c), image(i, image.height - 1 - j, c));
                }
            }
        }
    }
};

class MergeFilter : public Filter {
private:
    Image secondImage;

public:
    MergeFilter(const string& secondImagePath) : secondImage(secondImagePath) {}

    void apply(Image& image) override {
        Image resizedSecond(image.width, image.height);

        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height; ++j) {
                
                int orig_x = i * secondImage.width / image.width;
                int orig_y = j * secondImage.height / image.height;

                for (int c = 0; c < 3; ++c) {
                    resizedSecond(i, j, c) = secondImage(orig_x, orig_y, c);
                }
            }
        }
        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height; ++j) {
                for (int c = 0; c < 3; ++c) {
                    image(i, j, c) = (image(i, j, c) + resizedSecond(i, j, c)) / 2;
                }
            }
        }
    }
};
class WanoFilter : public Filter {
public:
    void apply(Image& image) override {
        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height; ++j) {
          
                int red = image(i, j, 0) + 40;
                int green = image(i, j, 1) + 20;
                int blue = image(i, j, 2);
                image(i, j, 0) = min(255, red);
                image(i, j, 1) = min(255, green);
                image(i, j, 2) = min(255, blue);
            }
        }
    }
};
int main() {
    Image img("luffy.jpg");
    MergeFilter fliv("mario.bmp");
    fliv.apply(img);
    img.saveImage("output.jpg");
    cout << "Done! Filter applied successfully." << endl;
    return 0;
}
