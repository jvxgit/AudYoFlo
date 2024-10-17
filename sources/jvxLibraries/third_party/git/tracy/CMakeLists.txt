if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tracy)
	message("Tracy Profiler sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("Tracy Profiler installation complete.")
endif()

set(TRACY_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/TracyClient.cpp" CACHE INTERNAL "Tracy Profiler client source file path")
set(TRACY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public" CACHE INTERNAL "Tracy Profiler client include dirs")
