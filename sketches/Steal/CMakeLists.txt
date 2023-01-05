cmake_minimum_required (VERSION 3.2.2)

project(Steal)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/Noise.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
