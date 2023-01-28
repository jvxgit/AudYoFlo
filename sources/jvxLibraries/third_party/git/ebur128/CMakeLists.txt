if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libebur128)
	message("libebur128 sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("libebur128 installation complete.")
endif()

add_subdirectory(./libebur128)