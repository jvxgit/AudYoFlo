# common functions for all subprojects that create an executable
# PROJECT_NAME must be set on entry
include(${JVX_CMAKE_DIR}/common_local_head.cmake)
set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${JVX_CMAKE_CXX_FLAGS_EXEC}")
