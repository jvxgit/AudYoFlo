cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-mixmatrix-cpp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILE_OPTIONS
	-I ${CMAKE_CURRENT_SOURCE_DIR}/pcg
	)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  ${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-mixmatrix/include
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMixMatrix.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMixMatrix.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/IjvxMixMatrix.h
	)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/CjvxMixMatrix.pcg
	)

set(BUILD_STATIC TRUE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
