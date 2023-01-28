if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/civetweb)
	message("civetweb sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("civetweb installation complete.")
endif()

if(NOT JVX_CIVETWEB_INCLUDED_AS_SOURCE)
	add_subdirectory(./civetweb)
endif()