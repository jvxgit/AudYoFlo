# Some common include files
set(PROJECT_NAME jvxQtOpenGlWindow)

set(JVX_ADD_WIDGET_WRAPPER TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

 set(CMAKE_AUTOMOC ON)

#include_directories(
#	${CMAKE_CURRENT_SOURCE_DIR}/src
#	)

# Source code files
set(LOCAL_SOURCES

    ${CMAKE_CURRENT_SOURCE_DIR}/openglwindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	)

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(USE_QT_LIBRARIES TRUE)

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
