cmake_minimum_required(VERSION 3.16)

project(PresentationLayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ── Backend (Application) include path ──
# Allows #include "Interfaces/IImageFilter.h", "Models/Image_Class.h", etc.
set(APPLICATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Application")

# ── Presentation sources ──
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ImageEditorService.h
        ImageEditorService.cpp
        FilterRegistry.h
        TopBar.h
        TopBar.cpp
        ImageViewWidget.h
        ImageViewWidget.cpp
        FilterCard.h
        FilterCard.cpp
        FiltersPanel.h
        FiltersPanel.cpp
        StatusBar.h
        StatusBar.cpp
        SaveImageDialog.h
        SaveImageDialog.cpp
        # ── New PresentationLayer classes ──
        ThemeManager.h
        ThemeManager.cpp
        AssetExplorer.h
        AssetExplorer.cpp
        StickerItem.h
        StickerItem.cpp
        SelectionTool.h
        SelectionTool.cpp
        CropTool.h
        CropTool.cpp
        EditorTarget.h
        EditorTarget.cpp
)

# ── Backend filter sources ──
# Compiled as part of the presentation build since the Application
# directory has no standalone CMake build target.
set(BACKEND_SOURCES
        ${APPLICATION_DIR}/ImageProcessing/Filters/BlackAndWhite/BlackAndWhiteFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Brightness/BrightnessFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Crop/CropFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/DetectEdges/DetectEdgesFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Invert/InvertFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Rotate/RotateFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/TVScanlines/TVScanlinesFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/WanoPurple/WanoPurpleFilter.cpp
        # ── New concrete filters (same IImageFilter architecture) ──
        ${APPLICATION_DIR}/ImageProcessing/Filters/Contrast/ContrastFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Saturation/SaturationFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Hue/HueFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/BlurFilter/BlurFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/FrameFilter/FrameFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Gray/Grayfilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/InfraredFilter/InfraredFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Flip/FlipHorizontalFilterAndFlipVerticalFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/ResizeFilter/ResizeFilter.cpp
        ${APPLICATION_DIR}/ImageProcessing/Filters/Wanw/WanoFilter.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PresentationLayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${BACKEND_SOURCES}
    )
else()
    if(ANDROID)
        add_library(PresentationLayer SHARED
            ${PROJECT_SOURCES}
            ${BACKEND_SOURCES}
        )
    else()
        add_executable(PresentationLayer
            ${PROJECT_SOURCES}
            ${BACKEND_SOURCES}
        )
    endif()
endif()

target_include_directories(PresentationLayer PRIVATE ${APPLICATION_DIR})

target_link_libraries(PresentationLayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# ── STB duplicate-symbol fix ──
# Image_Class.h unconditionally #defines STB_IMAGE_IMPLEMENTATION before
# including stb headers. Since every filter .cpp transitively includes
# Image_Class.h, this would cause duplicate stb symbols at link time.
# Defining STB_IMAGE_STATIC / STB_IMAGE_WRITE_STATIC makes all stb
# functions "static", which confines them to each TU and eliminates
# duplicate-symbol linker errors.
target_compile_definitions(PresentationLayer PRIVATE
    STB_IMAGE_STATIC
    STB_IMAGE_WRITE_STATIC
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PresentationLayer)
endif()
set_target_properties(PresentationLayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PresentationLayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PresentationLayer)
endif()
