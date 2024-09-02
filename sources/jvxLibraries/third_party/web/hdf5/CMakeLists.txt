set(PROJECT_NAME hdf5)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	set(targetfolder hdf5-1.14.2)
	set(archtoken_simple x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken_simple x86)
	endif()
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	set(libpostfix "")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
		set(libpostfix "_D")
	endif()
	
	set(zlib_local_folder "${CMAKE_CURRENT_SOURCE_DIR}/../../platform-packages/msvc/zlib/msvc2017_${archtoken_simple}")
	
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("hdf5 sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("hdf5 installation complete.")
		endif()
	
		# Prepare the zlib location to pass forward to bat script AND to link further down here
		cmake_path(CONVERT ${zlib_local_folder} TO_NATIVE_PATH_LIST zlib_local_folder_bat)
		
		# Uhrg, that was kind of a mess. Batch scripts are always pain in the ass.
		# I create dedicated build folders and run a cmake-ninja build within the build folder.
		# Then, I remove the build folders.
		# Workarounds:
		# 1) Somehow rmdir does not remove the build folder -> remove in cmake
		# 2) Need to run another cmake process in a cmake process. Therefore, I map the batch file to
		#    run secondary cmake. Fortunately, bat files inherit the environment :-)
		file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${archtoken} ${targetfolder} ${zlib_local_folder_bat} ${CMAKE_BUILD_TYPE} ${archtoken_simple}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
	endif()

	set(AYF_HDF5_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/include" CACHE INTERNAL "hdf5 include references")
	# set(AYF_HDF5_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/hdf5_D.lib" CACHE INTERNAL "hdf5 library references")
	set(AYF_HDF5_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/libhdf5${libpostfix}.lib;${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/libhdf5_tools${libpostfix}.lib;${zlib_local_folder}/lib/zlib/zlib.lib;shlwapi.lib" CACHE INTERNAL "hdf5 library references")
	
	# DLL linkage does not work since a few important symbols are defined only in the "static" lib part
	# set(AYF_HDF5_SHARED_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/bin/hdf5_D.dll" CACHE INTERNAL "hdf5 shared library references")
	set(AYF_HDF5_ZLIB_SHARED_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../../platform-packages/msvc/zlib/msvc2017_${archtoken_simple}/lib/zlib/zlib.dll" CACHE INTERNAL "zlib for hdf5 shared library references")

elseif(${JVX_OS} MATCHES "linux")
	## Install <sudo apt-get install libhdf5-dev>
	find_package(PkgConfig)
	
	pkg_check_modules(ZLIB zlib REQUIRED)
	#message("Lib hdf5: ${HDF5_LIBRARIES}")
	
	#set(AYF_HDF5_INCLUDE_DIR ${HDF5_INCLUDE_DIRS} CACHE INTERNAL "hdf5 include references")
	#set(AYF_HDF5_LIBRARIES ${HDF5_LIBRARIES} CACHE INTERNAL "hdf5 library references" )

	#set(AYF_HDF5_LIBRARY_DIR ${HDF5_LIBRARY_DIRS} CACHE INTERNAL "hdf5 library folder")
	## message(FATAL_ERROR "Hier ${AYF_HDF5_INCLUDE_DIR} -- ${AYF_HDF5_LIBRARIES} -- ${AYF_HDF5_LIBRARY_DIR}")
	
	set(targetfolder hdf5-1.14.2)
	set(archtoken glnx)
	
	message("hdf5 sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh; exit")
	message("hdf5 installation complete.")
	
	set(AYF_HDF5_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/include" CACHE INTERNAL "hdf5 include references")
	set(AYF_HDF5_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/libhdf5_debug.a;${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/libhdf5_tools_debug.a;${ZLIB_LIBRARIES}" CACHE INTERNAL "hdf5 library references")	
endif()

# message(FATAL_ERROR "Hier -- ")
