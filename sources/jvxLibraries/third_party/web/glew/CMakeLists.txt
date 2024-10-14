set(PROJECT_NAME glew)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	
	set(targetfolder glew)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()	
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	set(libpostfix "")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
		set(libpostfix "d")
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}")

		message("Folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}> does not exist!")
		
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("opencv sub project not available. Starting script to download it.")
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("opencv installation complete.")
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

	if(JVX_RELEASE_SDK)
		#get_filename_component(tmp_nm ${PCAP_LIBRARIES} NAME)
		#set(PCAP_LIB_SDK ${tmp_nm} CACHE INTERNAL "Global values pcap lib")
	
		#get_filename_component(tmp_nm ${PCAPPA_LIBRARIES} NAME)
		#set(PCAPPA_LIB_SDK ${tmp_nm} CACHE INTERNAL "Global values pcappa lib")
		
		#install(FILES ${PCAP_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_STATIC})
		#install(FILES ${PCAPPA_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_STATIC})
	endif()

	set(GLEW_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/include CACHE INTERNAL "glew include references")
	set(GLEW_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/lib/glew32${libpostfix}.lib CACHE INTERNAL "glew include references")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/bin/ DESTINATION ${INSTALL_PATH_LIB_SHARED} FILES_MATCHING COMPONENT glew PATTERN "*.dll")
	
elseif(${JVX_OS} MATCHES "linux")
	### Install <sudo apt-get install libhdf5-dev>
	#
	#set(targetfolder glew)
	#set(archtoken glnx)
	#set(buildtype ${CMAKE_BUILD_TYPE})
	#
	#message("glew sub project not available. Starting script to download it.")
	#execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${archtoken} ${targetfolder} ${buildtype}; exit")
	#message("glew installation complete.")
	#
	#set(GLEW_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${targetfolder}-${archtoken}/include CACHE INTERNAL "glew include references")
	#set(GLEW_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${targetfolder}-${archtoken}/lib/libGLEWd.a CACHE INTERNAL "glew include references")

	### Install <sudo apt-get install libglew-dev>
	find_package(GLEW REQUIRED)
	set(GLEW_INCLUDEDIR "${GLEW_INCLUDE_DIRS}" CACHE INTERNAL "glew include references")
	set(GLEW_LIBRARIES "${GLEW_SHARED_LIBRARIES}" CACHE INTERNAL "glew include references")
endif()
# message(FATAL_ERROR "--> Stop in glew build!")
