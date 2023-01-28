cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qcp-qt-helpers)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)


set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-qcp-qt-helpers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-qcp-qt-helpers.h
	)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/web/qcustomplot/qcustomplot
	)

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
  	qcustomplot_static
  )

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
