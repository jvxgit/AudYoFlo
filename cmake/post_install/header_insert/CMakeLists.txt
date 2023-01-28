if(JVX_OS MATCHES "windows")
	get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY)
	find_program(mysh sh PATHS ${gitfolder}/../bin NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)
else()
  # Some of the syntax constructs in the used header files only allow the bash script. By default, cmake activates sh which yields a syntax error
  set(mysh bash)
endif()

if(JVX_OS MATCHES "windows")
  install(CODE "execute_process(COMMAND ${mysh} -c 
	\"${CMAKE_CURRENT_SOURCE_DIR}/jvx_add_header_all.sh ${CMAKE_CURRENT_BINARY_DIR}/../../../release/sdk/ ${CMAKE_CURRENT_SOURCE_DIR}/inserted_header.txt; exit\")")
else()
  install(CODE "execute_process(COMMAND  ${mysh} -c 
	\"${CMAKE_CURRENT_SOURCE_DIR}/jvx_add_header_all.sh ${CMAKE_CURRENT_BINARY_DIR}/../../../release/sdk/ ${CMAKE_CURRENT_SOURCE_DIR}/inserted_header.txt; exit\")")
endif()		

