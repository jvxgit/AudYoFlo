set(JVX_TARGET_NAME "ffplay")
project (${JVX_TARGET_NAME})

include(${JVX_CMAKE_DIR}/common_local_head.cmake)

	# CFLAGS as used when building ffmpeg:
	# -D_ISOC99_SOURCE -D -D_LARGEFILE_SOURCE -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_WIN32_WINNT=0x0600
if(${JVX_OS} MATCHES "windows")
	set(LOCAL_C_FLAGS "_FILE_OFFSET_BITS=64;_USE_MATH_DEFINES;_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_WARNINGS;_WIN32_WINNT=0x0600")
endif()
set(LOCAL_C_STANDARD 99)

set(LOCAL_SOURCES

	
	${JVX_FFMPEG_SOURCE_DIR}/fftools/ffplay.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/cmdutils.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/opt_common.c
 )

# set_source_files_properties(${LOCAL_SOURCES} PROPERTIES LANGUAGE CXX )

include_directories(
	${JVX_FFMPEG_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools
	${SDL_LIBRARY_PATH}/include
  )

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(LOCAL_LIBS 
		${JVX_FFMPEG_LIBS}
		${JVX_SOCKET_LIBRARIES}
		${JVX_FFMPEG_SYS_LIBS}
		${SDL_LIBRARY}		
		)

if(${JVX_OS} MATCHES "windows")

	if(${JVX_PLATFORM} MATCHES "64bit")
		jvx_install_dedicated_file(${SDL_LIBRARY_PATH}/lib/x64/SDL2.dll ${INSTALL_PATH_BIN})
	else()
		jvx_install_dedicated_file(${SDL_LIBRARY_PATH}/lib/x86/SDL2.dll ${INSTALL_PATH_BIN})
	endif()
endif()

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

# Run in Visual Studio

# Command: $(TargetPath)
# Command Argument: "E:\jvxrt\audio\release\runtime\wavs\music_stereo_48000Hz.wav"
# Working Directory: ""..\..\..\..\..\..\release\runtime\bin""

