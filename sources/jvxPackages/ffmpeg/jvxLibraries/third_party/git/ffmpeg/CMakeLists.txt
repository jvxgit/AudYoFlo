message("--> configuring ffmpeg")

if(${JVX_OS} MATCHES "windows")
	
	if(JVX_PLATFORM MATCHES "32bit")
		set(JVX_FFMPEG_POSTFIX "win32")
	else()
		set(JVX_FFMPEG_POSTFIX "x64")
	endif()
	
	set(JVX_FFMPEG_POSTFIX "${JVX_FFMPEG_POSTFIX}-${CMAKE_BUILD_TYPE}")
	
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
		set(JVX_COMPILE_EXTRA_CFLAGS "-MDd")
	else()
		set(JVX_COMPILE_EXTRA_CFLAGS "-MD")
	endif()
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib)

		FIND_PROGRAM(msys2 "msys2_shell.cmd" REQUIRED)
		message("ffmpeg sub project not available. Starting script to download it.") 
		message("This is going to take a while..") 
		message("${msys2} -use-full-path -c cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${JVX_FFMPEG_POSTFIX} ${CMAKE_BUILD_TYPE} ${JVX_COMPILE_EXTRA_CFLAGS}") 
		execute_process(COMMAND ${msys2} -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${JVX_FFMPEG_POSTFIX} ${CMAKE_BUILD_TYPE} ${JVX_COMPILE_EXTRA_CFLAGS}")
		
		if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib)
			message(FATAL_ERROR "ffmpeg installation failed.")
		else()
			message("ffmpeg installation complete.")
		endif()
		
	endif()

	## message(FATAL_ERROR "Wait here!")
	
	set(JVX_FFMPEG_LIB_AVCODEC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavcodec.a")
	set(JVX_FFMPEG_LIB_AVDEVICE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavdevice.a")
	set(JVX_FFMPEG_LIB_AVFILTER "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavfilter.a")
	set(JVX_FFMPEG_LIB_AVFORMAT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavformat.a")
	set(JVX_FFMPEG_LIB_AVUTIL "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavutil.a")
	set(JVX_FFMPEG_LIB_SWRESAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswresample.a")
	set(JVX_FFMPEG_LIB_SWSCALE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswscale.a")
	set(JVX_FFMPEG_LIB_POSTPROC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libpostproc.a")

	set(JVX_LIB_X264 "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/x264/install-${CMAKE_BUILD_TYPE}/lib/libx264.lib")
	set(JVX_LIB_X265 "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/x265_git/build/msys-cl/install-${CMAKE_BUILD_TYPE}/lib/x265-static.lib")
	# set(JVX_LIB_X265 "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/x265_git/build/msys-cl/install/lib/x265-static.lib")
	
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
		${JVX_FFMPEG_LIB_POSTPROC}
		${JVX_LIB_X264}
		${JVX_LIB_X265}
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

	find_package(PkgConfig)

	if(JVX_LINUX_NATIVE_INSTALLED_LIBS)

		# This will also include x264 and x265, I assume
		# sudo apt-get install libavdevice-dev
		# sudo apt-get install libavcodec-dev
		# sudo apt-get install ffmpeg

		pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
	    		libavdevice
	    		libavfilter
    	    		libavformat
    	    		libavcodec
    	    		libswresample
    	    		libswscale
    	    		libavutil
			libpostproc
	    		)
		pkg_check_modules(X264 REQUIRED IMPORTED_TARGET x264)
		pkg_check_modules(X265 REQUIRED IMPORTED_TARGET x265)

		set(JVX_FFMPEG_INCLUDE_DIR "${LIBAV_INCLUDE_DIRS} ${X264_INCLUDE_DIRS} ${X265_INCLUDE_DIRS}" CACHE INTERNAL "ffmpeg include path (windows)")
		set(JVX_FFMPEG_LIBS "${LIBAV_LIBRARIES};${X264_LIBRARIES};${X265_LIBRARIES}" CACHE INTERNAL "ffmpeg libs (glnx)")	
		# message(FATAL_ERROR "Hier FFMPEG:: ${LIBAV_LIBRARIES} ${X264_LIBRARIES} ${X265_LIBRARIES} -- ${LIBAV_INCLUDE_DIRS} ${X264_INCLUDE_DIRS} ${X265_INCLUDE_DIRS}")

	else()
		set(JVX_FFMPEG_POSTFIX "glnx")
		
		message("Checking for pre-compiled ffmpeg library in folder <${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}>.")
		if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX})

		       message("ffmpeg sub project not available. Starting script to download it.") 
		       message("This is going to take a while..") 
		       message("${mysh} -c cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${JVX_FFMPEG_POSTFIX}") 
		       execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${JVX_FFMPEG_POSTFIX}")
		       message("ffmpeg installation complete.")
		     
		else()
		      message("---> Already there.")
		endif()

		## message(FATAL_ERROR "Wait here!")
	
		set(JVX_FFMPEG_LIB_AVCODEC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavcodec.a")
		set(JVX_FFMPEG_LIB_AVDEVICE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavdevice.a")
		set(JVX_FFMPEG_LIB_AVFILTER "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavfilter.a")
		set(JVX_FFMPEG_LIB_AVFORMAT "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavformat.a")
		set(JVX_FFMPEG_LIB_AVUTIL "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libavutil.a")
		set(JVX_FFMPEG_LIB_SWRESAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswresample.a")
		set(JVX_FFMPEG_LIB_SWSCALE "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libswscale.a")
		set(JVX_FFMPEG_LIB_POSTPROC "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${JVX_FFMPEG_POSTFIX}/lib/libpostproc.a")

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
			${JVX_FFMPEG_LIB_POSTPROC}
			x264
			x265
			CACHE INTERNAL "ffmpeg libs (glnx)")	

	endif()		
	
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

	#pkg_check_modules(SNDIO sndio REQUIRED)
	#message("Lib sndio: ${SNDIO_LIBRARIES}")

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
		
		CACHE INTERNAL "ffmpeg sys libs with strmiids (linux)")	
	
endif()

# message(FATAL_ERROR "Debug CMAKE stop in project ffmpeg!!")
