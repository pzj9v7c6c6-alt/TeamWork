#pragma once
#include <unordered_map>
#include <memory>
#include "../Interfaces/IImageAppController.h"
#include "../Interfaces/ISaveAndLoad.h"
#include "../Interfaces/IImageFilter.h"
#include "../Interfaces/IImageAppController.h"
#include "../Models/Image_Class.h"
#include "../Infrastructure/SaveAndLoad.h"

class ImageAppController : public IImageAppController, public SaveAndLoad {
public:



private:
    std::unordered_map<FilterType, std::shared_ptr<IImageFilter>> filterMap;

    void registerFilter(FilterType type, std::shared_ptr<IImageFilter> filter) {
        filterMap[type] = filter;
    }

public:
    ImageAppController();



    bool applyFilter(FilterType type, Image& img) override;
};