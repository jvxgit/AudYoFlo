# Some common include files
set(PROJECT_NAME jvxQtWebControl)

set(JVX_ADD_WIDGET_WRAPPER TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myCentralWidget.ui	
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)	

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
	${JVX_BINARY_DIR}/base/sources/jvxFactoryHosts/jvxFHAppFactoryHost
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-webcontrol/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
	)

# Source code files
set(LOCAL_SOURCES

    ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHostHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvx-qt-helpers_static
	jvx-qt-webcontrol-shared_static	
	jvxFHAppFactoryHost_static
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

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(CONFIGURE_LOCAL_START_SCRIPT true)

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
