cmake_minimum_required (VERSION 3.2.2)

project(JS)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/Episode.cpp"
  "src/Episode1A.cpp"
  "src/Episode1B.cpp"
  "src/Episode2A.cpp"
  "src/Episode3A.cpp"
  "src/Episode2B.cpp"
  "src/Episode3B.cpp"

  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/SeaSurface.cpp"
  "$ENV{SRC_PATH}/common/SeaSurfaceSpiral1.cpp"
  "$ENV{SRC_PATH}/common/Noise.cpp"
  "$ENV{SRC_PATH}/common/NoiseSurface.cpp"
  "$ENV{SRC_PATH}/common/LineProvider.cpp"
  "$ENV{SRC_PATH}/common/legacy/HPath.cpp"
  "$ENV{SRC_PATH}/common/legacy/View.cpp"
  "$ENV{SRC_PATH}/common/legacy/FogManager.cpp"
  "$ENV{SRC_PATH}/common/legacy/FadeManager.cpp"
  "$ENV{SRC_PATH}/common/legacy/Helix.cpp"
  "$ENV{SRC_PATH}/common/legacy/NoisePatch.cpp"
  "$ENV{SRC_PATH}/common/legacy/TextNoisePatch.cpp"
  "$ENV{SRC_PATH}/common/legacy/SeaPatch.cpp"
  "$ENV{SRC_PATH}/common/legacy/TextArc.cpp"
  "$ENV{SRC_PATH}/common/legacy/TextRectangle.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
