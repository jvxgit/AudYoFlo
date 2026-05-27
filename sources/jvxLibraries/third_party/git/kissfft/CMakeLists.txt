set(PROJECT_NAME kissfft)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	set(targetfolder kissfft)
	set(archtoken x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken x86)
	endif()	
	
	set(dataformattoken "double")
	if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
		set(dataformattoken "float")
	endif()
	set(AYF_KISSFFT_COMPILE_FLAGS "-Dkiss_fft_scalar=${dataformattoken}" CACHE INTERNAL "kissfft compile flags")
	
	set(AYF_KISSFFT_INSTALL_FOLDER "${targetfolder}_${archtoken}_${dataformattoken}_${CMAKE_BUILD_TYPE}")
	set(AYF_KISSFFT_BUILD_FOLDER "build_${archtoken}_${dataformattoken}_${CMAKE_BUILD_TYPE}")
	
	message("Checking folder ${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")
	
		message(" --> does not exist.")
		
		# kissfft-%arch%-%dataformattoken%-%buildtype%
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("kissfft sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("kissfft installation complete.")
		endif()
	
		# Uhrg, that was kind of a mess. Batch scripts are always pain in the ass.
		# I create dedicated build folders and run a cmake-ninja build within the build folder.
		# Then, I remove the build folders.
		# Workarounds:
		# 1) Somehow rmdir does not remove the build folder -> remove in cmake
		# 2) Need to run another cmake process in a cmake process. Therefore, I map the batch file to
		#    run secondary cmake. Fortunately, bat files inherit the environment :-)
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
			file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
		endif()
		execute_process(COMMAND cmd /c "prepareModules-win.bat ${archtoken} ${targetfolder} ${dataformattoken} ${CMAKE_BUILD_TYPE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		## message(FATAL_ERROR "prepareModules-win.bat -> ${archtoken} -> ${targetfolder} -> ${dataformattoken} -> ${CMAKE_BUILD_TYPE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		# file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build${archtoken}")
	endif()

	
	set(AYF_KISSFFT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/include/kissfft" CACHE INTERNAL "kissfft include references")
	set(AYF_KISSFFT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/lib/kissfft-${dataformattoken}.lib" CACHE INTERNAL "kissfft library references")
		
	# message(FATAL_ERROR "KISSFFT: ${AYF_KISSFFT_INCLUDE_DIR} -- ${AYF_KISSFFT_LIBRARIES} -- ${AYF_KISSFFT_COMPILE_FLAGS}")
	
elseif(${JVX_OS} MATCHES "linux")
	## Install <sudo apt-get install libhdf5-dev>
	find_package(PkgConfig)
	
	set(targetfolder kissfft)
	set(archtoken glnx)
	
	message("kissfft sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh; exit")
	message("kissfft installation complete.")
	
	set(AYF_KISSFFT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}" CACHE INTERNAL "kissfft include references")
	set(AYF_KISSFFT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/kissfft.a" CACHE INTERNAL "kissfft library references")
endif()

# message(FATAL_ERROR "Hier -- ")

