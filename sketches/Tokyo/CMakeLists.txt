cmake_minimum_required (VERSION 3.2.2)

project(Tokyo)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{SRC_PATH}/common/TextRectangle.cpp"
  "$ENV{SRC_PATH}/common/TextRing.cpp"
  "$ENV{SRC_PATH}/common/LineProvider.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Button.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/ButtonLayout.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/TiledAtlas.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/UILock.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
