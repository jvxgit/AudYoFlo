if(JVX_USE_STEAM_AUDIO)
	# Some common include files
	set(PROJECT_NAME ayfSteamRirGenerator)

	include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

	include_directories(
		${STEAMAUDIO_INCLUDES}
	)

	# Source code files
	set(LOCAL_SOURCES
	    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/target/dr_wav.h
	)

	# Link with audio host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		 ${STEAMAUDIO_LIBRARY}
		)

	if(${JVX_OS} MATCHES "windows")
		jvx_install_dedicated_file(${STEAMAUDIO_DYNAMIC} ${INSTALL_PATH_BIN})
	endif()

	# Some common include files
	force_console_app()
	include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
endif()
