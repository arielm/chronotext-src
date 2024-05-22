set(CTEST_PROJECT_NAME Chanson)
set(CTEST_CONFIGURATION_TYPE Release)

if (PLATFORM MATCHES osx)
  set(ARGS
    -DRUN=EXE
  )

elseif (PLATFORM MATCHES mxe)
  set(ARGS
    -DRUN=EXE
    -DFS=RC
  )
endif()

list(APPEND ARGS
  "-DPROTOBUF_ROOT=$ENV{CROSS_PATH}/deps/protobuf/dist/${PLATFORM}"
)

include("$ENV{CROSS_PATH}/core/cmake/platforms.cmake")
