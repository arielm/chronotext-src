cmake_minimum_required (VERSION 3.2.2)

project(WarOfTheWords)

set(PUGIXML_ROOT "$ENV{CROSS_PATH}/tree/pugixml/${PLATFORM}")

set(INCLUDE_DIRS
  "${PUGIXML_ROOT}/include"
  $ENV{SRC_PATH}
)

set(LIBRARIES
  "${PUGIXML_ROOT}/lib/libpugixml.a"
)

set(SRC_FILES
  "src/Sketch.cpp"
  "src/WarTextBox.cpp"
  "src/TextSpan.cpp"
  "src/SearchManager.cpp"
  "src/Article.cpp"
  "src/Processor.cpp"
  "src/MissileManager.cpp"
  "src/Missile.cpp"
  "src/ExplosionHelper.cpp"
  "src/CameraManager.cpp"
  "$ENV{SRC_PATH}/common/ArcBall.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Button.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/ButtonLayout.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/TiledAtlas.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/UILock.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Container.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/Shape.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/TextBox.cpp"
  "$ENV{SRC_PATH}/common/legacy/ui/WordWrapper.cpp"
)

#
# For Mojave and up
#
add_compile_definitions(
  GL_SILENCE_DEPRECATION
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")
