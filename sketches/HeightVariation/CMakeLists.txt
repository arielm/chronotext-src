cmake_minimum_required (VERSION 3.2.2)

project(HeightVariation)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/Noise.cpp"
  "$ENV{SRC_PATH}/common/NoiseSurface.cpp"
  "$ENV{SRC_PATH}/common/NoiseSurfaceSpiral1.cpp"
  "$ENV{SRC_PATH}/common/NoiseSurfaceGrid.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Button.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/ButtonLayout.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/TiledAtlas.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/UILock.cpp"
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
