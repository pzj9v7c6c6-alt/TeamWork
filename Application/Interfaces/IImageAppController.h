#pragma once

#include "../Models/Image_Class.h"
#include "../ApplicationLayer/FilterType.h"

class IImageAppController {
public:
    virtual ~IImageAppController() = default;



    virtual bool applyFilter(FilterType type, Image& img) = 0;
};