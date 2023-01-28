# Some common include files
set(PROJECT_NAME jvxRemoteCallServer)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSocket
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRemoteCall
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLsockets_st/include
	 )

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	jvxTSocket_static
	jvxTRemoteCall_static
	)

if (JVX_USE_PART_RS232)
	if(${JVX_OS} MATCHES "windows")
		include_directories(${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRs232Win32)
		set(LOCAL_LIBS ${LOCAL_LIBS} jvxTRs232Win32_static)
	endif()
	if(${JVX_OS} MATCHES "linux")
		include_directories(${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRs232Glnx)
		set(LOCAL_LIBS ${LOCAL_LIBS} jvxTRs232Glnx_static)
	endif()
endif()

if(JVX_USE_PCAP)
  set(LOCAL_COMPILE_DEFINITIONS "JVX_WITH_PCAP")
endif()

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
