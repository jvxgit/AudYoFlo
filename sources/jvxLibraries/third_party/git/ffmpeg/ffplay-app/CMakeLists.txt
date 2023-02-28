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

	
	${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffplay.c
	# ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_demux.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_filter.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_hw.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_mux.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_mux_init.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/ffmpeg_opt.c
	${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/cmdutils.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/objpool.c
	${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/opt_common.c
	# ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/sync_queue.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/../ffmpeg/fftools/thread_queue.c
	# ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/fftools/fftoolsres.rc
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

set(LOCAL_LIBS ${JVX_FFMPEG_LIB_AVCODEC}
		${JVX_FFMPEG_LIB_AVDEVICE}
		${JVX_FFMPEG_LIB_AVFILTER}
		${JVX_FFMPEG_LIB_AVFORMAT}
		${JVX_FFMPEG_LIB_AVUTIL}
		${JVX_FFMPEG_LIB_SWRESAMPLE}
		${JVX_FFMPEG_LIB_SWSCALE}
		${JVX_SOCKET_LIBRARIES}
		${SDL_LIBRARY}
		psapi.lib strmiids.lib 
		uuid.lib shlwapi.lib 
		vfw32.lib  secur32.lib 
		mfuuid.lib strmiids.lib bcrypt.lib  
		# ole32.lib shell32.lib user32.lib gdi32.lib oleaut32.lib 
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

