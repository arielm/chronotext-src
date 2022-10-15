cmake_minimum_required (VERSION 3.2.2)

project(Desert)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/CinderCamera.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/Noise.cpp"
  "$ENV{SRC_PATH}/common/NoiseSurface.cpp"
  "$ENV{SRC_PATH}/common/legacy/NoisePatch.cpp"
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
