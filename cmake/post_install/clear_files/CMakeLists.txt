	if(JVX_OS MATCHES "windows")
		
		get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY)
		find_program(mysh sh PATHS ${gitfolder}/../bin NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)
		get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY) 
	else()
		set(mysh bash)
	endif()		
		
	if(JVX_RELEASE_SDK)
	
		if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_sdk.txt)
	
			install(CODE "execute_process(COMMAND ${mysh} -c 
				\"${CMAKE_CURRENT_SOURCE_DIR}/jvx_rm_files_list.sh ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_sdk.txt ${CMAKE_INSTALL_PREFIX}; exit\")")
	
		else()
			install(CODE "message(\"--> ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_sdk.txt does not exist\")")

		endif()	
		
	else()
		
		if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_rt.txt)
			install(CODE "execute_process(COMMAND ${mysh} -c 
				\"${CMAKE_CURRENT_SOURCE_DIR}/jvx_rm_files_list.sh ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_rt.txt ${CMAKE_INSTALL_PREFIX}; exit\")")
		else()
			install(CODE "message(\"--> ${CMAKE_CURRENT_BINARY_DIR}/../../../rm_files_rt.txt does not exist\")")
		endif()
		
	endif()
	

