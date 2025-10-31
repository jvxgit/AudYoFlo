# Some common include files
set(PROJECT_NAME myNewRtapRemoteControl)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(JVX_ADD_WIDGET_WRAPPER TRUE)

set(CMAKE_AUTOMOC ON)

set(LOCAL_UI_SOURCES
	# ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myCentralWidget.ui	
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc/myMainWidget.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc)
	
set(LOCAL_QT_RESOURCES
	# ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myCentralWidget.qrc
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc/jvxmyNewProject.qrc
	)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwt-${JVX_QWT_VERSION}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwtPolar-${JVX_QWT_POLAR_VERSION}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
	${JVX_BINARY_DIR}/base/sources/jvxFactoryHosts/jvxFHAppFactoryHost
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-webcontrol/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTGlobalLock/
	)

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHostHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src/myCentralWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src/myCentralWidget.cpp
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvx-qt-helpers_static
	jvx-qt-webcontrol-shared_static
	jvxFHAppFactoryHost_static
	jvxTSystemTextLog_static
	jvx-qt-app-templates_static
	)

if(JVX_ADD_WIDGET_WRAPPER)
	include_directories(
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
		)
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLQtSaWidgetWrapper_static
		jvxLQtSaWidgets_static
	)
	set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE;JVX_ADD_WIDGET_WRAPPER")
else()
	set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")
endif()

# Add http websocket stuff
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HTTP_CONTROL;JVX_COMPILE_REMOTE_CONTROL")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(CONFIGURE_LOCAL_START_SCRIPT true)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

## We may defice a minimal version here
#set(JVX_TARGET_NAME "${PROJECT_NAME}S")
#set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_SIMPLE_REMOTE_CONTROL")
#include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
