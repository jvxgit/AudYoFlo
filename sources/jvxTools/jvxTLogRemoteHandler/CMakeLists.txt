set(PROJECT_NAME jvxTLogRemoteHandler)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxLogRemoteHandler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxLogRemoteHandler.cpp
  )

# set(LOCAL_LIBS jvxLthreads_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
