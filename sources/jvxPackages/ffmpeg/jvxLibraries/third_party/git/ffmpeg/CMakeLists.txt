message("--> configuring ffmpeg")

if(${JVX_OS} MATCHES "windows")
	
	if(JVX_PLATFORM MATCHES "32bit")
		set(JVX_FFMPEG_POSTFIX "win32")
	else()
		set(JVX_FFMPEG_POSTFIX "x64")
	endif()
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX})

		FIND_PROGRAM(msys2 "msys2_shell.cmd" REQUIRED)
		message("ffmpeg sub project not available. Starting script to download it.") 
		message("This is going to take a while..") 
		message("${msys2} -use-full-path -c cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${JVX_FFMPEG_POSTFIX}") 
		execute_process(COMMAND ${msys2} -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${JVX_FFMPEG_POSTFIX}")
		message("ffmpeg installation complete.")
	endif()

	## message(FATAL_ERROR "Wait here!")
	
	set(JVX_FFMPEG_LIB_AVCODEC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavcodec.a")
	set(JVX_FFMPEG_LIB_AVDEVICE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavdevice.a")
	set(JVX_FFMPEG_LIB_AVFILTER "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavfilter.a")
	set(JVX_FFMPEG_LIB_AVFORMAT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavformat.a")
	set(JVX_FFMPEG_LIB_AVUTIL "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavutil.a")
	set(JVX_FFMPEG_LIB_SWRESAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswresample.a")
	set(JVX_FFMPEG_LIB_SWSCALE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswscale.a")

	set(JVX_FFMPEG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}" CACHE INTERNAL "ffmpeg source code path (windows)")
	
	set(JVX_FFMPEG_INCLUDE_DIR 
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}"	
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/compat/atomics/win32" CACHE INTERNAL "ffmpeg include path (windows)")
		
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

elseif(${JVX_OS} MATCHES "linux")
	
	set(JVX_FFMPEG_POSTFIX "glnx")
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX})

		message("ffmpeg sub project not available. Starting script to download it.") 
		message("This is going to take a while..") 
		message("${mysh} -c cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${JVX_FFMPEG_POSTFIX}") 
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${JVX_FFMPEG_POSTFIX}")
		message("ffmpeg installation complete.")
	endif()

	## message(FATAL_ERROR "Wait here!")
	
	set(JVX_FFMPEG_LIB_AVCODEC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavcodec.a")
	set(JVX_FFMPEG_LIB_AVDEVICE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavdevice.a")
	set(JVX_FFMPEG_LIB_AVFILTER "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavfilter.a")
	set(JVX_FFMPEG_LIB_AVFORMAT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavformat.a")
	set(JVX_FFMPEG_LIB_AVUTIL "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavutil.a")
	set(JVX_FFMPEG_LIB_SWRESAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswresample.a")
	set(JVX_FFMPEG_LIB_SWSCALE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswscale.a")

	set(JVX_FFMPEG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}" CACHE INTERNAL "ffmpeg source code path (windows)")
	
	set(JVX_FFMPEG_INCLUDE_DIR 
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}"	
		"${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/include" CACHE INTERNAL "ffmpeg include path (windows)")
		
	set(JVX_FFMPEG_LIBS 
		${JVX_FFMPEG_LIB_AVCODEC}
		${JVX_FFMPEG_LIB_AVDEVICE}
		${JVX_FFMPEG_LIB_AVFILTER}
		${JVX_FFMPEG_LIB_AVFORMAT}
		${JVX_FFMPEG_LIB_AVUTIL}
		${JVX_FFMPEG_LIB_SWRESAMPLE}
		${JVX_FFMPEG_LIB_SWSCALE}
		CACHE INTERNAL "ffmpeg libs (glnx)")	
	
	# Libs as taken from ffbuild/config.mak
	set(JVX_FFMPEG_SYS_LIBS
		${JVX_FFMPEG_LIBS}
		atomic
		xcb
		xcb-shm
		asound
		SDL2
		sndio
		Xv
		X11
		Xext
		z
		lzma
		X11
		CACHE INTERNAL "ffmpeg sys libs with strmiids (windows)")

endif()

