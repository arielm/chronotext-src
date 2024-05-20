cmake_minimum_required (VERSION 3.2.2)

project(TTC)

set(INCLUDE_DIRS
  $ENV{SRC_PATH}
)

set(SRC_FILES
  "src/main.cpp"
  "src/Sketch.cpp"
  "src/Reader.cpp"
  "src/Writer.cpp"
  "src/Branch.cpp"
  "src/Tree.cpp"
  "src/NavHistory.cpp"
  "src/EditHistory.cpp"
  "src/Stream.cpp"
  "src/Navigator.cpp"
  "src/UI.cpp"
  "src/Button.cpp"
  "src/ImageButton.cpp"
  "src/TextButton.cpp"
  "src/ImageItem.cpp"
  "src/TextItem.cpp"
  "src/ClockItem.cpp"
  "src/SharedGroup.cpp"
  "src/PlaybackGroup.cpp"
  "src/PlaybackScreen.cpp"
  "src/LiveScreen.cpp"
  "src/MainScreen.cpp"
  "src/AboutScreen.cpp"
  "src/TreeManager.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
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
