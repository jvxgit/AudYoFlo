if(${JVX_OS} MATCHES "windows")

	set(targetfolder orc)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()
	
	set(archtoken "${archtoken}-${CMAKE_BUILD_TYPE}")
	
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(JVX_METHOD_OPTION_BVSCRT "mdd")
	else()
		set(JVX_METHOD_OPTION_BVSCRT "md")
	endif()
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT}/lib")
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
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT}")
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT} ${archtoken} ${JVX_METHOD_OPTION_BVSCRT} ${CMAKE_BUILD_TYPE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		# file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
		
	endif()

	set(ORC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT}/include/orc-0.4" CACHE INTERNAL "orc include directory")
	set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT}/lib/liborc-0.4.a" CACHE INTERNAL "orc libraries")
	set(ORC_COMPILE_DEFINITIONS "ORC_STATIC_COMPILATION" CACHE INTERNAL "orc compile definitions")
	# set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/liborc-0.4.a" CACHE INTERNAL "orc libraries")
	# set(ORC_DLL_INSTALLER_TARGET "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/bin/orc-0.4-0.dll" CACHE INTERNAL "orc runtime library")
	set(ORCC ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/${JVX_METHOD_OPTION_BVSCRT}/bin/orcc.exe CACHE INTERNAL "orc compiler")
	
	if(JVX_RELEASE_SDK)
		set(ORCC_EXECUTABLE_SDK "orcc.exe" CACHE INTERNAL "Global values orcc")
		get_filename_component(tmp_nm ${ORC_LIBRARIES} NAME)
		set(ORCC_LIB_SDK ${tmp_nm} CACHE INTERNAL "Global values orcc liobs")
		set(ORC_COMPILE_DEFINITIONS_SDK ${ORC_COMPILE_DEFINITIONS} CACHE INTERNAL "Global values orcc flags")
		
		install(FILES ${ORCC} DESTINATION ${INSTALL_PATH_BIN})
		install(FILES ${ORC_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_STATIC})
		install(DIRECTORY orc/orc DESTINATION ${INSTALL_PATH_INCLUDE_STATIC})

	endif()
	
elseif(${JVX_OS} MATCHES "linux")

  	set(targetfolder orc)
	set(archtoken glnx)

	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")	  	
	  message("orc sub project not available. Starting script to download it.")
	  execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken} ${archtoken}; exit")
	  message("orc installation complete.")
	endif()

#		set(PROJECT_NAME orc)
#		include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)
	set(ORC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/orc/glnx/include/orc-0.4" CACHE INTERNAL "orc include directory")
	set(ORC_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/orc/glnx/lib/x86_64-linux-gnu/liborc-0.4.a" CACHE INTERNAL "orc libraries")
	set(ORCC "${CMAKE_SOURCE_DIR}/sources/jvxLibraries/third_party/git/orc/orc/glnx/bin/orcc" CACHE INTERNAL "orc compiler")
	
endif()

add_library(orcImport STATIC IMPORTED GLOBAL)
set_property(TARGET orcImport PROPERTY IMPORTED_LOCATION "${ORC_LIBRARIES}")

## message(FATAL_ERROR "Stop here!")
