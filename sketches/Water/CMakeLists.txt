cmake_minimum_required (VERSION 3.2.2)

project(Water)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/CinderCamera.cpp"
  "$ENV{SRC_PATH}/common/SeaSurface.cpp"
  "$ENV{SRC_PATH}/common/legacy/SeaPatch.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
