set(PROJECT_NAME flutter)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/flutter)
	message("flutter sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	message("flutter installation complete.")
endif()

