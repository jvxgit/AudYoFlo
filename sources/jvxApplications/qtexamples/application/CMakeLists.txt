# Some common include files
set(PROJECT_NAME jvxQtApplication)

set(JVX_ADD_WIDGET_WRAPPER TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/application.qrc
	)
set(USE_QT_LIBRARIES TRUE)

# Source code files
set(LOCAL_SOURCES

    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
#	${UI_HEADERS}
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	)

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
