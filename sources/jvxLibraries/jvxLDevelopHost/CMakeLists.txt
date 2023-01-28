cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME_ROOT jvxLDevelopHost)
set(PROJECT_NAME ${PROJECT_NAME_ROOT})

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

#set(CMAKE_AUTOMOC ON)

set(LOCAL_UI_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/messagesOutput.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewer.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerPlots.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerProperties.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerOneProperty.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerOneLinearPlot.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerTDSPlot.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/realtimeViewerOnePolarPlot.ui
  )

if(DEFINED JVX_QT_DEVELOP_HOST_SPECIFIC_APPLICATION_UI)
	set(LOCAL_UI_SOURCES ${LOCAL_UI_SOURCES} ${JVX_INTEGRATE_SOURCES_PATH}/sources/jvxLibraries/${PROJECT_NAME}/src/mainApplication.ui)
	set(LOCAL_UI_SOURCE_PATH ${JVX_INTEGRATE_SOURCES_PATH}/sources/jvxLibraries/${PROJECT_NAME}/src)
else()
	set(LOCAL_UI_SOURCES ${LOCAL_UI_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainApplication.ui)
	set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)
endif()

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_menu.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_bootup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_shutdown.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_sys_cb.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_control.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_ui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/messagesOutput.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewer_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerPlots.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerOneProperty.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerOneLinearPlot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerTDSPlot.cpp
  
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common/realtimeViewer_helpers.cpp
  	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/target/main.cpp
	
  ${JVX_INTEGRATE_SOURCES_PATH}/sources/jvxLibraries/${PROJECT_NAME}/src/uMainWindow_specific.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/messagesOutput.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewer_widgets.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerProperties.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerPlots.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerOneProperty.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerOneLinearPlot.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/realtimeViewerTDSPlot.h
  
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common/realtimeViewer_helpers.h

  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_SUBPRODUCT_ROOT}/software/interfaces/qt-develop-host
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_INTEGRATE_SOURCES_PATH}/software/codeFragments/jvxHosts/common
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/src

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/src

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreads/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

if(JVX_OS MATCHES "windows")
	if(NOT JVX_USE_CONSOLE_APPS)
		set(LOCAL_COMPILE_DEFINITIONS "")
	endif()
endif()

set(LOCAL_LIBS
	jvxLthreads_static
	qwt_static
	${JVX_QT_LIBS}
	jvx-qt-host-widgets_static
	jvx-qt-helpers_static
	jvxTconfigProcessor_static
    jvxTSystemTextLog_static
	jvxTThreads_static
	jvxTDataLogger_static
	)

include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)

set(LOCAL_LIBS_ALL ${LOCAL_LIBS})

set(BUILD_STATIC TRUE)

set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-shared_static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-shared_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-shared_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-static_static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-static_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HOST_USE_ONLY_STATIC_OBJECTS")
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-static_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
