set(archtoken x64)
if(${JVX_PLATFORM} MATCHES "32bit")
	set(archtoken x86)
endif()	
	
execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh ${archtoken} ${CMAKE_BUILD_TYPE}; exit")
set(AYF_BISON_FLEX_EXECUTABLE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/winflexbison/${CMAKE_BUILD_TYPE}/${archtoken}/bin" CACHE INTERNAL "bison lib path (windows)")
