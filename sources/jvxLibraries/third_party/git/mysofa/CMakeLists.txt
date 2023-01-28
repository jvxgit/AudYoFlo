if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libmysofa)
	message("libmysofa sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("libmysofa installation complete.")
endif()

add_subdirectory(./libmysofa)