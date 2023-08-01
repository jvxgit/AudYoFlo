if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-buffer-sync)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack/include
#	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxBufferSync.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxBufferSync.h
  )

set(LOCAL_LIBS jvx-audio-stack_static)

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
