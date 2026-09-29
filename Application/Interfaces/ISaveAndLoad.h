#pragma once
#include <string>
#include "../Models/Image_Class.h"
class ISaveAndLoad {
public:
    virtual  bool  SaveImage(Image& image, std::string path) = 0;
    virtual bool LoadImage(std::string path, Image& image) = 0;
};