set(PROJECT_NAME hdf5)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	set(targetfolder hdf5-1.14.2)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("hdf5 sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("hdf5 installation complete.")
		endif()
	
		# Uhrg, that was kind of a mess. Batch scripts are always pain in the ass.
		# I create dedicated build folders and run a cmake-ninja build within the build folder.
		# Then, I remove the build folders.
		# Workarounds:
		# 1) Somehow rmdir does not remove the build folder -> remove in cmake
		# 2) Need to run another cmake process in a cmake process. Therefore, I map the batch file to
		#    run secondary cmake. Fortunately, bat files inherit the environment :-)
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${archtoken} ${targetfolder}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
	endif()

	set(AYF_HDF5_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/include" CACHE INTERNAL "hdf5 include references")
	set(AYF_HDF5_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/hdf5_D.lib" CACHE INTERNAL "hdf5 library references")
	set(AYF_HDF5_SHARED_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/bin/hdf5_D.dll" CACHE INTERNAL "hdf5 shared library references")
	set(AYF_HDF5_ZLIB_SHARED_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../../platform-packages/msvc/zlib/msvc2017_${archtoken}/lib/zlib/zlib.dll" CACHE INTERNAL "zlib for hdf5 shared library references")
endif()

## message(FATAL_ERROR "Hier -- ${ZLIB_DIR}")