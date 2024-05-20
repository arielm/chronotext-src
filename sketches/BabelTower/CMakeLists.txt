cmake_minimum_required (VERSION 3.2.2)

project(BabelTower)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/TextHelix.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Button.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/ButtonLayout.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/TiledAtlas.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/UILock.cpp"
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
