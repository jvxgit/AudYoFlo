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

		file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
	endif()

	
	set(AYF_KISSFFT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/include/kissfft" CACHE INTERNAL "kissfft include references")
	set(AYF_KISSFFT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/lib/kissfft-${dataformattoken}.lib" CACHE INTERNAL "kissfft library references")
		
	# message(FATAL_ERROR "KISSFFT: ${AYF_KISSFFT_INCLUDE_DIR} -- ${AYF_KISSFFT_LIBRARIES} -- ${AYF_KISSFFT_COMPILE_FLAGS}")
	
elseif(${JVX_OS} MATCHES "linux")


	# It seems that kissfft as a package does not work so well: there is no way to install
	# double precision package
	# if(JVX_LINUX_NATIVE_INSTALLED_LIBS)
	#	find_package(kissfft REQUIRED)
	#	
	#	message(FATAL_ERROR "-> KISSFFT -- ${OpenCV_LIBS} -- ${OpenCV_INCLUDE_DIRS}")
		
	#	set(OPENCV_LIBRARY_PATH "${KissFft_INCLUDE_DIRS}" CACHE INTERNAL "kissfft include references")
	#	set(OPENCV_LIBRARIES ${KissFft_LIBS} CACHE INTERNAL "opencv library references")
	#else()
		set(targetfolder kissfft)
	
		set(archtoken glnx)
	
		set(archtoken "${archtoken}")

		set(dataformattoken "double")
		if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
			set(dataformattoken "float")
		endif()

		set(AYF_KISSFFT_COMPILE_FLAGS "-Dkiss_fft_scalar=${dataformattoken}" CACHE INTERNAL "kissfft compile flags")
	
		set(AYF_KISSFFT_INSTALL_FOLDER "${targetfolder}-${archtoken}-${dataformattoken}-${CMAKE_BUILD_TYPE}")
		set(AYF_KISSFFT_BUILD_FOLDER "build-${archtoken}_${dataformattoken}_${CMAKE_BUILD_TYPE}")

		message("Checking folder ${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")

		      message(" --> does not exist.")
		
			# kissfft-%arch%-%dataformattoken%-%buildtype%
			if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			       message("kissfft sub project not available. Starting script to download it.") 
			       execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			       message("kissfft installation complete.")
			       endif()

			# Build the kissfft library
		  	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
				file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
			endif()

			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${archtoken} ${targetfolder} ${dataformattoken} ${CMAKE_BUILD_TYPE}")

			# message(FATAL_ERROR "file(REMOVE_RECURSE \"${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}\")")
			#file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
		endif()

		set(AYF_KISSFFT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/include/kissfft" CACHE INTERNAL "kissfft include references")
		set(AYF_KISSFFT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/lib/libkissfft-${dataformattoken}.a" CACHE INTERNAL "kissfft library references")
		
	#endif()	

elseif(${JVX_OS} MATCHES "Android")

	set(targetfolder kissfft)	
	set(archtoken android)
	
	set(archtoken "${archtoken}-${ANDROID_ABI}")
	
	set(dataformattoken "double")
	if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
		set(dataformattoken "float")
	endif()
	set(AYF_KISSFFT_COMPILE_FLAGS "-Dkiss_fft_scalar=${dataformattoken}" CACHE INTERNAL "kissfft compile flags")
	
	set(AYF_KISSFFT_INSTALL_FOLDER "${targetfolder}-${archtoken}-${dataformattoken}-${CMAKE_BUILD_TYPE}")
	set(AYF_KISSFFT_BUILD_FOLDER "build-${archtoken}_${dataformattoken}_${CMAKE_BUILD_TYPE}")
	
	message("Checking folder ${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}")
	
		message(" --> does not exist.")
		
		# kissfft-%arch%-%dataformattoken%-%buildtype%
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}")
			message("kissfft sub project not available. Starting script to download it.") 
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
			message("kissfft installation complete.")
		endif()
	
		# Build the kissfft library
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
			file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
		endif()
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-android.sh ${archtoken} ${targetfolder} ${dataformattoken} ${CMAKE_BUILD_TYPE} ${ANDROID_NDK} ${ANDROID_PLATFORM} ${ANDROID_ABI}")
		
		# message(FATAL_ERROR "file(REMOVE_RECURSE \"${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}\")")
		file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_BUILD_FOLDER}")
	endif()

	set(AYF_KISSFFT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/include/kissfft" CACHE INTERNAL "kissfft include references")
	set(AYF_KISSFFT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/${AYF_KISSFFT_INSTALL_FOLDER}/lib/libkissfft-${dataformattoken}.a" CACHE INTERNAL "kissfft library references")
		
	# message(FATAL_ERROR "KISSFFT: ${AYF_KISSFFT_INCLUDE_DIR} -- ${AYF_KISSFFT_LIBRARIES} -- ${AYF_KISSFFT_COMPILE_FLAGS}")
	
endif()

# message(FATAL_ERROR "Hier -- ")

