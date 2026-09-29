#include "pch.h"
#include "SaveAndLoad.h"
#include "Interfaces/ISaveAndLoad.h"
#include "../Models/Image_Class.h"


bool SaveAndLoad::LoadImage(std::string path, Image& image) {
    try {

        if (image.loadNewImage(path)) {
            return true;
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return false;
    }
    return false;

}

bool SaveAndLoad::SaveImage(Image& image, std::string path) {
    try {
        if (image.saveImage(path))
            return true;

    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return false;
    }
    return false;
}

