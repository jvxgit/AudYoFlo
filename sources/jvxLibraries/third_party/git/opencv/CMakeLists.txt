set(PROJECT_NAME opencv)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(${JVX_OS} MATCHES "windows")
	set(targetfolder opencv)
	set(archtoken_simple x64)
	if(${JVX_PLATFORM} MATCHES "32bit")
		set(archtoken_simple x86)
	endif()	
	
	set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
	
	set(libpostfix "")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")		
		set(libpostfix "d")
	endif()
	
	set(vs_version "vc17")
	if(MSVC_TOOLSET_VERSION LESS 143)
		set(vs_version "vc16")
	elseif(MSVC_TOOLSET_VERSION GREATER_EQUAL 142)	
		set(vs_version "vc16")
	else()
		message(FATAL_ERROR "Unsupported option for openCV library folder!")
	endif()
	
	#set(dataformattoken "double")
	#set(AYF_KISSFFT_COMPILE_FLAGS "kiss_fft_scalar=double")
	#if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
	#	set(dataformattoken "float")
	#	set(AYF_KISSFFT_COMPILE_FLAGS "kiss_fft_scalar=float")
	#endif()
	
	# if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/opencv")
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}")
	
		message("Folder <${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}> does not exist!")
		
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

	set(OPENCV_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/include" CACHE INTERNAL "opencv include references")
	set(OPENCV_LIBRARIES 
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/${archtoken_simple}/vc17/staticlib/opencv_imgproc4100${libpostfix}.lib
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/${archtoken_simple}/vc17/staticlib/opencv_core4100${libpostfix}.lib
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/${archtoken_simple}/vc17/staticlib/ippicvmt.lib
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/${archtoken_simple}/vc17/staticlib/ippiw${libpostfix}.lib
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/opencv-${archtoken}/${archtoken_simple}/vc17/staticlib/ittnotify${libpostfix}.lib
		CACHE INTERNAL "opencv library references")	
		
#		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/${archtoken}/vc17/staticlib/opencv_highgui4100d.lib
#		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/${archtoken}/vc17/staticlib/opencv_video4100d.lib
#		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/build_${archtoken}/${targetfolder}/${archtoken}/vc17/staticlib/opencv_videoio4100d.lib		
		
	
elseif(${JVX_OS} MATCHES "linux")
	## Install <sudo apt-get install libhdf5-dev>
	find_package(PkgConfig)
	
	set(targetfolder opencv)
	set(archtoken glnx)
	set(buildtype ${CMAKE_BUILD_TYPE})
	
	message("opencv sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh ${archtoken} ${targetfolder} ${buildtype}; exit")
	message("opencv installation complete.")
	
	set(OPENCV_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/include" CACHE INTERNAL "opencv include references")
	set(OPENCV_LIBRARIES 
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_core
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_features2d
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_gapi
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_imgcodecs
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_ml
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_photo
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_video
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_calib3d
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_dnn
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_flann
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_highgui
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_imgproc
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_objdetect
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_stitching
		${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/${archtoken}/lib/opencv_videoio
		CACHE INTERNAL "opencv library references")
		
endif()

# message(FATAL_ERROR "Hier [${PROJECT_NAME}]")

