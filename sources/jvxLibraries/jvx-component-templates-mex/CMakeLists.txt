cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-component-templates-mex)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
 
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxBareNode1ioMex.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxBareNode1ioMex.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
	${JVX_BASE_BINARY_INCLUDE_PATH}/jvx-component-templates-base/generated
  )

include_directories(${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include)
set(LOCAL_LIBS ${LOCAL_LIBS}
	jvxLexternalCall_static
	jvx-component-templates-base_static
	)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)


include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
