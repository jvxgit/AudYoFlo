set(PROJECT_NAME jvxTGlobalLock)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGlobalLock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGlobalLock.h
  )

#set(LOCAL_LIBS jvx-sockets_static
#  )
set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
