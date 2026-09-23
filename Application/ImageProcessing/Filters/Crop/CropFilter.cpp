
#include "CropFilter.h"

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