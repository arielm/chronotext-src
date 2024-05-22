cmake_minimum_required (VERSION 3.2.2)

project(Chanson)

set(PUGIXML_ROOT "$ENV{CROSS_PATH}/tree/pugixml/${PLATFORM}")
set(FMOD_ROOT "$ENV{SRC_PATH}/tree/FMOD/${PLATFORM}")

set(INCLUDE_DIRS
  "${PUGIXML_ROOT}/include"
  "${FMOD_ROOT}/include"
  $ENV{SRC_PATH}
)

set(LIBRARIES
  "${PUGIXML_ROOT}/lib/libpugixml.a"
)

if (PLATFORM MATCHES osx)
  set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_INSTALL_RPATH "@loader_path/../lib")

  find_library(FMOD_LIBRARY NAMES fmod PATHS "${FMOD_ROOT}/lib" REQUIRED)
  list(APPEND LIBRARIES
    "${FMOD_LIBRARY}"
  )
elseif (PLATFORM MATCHES mxe)
  list(APPEND LIBRARIES
    "${FMOD_ROOT}/lib/libfmod.a"
  )
endif()

set(SRC_FILES
  "src/Sketch.cpp"
  "src/SeaBrickSpiral.cpp"
  "src/AudioManager.cpp"
  "src/Episode.cpp"
  "src/Episode1.cpp"
  "src/Episode2.cpp"
  "src/BrickLine.cpp"
  "src/Camera.cpp"
  "src/WaveSurface.cpp"
  "src/WaveSurfaceLine.cpp"
  "src/Episode3.cpp"
  "src/WaveBrickLine.cpp"
  "src/ThinWaveBrickLine.cpp"
  "src/ThinFlatBrickLine.cpp"
  "src/Run.cpp"
  "src/Episode4.cpp"
  "$ENV{SRC_PATH}/common/xf/Font.cpp"
  "$ENV{SRC_PATH}/common/xf/FontManager.cpp"
  "$ENV{SRC_PATH}/common/xf/FontSequence.cpp"
  "$ENV{SRC_PATH}/common/quad/QuadBatch.cpp"
  "$ENV{SRC_PATH}/common/JavaRandom.cpp"
  "$ENV{SRC_PATH}/common/SeaSurface.cpp"
  "$ENV{SRC_PATH}/common/SeaSurfaceSpiral2.cpp"
)

include("$ENV{CROSS_PATH}/core/cmake/sketch.cmake")
include("$ENV{CROSS_PATH}/core/cmake/test.cmake")

set_target_properties(${PROJECT_NAME} PROPERTIES
  INSTALL_RPATH "${FMOD_ROOT}/lib"
  BUILD_WITH_INSTALL_RPATH TRUE
)
