set(PROJECT_NAME fftw-build)
# include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(CMAKE_HOST_WIN32)

	FIND_PROGRAM(myshplus "msys2_shell.cmd" REQUIRED)
	set(targetfolder fftw-3.3.10)
	set(archtoken_simple ${JVX_PROCESSOR})
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	#set(libpostfix "")
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
	#	set(libpostfix "_D")
	#endif()	
	
	message("Trying to build fftw, look for folder ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("fftw-3.3 sub project not available. Starting script to download it.") 
			# execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
			execute_process(COMMAND ${myshplus} -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh ${targetfolder} ; sleep 10")
			message("fftw-3.3 installation complete.")
			## ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}
		endif()
		
		# This builds fftw and fftw3f. ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY may contain build information for cross compilation
		execute_process(COMMAND cmd /c "compile_android_win.bat ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)		
	else()
		message("Folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}> already exists.")
	endif()

	set(FFT_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/include" CACHE INTERNAL "fftw lib (android)")	
	set(FFT_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib" CACHE INTERNAL "fftw lib (android)")
	set(FFT_COMPILE_DEFINITIONS "" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYF "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib/libfftw3f.so" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYD "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib/libfftw3.so" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARIES "${FFT_LIBRARYF};${FFT_LIBRARYD}" CACHE INTERNAL "fftw lib (android)")
	
	message("     include path: ${FFT_INCLUDEDIR}")
	message("     lib path: ${FFT_LIBDIR}")
	message("     compile definitions: ${FFT_COMPILE_DEFINITIONS}")
	message("     lib float: ${FFT_LIBRARYF}")
	message("     lib double: ${FFT_LIBRARYD}")
	message("     lib: ${FFT_LIBRARIES}")
	
	# We install the fftw libs in the runtime dll path
	install(FILES ${FFT_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_SHARED})
	## message(FATAL_ERROR "Hier -- ")
else()	

set(targetfolder fftw-3.3.10)

	set(archtoken_simple ${JVX_PROCESSOR})
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	#set(libpostfix "")
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
	#	set(libpostfix "_D")
	#endif()	
	
	message("Trying to build fftw")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("fftw-3.3 sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
			message("fftw-3.3 installation complete.")
			## ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}
		endif()
		
		# This builds fftw and fftw3f. ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY may contain build information for cross compilation
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR}/build; ./compile_android_glnx.sh ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}; exit")
		message("./compile_android_glnx.sh ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}")
	else()
		message("Folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}> already exists.")
	endif()

	set(FFT_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/include" CACHE INTERNAL "fftw lib (android)")	
	set(FFT_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib" CACHE INTERNAL "fftw lib (android)")
	set(FFT_COMPILE_DEFINITIONS "" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYF "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib/libfftw3f.so" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYD "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib/libfftw3.so" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARIES "${FFT_LIBRARYF};${FFT_LIBRARYD}" CACHE INTERNAL "fftw lib (android)")
	
	message("     include path: ${FFT_INCLUDEDIR}")
	message("     lib path: ${FFT_LIBDIR}")
	message("     compile definitions: ${FFT_COMPILE_DEFINITIONS}")
	message("     lib float: ${FFT_LIBRARYF}")
	message("     lib double: ${FFT_LIBRARYD}")
	message("     lib: ${FFT_LIBRARIES}")
	
	# We install the fftw libs in the runtime dll path
	install(FILES ${FFT_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_SHARED})
	# message(FATAL_ERROR "Hier -- ")
	
endif()

 
