// Precompiled header stub for CMake builds.
// The Visual Studio project auto-generates this; we provide a
// placeholder so that backend .cpp files compile under CMake.
//
// IMPORTANT: We pre-include the stb headers in declaration-only mode
// so that when Image_Class.h is later included (by filter headers),
// the stb include guards prevent the declaration section from being
// re-parsed. This works because pch.h is included FIRST in every
// backend .cpp file.
#pragma once
