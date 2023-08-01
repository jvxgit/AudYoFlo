if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-json)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  #${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/jsmn
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/jsmn/jsmn
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxJson.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxJson.h
  )

#set(LOCAL_LIBS
#  jsmn
#  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
