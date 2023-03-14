set(JVX_TARGET_NAME "ffmpeg")
project (${JVX_TARGET_NAME})

include(${JVX_CMAKE_DIR}/common_local_head.cmake)

	# CFLAGS as used when building ffmpeg:
	# -D_ISOC99_SOURCE -D -D_LARGEFILE_SOURCE -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_WIN32_WINNT=0x0600
if(${JVX_OS} MATCHES "windows")
	set(LOCAL_C_FLAGS "_FILE_OFFSET_BITS=64;_USE_MATH_DEFINES;_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_WARNINGS;_WIN32_WINNT=0x0600")
endif()
set(LOCAL_C_STANDARD 99)

set(LOCAL_SOURCES

	${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_demux.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_filter.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_hw.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_mux.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_mux_init.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/ffmpeg_opt.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/cmdutils.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/objpool.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/opt_common.c
	${JVX_FFMPEG_SOURCE_DIR}/fftools/sync_queue.c
    ${JVX_FFMPEG_SOURCE_DIR}/fftools/thread_queue.c	# ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/fftools/fftoolsres.rc
 )

# set_source_files_properties(${LOCAL_SOURCES} PROPERTIES LANGUAGE CXX )

include_directories(
	${JVX_FFMPEG_INCLUDE_DIR}
    ${JVX_FFMPEG_SOURCE_DIR}/fftools
  )

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(LOCAL_LIBS ${JVX_FFMPEG_LIBS}
		${JVX_SOCKET_LIBRARIES}
		${JVX_FFMPEG_SYS_LIBS})

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

