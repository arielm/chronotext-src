cmake_minimum_required (VERSION 3.2.2)

project(TwitterMaze)

set(JSONCPP_ROOT "$ENV{CROSS_PATH}/tree/jsoncpp/${PLATFORM}")

set(INCLUDE_DIRS
  "${JSONCPP_ROOT}/include"
  $ENV{SRC_PATH}
)

set(LIBRARIES
  "${JSONCPP_ROOT}/lib/libjsoncpp.a"
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/SpiralPath.cpp"
  "$ENV{SRC_PATH}/common/ArcBall.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
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
