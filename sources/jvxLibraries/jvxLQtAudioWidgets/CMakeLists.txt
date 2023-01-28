cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLQtAudioWidgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtBeampatternViewer.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  ${JVX_SUBPRODUCT_ROOT}/software/helpers/
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/qt-audio-host
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common  
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qcustomplot
  )

 set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
  
set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtBeampatternViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtBeampatternViewer.h
  )

set(LOCAL_LIBS
  qwt_static
  qcustomplot_static
  jvx-qt-helpers_static
)
set(BUILD_STATIC TRUE)
#set(LOCAL_COMPILE_DEFINITIONS "NOMINMAX")

set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
