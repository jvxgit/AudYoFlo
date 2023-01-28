# Some common include files
set(PROJECT_NAME jvxSocketTestClient)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-audio-sockets-sm/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-sockets/include
	 )

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvx-sockets_static
	jvx-audio-sockets-sm_static
	)

if(JVX_USE_PCAP)
  set(LOCAL_COMPILE_DEFINITIONS "JVX_WITH_PCAP")
endif()

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
