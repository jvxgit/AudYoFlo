if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/eaqual)
	message("eaqual sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("equal installation complete.")
endif()


