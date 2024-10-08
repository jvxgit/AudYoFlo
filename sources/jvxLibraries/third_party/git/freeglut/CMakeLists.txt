set(PROJECT_NAME freeglut)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	set(targetfolder freeglut)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()	
	
	set(archtoken "${archtoken}-${CMAKE_BUILD_TYPE}")
	set(libpostfix "")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
		set(libpostfix "d")
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}")
	
		message("Folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}> does not exist!")
		
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("${targetfolder} sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("${targetfolder} installation complete.")
		endif()
	
		# Uhrg, that was kind of a mess. Batch scripts are always pain in the ass.
		# I create dedicated build folders and run a cmake-ninja build within the build folder.
		# Then, I remove the build folders.
		# Workarounds:
		# 1) Somehow rmdir does not remove the build folder -> remove in cmake
		# 2) Need to run another cmake process in a cmake process. Therefore, I map the batch file to
		#    run secondary cmake. Fortunately, bat files inherit the environment :-)
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${archtoken} ${targetfolder} ${CMAKE_BUILD_TYPE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		# execute_process(COMMAND cmd /c "prepareModules-win.bat ${archtoken} ${targetfolder} ${dataformattoken}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		# file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
	endif()
	
	set(GLUT_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/include	CACHE INTERNAL "glut include references")
	set(GLUT_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/lib/freeglut${libpostfix}.lib	CACHE INTERNAL "glut libraries")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/bin/ DESTINATION ${INSTALL_PATH_LIB_SHARED} FILES_MATCHING COMPONENT freeglut PATTERN "*.dll")
	
elseif(${JVX_OS} MATCHES "linux")
	## Install <sudo apt-get install libhdf5-dev>
	find_package(PkgConfig)
	
	set(targetfolder freeglut)
	set(archtoken glnx)
	set(buildtype ${CMAKE_BUILD_TYPE})
	
	message("freeglut sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${archtoken} ${targetfolder} ${buildtype}; exit")
	message("freeglut installation complete.")
	
	set(GLUT_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${targetfolder}-${archtoken}" CACHE INTERNAL "freglut include references")
	set(GLUT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${targetfolder}-${archtoken}/lib/libglut.so" CACHE INTERNAL "freeglut library references")
endif()

# message(FATAL_ERROR "Hier [${PROJECT_NAME}]")

