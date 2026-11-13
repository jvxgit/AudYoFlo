cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-audiotools)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)


set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/CjvxQtMeasureAudio_ir.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/CjvxQtMeasureAudio_jitter_qcp.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/CjvxQtMeasureAudio_spectrum.ui
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx-audiotools.qrc
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  ${JVX_SUBPRODUCT_ROOT}/software/helpers/
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/qt-audio-host
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common  
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-helpers/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/web/qwt/qwt/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/web/qcustomplot/qcustomplot
  ${QtWidgets_INCLUDE_DIRS}

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/stb-dejavu/include
  )

 set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
  
set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_ir.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_jitter.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_ir.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_jitter.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtMeasureAudioWidget.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_spectrum.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtMeasureAudio_spectrum.h
	)

set(LOCAL_LIBS
	qwt_static
	qcustomplot_static
	jvx-qt-helpers_static
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	)

set(BUILD_STATIC TRUE)

set(LOCAL_COMPILE_DEFINITIONS "JVX_USE_SPECTRUM_PLOT")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
