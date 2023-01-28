if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cminpack)
	message("cminpack sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("cminpack installation complete.")
endif()

add_subdirectory(./cminpack)
