cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-project)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/CjvxQtProject.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  #${JVX_SUBPRODUCT_ROOT}/software/helpers/
  #${CMAKE_CURRENT_BINARY_DIR}
  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  )

 set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
  
set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtProject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtProject.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtProject.h 
  )

set(LOCAL_LIBS
  jvx-qt-helpers_static
)
set(BUILD_STATIC TRUE)
#set(LOCAL_COMPILE_DEFINITIONS "NOMINMAX")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
