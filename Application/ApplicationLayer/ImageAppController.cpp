#include "ImageAppController.h"


#include "ImageProcessing/Filters/BlackAndWhite/BlackAndWhiteFilter.h"
#include "ImageProcessing/Filters/BlurFilter/BlurFilter.h"
#include "ImageProcessing/Filters/Brightness/BrightnessFilter.h"
#include "ImageProcessing/Filters/Contrast/ContrastFilter.h"
#include "ImageProcessing/Filters/Crop/CropFilter.h"
#include "ImageProcessing/Filters/DetectEdges/DetectEdgesFilter.h"
#include "ImageProcessing/Filters/Flip/FlipHorizontalFilterAndFlipVerticalFilter.h"
#include "ImageProcessing/Filters/FrameFilter/FrameFilter.h"
#include "ImageProcessing/Filters/Gray/GrayFilter.h"
#include "ImageProcessing/Filters/Hue/HueFilter.h"
#include "ImageProcessing/Filters/InfraredFilter/InfraredFilter.h"
#include "ImageProcessing/Filters/Invert/InvertFilter.h"
#include "ImageProcessing/Filters/Merge/MergeFilter.h"
#include "ImageProcessing/Filters/ResizeFilter/ResizeFilter.h"
#include "ImageProcessing/Filters/Rotate/RotateFilter.h"
#include "ImageProcessing/Filters/Saturation/SaturationFilter.h"
#include "ImageProcessing/Filters/TVScanlines/TVScanlinesFilter.h"
#include "ImageProcessing/Filters/WanoPurple/WanoPurpleFilter.h"
#include "ImageProcessing/Filters/Wanw/WanoFilter.h"

ImageAppController::ImageAppController() {

    registerFilter(FilterType::BlackAndWhite, std::make_shared<BlackAndWhiteFilter>());
    registerFilter(FilterType::Blur, std::make_shared<BlurFilter>());

    registerFilter(FilterType::Brightness, std::make_shared<BrightnessFilter>(20));
    registerFilter(FilterType::Contrast, std::make_shared<ContrastFilter>(20));
    registerFilter(FilterType::Crop, std::make_shared<CropFilter>(0, 0, 100, 100));

    registerFilter(FilterType::DetectEdges, std::make_shared<DetectEdgesFilter>());
    registerFilter(FilterType::Flip, std::make_shared<FlipHorizontalFilter>());
    registerFilter(FilterType::FrameFilter, std::make_shared<FrameFilter>());
    registerFilter(FilterType::Gray, std::make_shared<Grayfilter>());

    registerFilter(FilterType::Hue, std::make_shared<HueFilter>(30));

    registerFilter(FilterType::InfraredFilter, std::make_shared<InfraredFilter>());
    registerFilter(FilterType::Invert, std::make_shared<InvertFilter>());

    registerFilter(FilterType::Merge, std::make_shared<MergeFilter>("second_image.jpg"));

    registerFilter(FilterType::ResizeFilter, std::make_shared<ResizeFilter>(800, 600));
    registerFilter(FilterType::Rotate, std::make_shared<RotateFilter>(90));
    registerFilter(FilterType::Saturation, std::make_shared<SaturationFilter>(20));

    registerFilter(FilterType::TVScanlines, std::make_shared<TVScanlinesFilter>());
    registerFilter(FilterType::WanoPurple, std::make_shared<WanoPurpleFilter>());
    registerFilter(FilterType::Wanw, std::make_shared<WanoFilter>());
}

bool ImageAppController::applyFilter(FilterType type, Image& img) {
    auto it = filterMap.find(type);
    if (it != filterMap.end() && it->second) {
        it->second->apply(img);
        return true;
    }
    return false;
}