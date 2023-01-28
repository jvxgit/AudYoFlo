# Some common include files
set(PROJECT_NAME jvxQmlTest)

set(JVX_ADD_WIDGET_WRAPPER TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)
	
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)
	
# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/qrc/jvxQmlTest.qrc
	)
	
# Source code files
set(LOCAL_SOURCES

    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/backend.cpp
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvx-qt-helpers_static
	${JVX_QT_QML_LIB}
	)

set(USE_QT_LIBRARIES TRUE)
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(CONFIGURE_LOCAL_START_SCRIPT true)

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
