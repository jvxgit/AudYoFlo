cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-measure-ir)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(CMAKE_AUTOMOC ON)

set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxAcousticMeasurement.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxMeasureControl.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxAcousticEqualizer.ui
	)

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)


# Qt resource file add
#set(LOCAL_QT_RESOURCES
#	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx-signals.qrc
#)

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/web/qcustomplot/qcustomplot
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
		${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNMeasureIr
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qcp-qt-helpers/include

		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  )
  
set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure_iface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure_plot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure_edit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure_config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure_markers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticMeasure.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticEqualizer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAcousticEqualizer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxMeasureControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxMeasureControl.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtAcousticMeasurement.h
	)

set(LOCAL_LIBS
	jvx-dsp-base-cpp_static
	jvx-qcp-qt-helpers_static
	jvxSpNMeasureIr_static
	)

set(BUILD_STATIC TRUE)

# set(LOCAL_COMPILE_DEFINITIONS "JVX_USE_SPECTRUM_PLOT")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

# add_dependencies(${PROJECT_NAME}_static jvxSpNMeasureIr)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
