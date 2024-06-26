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
	
	find_package(PkgConfig)
	
	pkg_check_modules(XCB xcb REQUIRED)
	message("Lib xcb: ${XCB_LIBRARIES}")
	
	pkg_check_modules(XCBSHM xcb-shm REQUIRED)
	message("Lib xcb-shm: ${XCBSHM_LIBRARIES}")

	find_library(LIBATOMIC_LIBRARIES NAMES libatomic.so libatomic.so.1 REQUIRED)
	message("Lib atomic: ${LIBATOMIC_LIBRARIES}")
	
	# pkg_check_modules(FFTF fftw3f REQUIRED)
	# find_package(ATOMIC REQUIRED)
	# find_package(XCB::shm REQUIRED)

	pkg_check_modules(X11 x11 REQUIRED)
	message("Lib x11: ${X11_LIBRARIES}")

	pkg_check_modules(XV xv REQUIRED)
	message("Lib xv: ${XV_LIBRARIES}")

	pkg_check_modules(ALSA alsa REQUIRED)
	message("Lib alsa: ${ALSA_LIBRARIES}")

	pkg_check_modules(ZLIB zlib REQUIRED)
	message("Lib zlib: ${ZLIB_LIBRARIES}")

	pkg_check_modules(LZMA liblzma REQUIRED)
	message("Lib lzma: ${LZMA_LIBRARIES}")

	pkg_check_modules(SNDIO sndio REQUIRED)
	message("Lib sndio: ${SNDIO_LIBRARIES}")

	pkg_check_modules(XEXT xext REQUIRED)
	message("Lib xext: ${XEXT_LIBRARIES}")

	# find_package(X11 REQUIRED)
	# find_package(X11::Xv X11::Xext REQUIRED)
	# find_package(ALSA REQUIRED)
	# find_package(SoundIO REQUIRED)
	# find_package(ZLIB REQUIRED)
	# find_package(LZMA REQUIRED)
	
	# Libs as taken from ffbuild/config.mak
	set(JVX_FFMPEG_SYS_LIBS
		${JVX_FFMPEG_LIBS}
		${LIBATOMIC_LIBRARIES}
		${XCB_LIBRARIES}
		${XCBSHM_LIBRARIES}
		${ALSA_LIBRARIES}
		${SDL_LIBRARY}
		${X11_LIBRARIES}
		${XV_LIBRARIES}
		${ZLIB_LIBRARIES}
		${LZMA_LIBRARIES}	
		${SNDIO_LIBRARIES}
		${XEXT_LIBRARIES}
		
		CACHE INTERNAL "ffmpeg sys libs with strmiids (windows)")
		
	
endif()

