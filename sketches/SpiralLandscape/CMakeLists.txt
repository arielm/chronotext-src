cmake_minimum_required (VERSION 3.2.2)

project(SpiralLandscape)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/Noise.cpp"
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
