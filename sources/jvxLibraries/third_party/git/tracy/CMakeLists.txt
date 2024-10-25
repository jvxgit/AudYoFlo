set(PROJECT_NAME tracy)

# We cannot use common_local_lib_{head,foot}.cmake here because it leads to cyclic dependencies
# as Tracy is used globally, in every lib and every binary. This is the only way to be able
# to profile all of the code.

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tracy)
	message("Tracy Profiler sub project not available. Starting script to download it.")
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("Tracy Profiler installation complete.")
endif()

set(TRACY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/" CACHE INTERNAL "Tracy Profiler client include dirs")

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/TracyClient.cpp)
set(LOCAL_COMPILE_DEFINITIONS "TRACY_ENABLE")

if(JVX_USE_TRACY)
	add_library(tracy SHARED "${LOCAL_SOURCES}")
	target_link_libraries(tracy PUBLIC pthread dl)
	target_compile_definitions(tracy PUBLIC "${LOCAL_COMPILE_DEFINITIONS}")
	target_include_directories(tracy PUBLIC ${TRACY_INCLUDE_DIRS})
else()
	add_library(tracy INTERFACE)
	target_include_directories(tracy INTERFACE ${TRACY_INCLUDE_DIRS})
endif()

if(JVX_USE_TRACY AND JVX_SHARED_LIB_TARGET_FOLDER)
	install(TARGETS tracy
		RUNTIME DESTINATION ${JVX_SHARED_LIB_TARGET_FOLDER}
		LIBRARY DESTINATION ${INSTALL_PATH_LIB_SHARED})
else()
	install(TARGETS tracy
		RUNTIME DESTINATION ${INSTALL_PATH_LIB_SHARED}
		LIBRARY DESTINATION ${INSTALL_PATH_LIB_SHARED})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tracy/public/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/git/tracy/tracy)
