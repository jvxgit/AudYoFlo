cmake_minimum_required (VERSION 2.8)

message("--> configuring ffmpeg")

if(${JVX_OS} MATCHES "windows")
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg)

		FIND_PROGRAM(msys2 "msys2_shell.cmd" REQUIRED)
		message("ffmpeg sub project not available. Starting script to download it.") 
		message("This is going to take a while..") 
		message("${msys2} -use-full-path -c cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh") 
		execute_process(COMMAND ${msys2} -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh")
		message("ffmpeg installation complete.")
	endif()

	set(JVX_FFMPEG_LIB_AVCODEC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libavcodec.a")
	set(JVX_FFMPEG_LIB_AVDEVICE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libavdevice.a")
	set(JVX_FFMPEG_LIB_AVFILTER "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libavfilter.a")
	set(JVX_FFMPEG_LIB_AVFORMAT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libavformat.a")
	set(JVX_FFMPEG_LIB_AVUTIL "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libavutil.a")
	set(JVX_FFMPEG_LIB_SWRESAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libswresample.a")
	set(JVX_FFMPEG_LIB_SWSCALE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/lib/libswscale.a")

	set(JVX_FFMPEG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg"
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/compat/atomics/win32")
endif()

set(JVX_FFMPEG_TEST_APPS
	${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-app
	${CMAKE_CURRENT_SOURCE_DIR}/ffplay-app
  )

jvx_addSubdirs("Configuring ffmpeg test apps" "${JVX_FFMPEG_TEST_APPS}" "${JVX_FOLDER_HIERARCHIE_BASE}/ffmpeg")
