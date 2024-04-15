if(${JVX_OS} MATCHES "windows")

	set(targetfolder orc)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("orc sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("orc installation complete.")
		endif()
		
		# Uhrg, that was kind of a mess. Batch scripts are always pain in the ass.
		# I create dedicated build folders and run a cmake-ninja build within the build folder.
		# Then, I remove the build folders.
		# Workarounds:
		# 1) Somehow rmdir does not remove the build folder -> remove in cmake
		# 2) Need to run another cmake process in a cmake process. Therefore, I map the batch file to
		#    run secondary cmake. Fortunately, bat files inherit the environment :-)
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		# file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
		
	endif()

	set(ORC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/include/orc-0.4" CACHE INTERNAL "orc include directory")
	set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/orc-0.4.lib" CACHE INTERNAL "orc libraries")
	# set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/liborc-0.4.a" CACHE INTERNAL "orc libraries")
	set(ORC_DLL_INSTALLER_TARGET "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/bin/orc-0.4-0.dll" CACHE INTERNAL "orc runtime library")
	set(ORCC ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/bin/orcc.exe CACHE INTERNAL "orc compiler")
	
elseif(${JVX_OS} MATCHES "linux")

  	set(targetfolder orc)
	set(archtoken glnx)

	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")	  	
	  message("orc sub project not available. Starting script to download it.")
	  execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}; exit")
	  message("orc installation complete.")
	endif()

#		set(PROJECT_NAME orc)
#		include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)
	set(ORC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/orc/glnx/include/orc-0.4" CACHE INTERNAL "orc include directory")
	set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/orc/glnx/lib/x86_64-linux-gnu/liborc-0.4.a" CACHE INTERNAL "orc libraries")
	set(ORCC "${CMAKE_SOURCE_DIR}/sources/jvxLibraries/third_party/git/orc/orc/glnx/bin/orcc" CACHE INTERNAL "orc compiler")
endif()

## message(FATAL_ERROR "Hier")

