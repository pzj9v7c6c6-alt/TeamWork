#pragma once
#include "../Interfaces/ISaveAndLoad.h"

class Image;
class SaveAndLoad :public ISaveAndLoad
{
public:
    bool  SaveImage(Image& image, std::string path) override;
    bool LoadImage(std::string path, Image& image) override;
};



