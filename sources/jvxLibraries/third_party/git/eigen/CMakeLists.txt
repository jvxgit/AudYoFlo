cmake_minimum_required (VERSION 2.8)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/eigen)
	message("eigen sub project not available. Starting script to download it.") 
	if(${JVX_OS} MATCHES "windows")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	elseif(${JVX_OS} MATCHES "macosx")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	else()
		message(FATAL_ERROR "No download script for current option")
	endif()

	message("eigen installation complete.")
endif()

set(EIGEN_LIBRARY_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/eigen" CACHE INTERNAL "eigen lib path (windows)")
