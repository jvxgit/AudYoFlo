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

	set(JVX_FFMPEG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg" CACHE INTERNAL "ffmpeg source code path (windows)")
	
	set(JVX_FFMPEG_INCLUDE_DIR 
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg"	
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/compat/atomics/win32" CACHE INTERNAL "ffmpeg include path (windows)")
		
	set(JVX_FFMPEG_LIBS 
		${JVX_FFMPEG_LIB_AVCODEC}
		${JVX_FFMPEG_LIB_AVDEVICE}
		${JVX_FFMPEG_LIB_AVFILTER}
		${JVX_FFMPEG_LIB_AVFORMAT}
		${JVX_FFMPEG_LIB_AVUTIL}
		${JVX_FFMPEG_LIB_SWRESAMPLE}
		${JVX_FFMPEG_LIB_SWSCALE}
		CACHE INTERNAL "ffmpeg libs (windows)")
		
	set(JVX_FFMPEG_SYS_LIBS_NO_SMIIDS
		psapi.lib uuid.lib shlwapi.lib 
		vfw32.lib  secur32.lib 
		mfuuid.lib bcrypt.lib  
		# ole32.lib shell32.lib user32.lib gdi32.lib oleaut32.lib 
		CACHE INTERNAL "ffmpeg sys libs with strmiids (windows)")

	set(JVX_FFMPEG_SYS_LIB_SMIIDS strmiids.lib 
		CACHE INTERNAL "ffmpeg sys libs with strmiids (windows)")
		
	set(JVX_FFMPEG_SYS_LIBS
		${JVX_FFMPEG_SYS_LIBS_NO_SMIIDS} ${JVX_FFMPEG_SYS_LIB_SMIIDS}
		CACHE INTERNAL "ffmpeg sys libs with strmiids (windows)")
		
endif()

