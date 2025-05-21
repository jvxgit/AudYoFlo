if(${JVX_OS} MATCHES "windows" OR ${JVX_OS} MATCHES "macosx")
	
	set(zlib_location "msvc2017_64")
	set(targetfolder libmysofa)
	set(archtoken_simple x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken_simple Win32)
		set(zlib_location "msvc2017_32")
	endif()
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
		
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
		message("Checking if libmysofa project is available.")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	endif()
	     		 
	# message(FATAL_ERROR "Hier -- prepareModules complete.")
		
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken_simple}/${CMAKE_BUILD_TYPE}")		
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken_simple}")
		execute_process(COMMAND cmd /c "build_win.bat ${targetfolder} ${CMAKE_BUILD_TYPE} ${archtoken_simple}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	endif()
	
	set(MYSOFA_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken_simple}/${CMAKE_BUILD_TYPE}/include" CACHE INTERNAL "Hint where mysofa library is located in this project")
    	set(MYSOFA_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken_simple}/${CMAKE_BUILD_TYPE}/lib/mysofa.lib" CACHE INTERNAL "Hint where mysofa and dependent libraries are located in this project")  	
	set(MYSOFA_ZLIB_LIBRARY_STATIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib/${zlib_location}/lib/zlib/zlib.lib" CACHE INTERNAL "Hint where mysofa finds the zlib")
	set(MYSOFA_ZLIB_LIBRARY_DYNAMIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib/${zlib_location}/lib/zlib/zlib.dll" CACHE INTERNAL "Hint where mysofa finds the zlib dll")
	
	# set(MYSOFA_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken_simple}/${CMAKE_BUILD_TYPE}" CACHE INTERNAL "Hint where mysofa library is located in this project")
else()		
	set(targetfolder libmysofa)
	set(archtoken_simple glnx)
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${archtoken_simple}; exit")
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken_simple}/${CMAKE_BUILD_TYPE}")
		# file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./build_glnx.sh ${targetfolder} ${CMAKE_BUILD_TYPE} ${archtoken_simple}"; exit)
	endif()
	
	find_package(ZLIB)
	
	set(MYSOFA_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build/${archtoken_simple}/${CMAKE_BUILD_TYPE}/include" CACHE INTERNAL "Hint where mysofa library is located in this project")
    	set(MYSOFA_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build/${archtoken_simple}/${CMAKE_BUILD_TYPE}/lib/libmysofa.a" CACHE INTERNAL "Hint where mysofa and dependent libraries are located in this project")  
    	set(MYSOFA_ZLIB_LIBRARY_STATIC ${ZLIB_LIBRARIES} CACHE INTERNAL "Hint where mysofa finds the zlib")
	# message(FATAL_ERROR "Hier 2 -- ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}_${archtoken_simple}")
	
endif()
message("libmysofa installation complete.")

# message(FATAL_ERROR "Hier 2 -- ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}_${archtoken_simple} -- ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken_simple}/${CMAKE_BUILD_TYPE}")
# add_subdirectory(./libmysofa)
