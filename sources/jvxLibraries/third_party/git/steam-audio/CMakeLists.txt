	if(${JVX_OS} MATCHES "windows" OR ${JVX_OS} MATCHES "macosx")

		set(steam_folder_prefix windows-x64)
		set(archtoken_simple x64)
		if(${JVX_PLATFORM} MATCHES "32bit")
			set(steam_folder_prefix windows-x86)
			set(archtoken_simple Win32)
		endif()
	
		set(archtoken "${archtoken_simple}-${CMAKE_BUILD_TYPE}")
			
		set(targetfolder steam-audio-${archtoken})
		# set(targetfolder steam-audio-)
		
		if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder})
			message("steam-audio sub project not available. Starting script to download it.") 
	
			# execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${targetfolder} ${archtoken} ${CMAKE_BUILD_TYPE}; sleep 10; exit")
			execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh ${targetfolder} ${archtoken} ${CMAKE_BUILD_TYPE}; exit")
		endif()
	
		set(STEAMAUDIO_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/core/bin/include" CACHE INTERNAL "Hint where steam audio library is located in this project")
		set(STEAMAUDIO_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/core/bin/lib/${steam_folder_prefix}/phonon.lib" CACHE INTERNAL "Hint where phonon library is located in this project")  	
		set(STEAMAUDIO_DYNAMIC "${CMAKE_CURRENT_SOURCE_DIR}/${targetfolder}/core/bin/lib/${steam_folder_prefix}/phonon.dll" CACHE INTERNAL "Hint where phonon runtime library is located in this project")  

		message("steam-audio installation complete.")
		message("STEAMAUDIO_INCLUDES = ${STEAMAUDIO_INCLUDES}.")
		message("STEAMAUDIO_LIBRARY = ${STEAMAUDIO_LIBRARY}.")
		message("STEAMAUDIO_SHARED = ${STEAMAUDIO_SHARED}.")
		
		# message(FATAL_ERROR "Steam Audio stop here")
		
	else()
		message(FATAL_ERROR "Steam Audio not supported in Linux up to now")
		execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-glnx.sh; exit")
	endif()
	

