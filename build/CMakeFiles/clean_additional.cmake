# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\PresentationLayer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PresentationLayer_autogen.dir\\ParseCache.txt"
  "PresentationLayer_autogen"
  )
endif()
