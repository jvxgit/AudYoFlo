set(PROJECT_NAME boost)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boost)
	message("boost sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	message("boost installation complete.")
endif()

set(BOOST_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/boost")
