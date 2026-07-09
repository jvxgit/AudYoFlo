if(CMAKE_HOST_WIN32)
	set(archtoken host)

	if(JVX_COMPILE_BUILDTOOLS)

		# We should only build this within the step for build tools - build tools always native!!
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh ${archtoken} ${CMAKE_BUILD_TYPE}; exit")
	endif()

	# We ALWAYS need to set the reference to the executable to run also crosscompile processes
	set(AYF_BISON_FLEX_EXECUTABLE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/winflexbison/${CMAKE_BUILD_TYPE}/${archtoken}/bin" CACHE INTERNAL "bison lib path (windows)")
	
endif()

# message(FATAL_ERROR "--> BISON: ${AYF_BISON_FLEX_EXECUTABLE_PATH} - ${CMAKE_HOST_WIN32}")
