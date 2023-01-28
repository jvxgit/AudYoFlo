# Some common include files
set(PROJECT_NAME jvxUdpDataViewer)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLsockets_st/include
	 )

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/mySocketReport.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mySocketReport.h
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvxTSocket_static
	)

force_console_app()

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
