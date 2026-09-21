set(PROJECT_NAME fftw-build)
# include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(CMAKE_HOST_WIN32)

	FIND_PROGRAM(myshplus "msys2_shell.cmd" REQUIRED)
	set(targetfolder fftw-3.3.11)
	set(archtoken_simple ${JVX_PROCESSOR})

	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}-${JVX_OS}-${JVX_OS_FLAVOUR}")
	set(JVX_LIB_FILE_EXTENSION "a")
	if(${JVX_OS} MATCHES "android")
		set(JVX_LIB_FILE_EXTENSION "a")
	endif()

	#set(libpostfix "")
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#	set(libpostfix "_D")
	#endif()

	# Check for the libraries rather than the build folder: an aborted build leaves the folder behind
	set(fftw_libdir "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib")
	set(fftw_libf "${fftw_libdir}/libfftw3f.${JVX_LIB_FILE_EXTENSION}")
	set(fftw_libd "${fftw_libdir}/libfftw3.${JVX_LIB_FILE_EXTENSION}")
	message("Trying to build fftw, look for libraries in ${fftw_libdir}")
	if(NOT EXISTS "${fftw_libf}" OR NOT EXISTS "${fftw_libd}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")

			message("fftw-3.3 sub project not available. Starting script to download it.")
			# execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
			execute_process(COMMAND ${myshplus} -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh ${targetfolder} ; sleep 10")
			# The exit code of msys2_shell.cmd is not reliable, so check for the result instead
			if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
				message(FATAL_ERROR "Downloading fftw failed, folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}> missing.")
			endif()
			message("fftw-3.3 installation complete.")
			## ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}
		endif()

		# This builds fftw and fftw3f. ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY may contain build information for cross compilation
		if(${JVX_OS} MATCHES "android")
			message("Android Target, building fftw in <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}>")
			message("compile_android_win.bat ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}")
			execute_process(COMMAND cmd /c "compile_android_win.bat ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
				RESULT_VARIABLE fftw_result)

		else() # DSpace target!!
			message("DSpace Target, building fftw in <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}>")

			set(TOOLCHAINFILE ${CMAKE_TOOLCHAIN_FILE})
			# message(FATAL_ERROR "Hier FFTW -${TOOLCHAINFILE}- ")

			# This builds fftw and fftw3f. ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY may contain build information for cross compilation
			message("compile_dspace_win.bat ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${TOOLCHAINFILE}")
			execute_process(COMMAND cmd /c "compile_dspace_win.bat ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${TOOLCHAINFILE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
				RESULT_VARIABLE fftw_result)
			#if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${archtoken}-Release")
			#	execute_process(COMMAND cmd /c "compile_dspace_win.bat ${archtoken} Release ${targetfolder} ${TOOLCHAINFILE}.cmake" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
			#endif()

		endif()

		# The scripts' exit codes only reflect their last step, so also check that both libraries were produced
		if(NOT fftw_result EQUAL 0 OR NOT EXISTS "${fftw_libf}" OR NOT EXISTS "${fftw_libd}")
			message(FATAL_ERROR "Building fftw failed (exit code ${fftw_result}), libraries missing in <${fftw_libdir}>. "
				"Remove <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}> and check the output above.")
		endif()
	else()
		message("fftw libraries found in <${fftw_libdir}>.")
	endif()

	set(FFT_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/include" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBDIR "${fftw_libdir}" CACHE INTERNAL "fftw lib (android)")
	set(FFT_COMPILE_DEFINITIONS "" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYF "${fftw_libf}" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARYD "${fftw_libd}" CACHE INTERNAL "fftw lib (android)")
	set(FFT_LIBRARIES "${FFT_LIBRARYF};${FFT_LIBRARYD}" CACHE INTERNAL "fftw lib (android)")
	
	message("     include path: ${FFT_INCLUDEDIR}")
	message("     lib path: ${FFT_LIBDIR}")
	message("     compile definitions: ${FFT_COMPILE_DEFINITIONS}")
	message("     lib float: ${FFT_LIBRARYF}")
	message("     lib double: ${FFT_LIBRARYD}")
	message("     lib: ${FFT_LIBRARIES}")

	# message(FATAL_ERROR "Hier FFTW 2 ")	
	
	if(NOT JVX_COMPILE_BUILDTOOLS)
		# We install the fftw libs in the runtime dll path
		install(FILES ${FFT_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_SHARED})
		## message(FATAL_ERROR "Hier -- ")
	endif()
	
else()	

set(targetfolder fftw-3.3.11)

	set(archtoken_simple ${JVX_PROCESSOR})
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	#set(libpostfix "")
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
	#	set(libpostfix "_D")
	#endif()	
	
	# Check for the libraries rather than the build folder: an aborted build leaves the folder behind
	set(fftw_libdir "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/lib")
	message("Trying to build fftw")
	if(NOT EXISTS "${fftw_libdir}/libfftw3f.so" OR NOT EXISTS "${fftw_libdir}/libfftw3.so")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("fftw-3.3 sub project not available. Starting script to download it.")
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh ${targetfolder}; exit"
				RESULT_VARIABLE fftw_result)
			if(NOT fftw_result EQUAL 0 OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
				message(FATAL_ERROR "Downloading fftw failed (exit code ${fftw_result}), folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}> missing.")
			endif()
			message("fftw-3.3 installation complete.")
			## ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}
		endif()

		# This builds fftw and fftw3f. ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY may contain build information for cross compilation
		message("./compile_android_glnx.sh ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR}/build; ./compile_android_glnx.sh ${archtoken} ${CMAKE_BUILD_TYPE} ${targetfolder} ${ADDITIONAL_FEATURE_TOKENS_CMAKE_THIRD_PARTY}; exit"
			RESULT_VARIABLE fftw_result)
		# The script's exit code only reflects its last step, so also check that both libraries were produced
		if(NOT fftw_result EQUAL 0 OR NOT EXISTS "${fftw_libdir}/libfftw3f.so" OR NOT EXISTS "${fftw_libdir}/libfftw3.so")
			message(FATAL_ERROR "Building fftw failed (exit code ${fftw_result}), libraries missing in <${fftw_libdir}>. "
				"Remove <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}> and check the output above.")
		endif()
	else()
		message("fftw libraries found in <${fftw_libdir}>.")
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
	
	if(NOT JVX_COMPILE_BUILDTOOLS)
		# We install the fftw libs in the runtime dll path
		install(FILES ${FFT_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_SHARED})
		# message(FATAL_ERROR "Hier -- ")
	endif()
	

endif()

# message(FATAL_ERROR "Hier FFTW -- ")
 
