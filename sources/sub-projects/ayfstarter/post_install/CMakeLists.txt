project (ayfstarter)

set(PROJECT_PREFIX starter_app)
set(SDK_FOLDER $ENV{AYF_SDK_PATH})

if(JVX_ALLOW_FLUTTER_SIMPLE_INSTALLERS)
	if(EXISTS ${SDK_FOLDER}/.bv)
	
		set(checkOutput true)
		file(READ ${SDK_FOLDER}/.bv contentFile)
		if(${contentFile} MATCHES "DEBUG")
			set(tokenBuild "--debug")
		else()
			set(tokenBuild "--release")
		endif()		
		set(tokenPlatform "${JVX_PROCESSOR}")
		
		set(tokenTarget ${JVX_OS})
		if(${tokenTarget} MATCHES "Android")

			#
			# Currently, we do not really check the Android output which is typically located in 
			# build\app\outputs\flutter-apk\app-debug.apk
			#

			set(tokenTarget "apk")
			set(checkOutput false)
		endif()
		
		# message(FATAL_ERROR "${tokenTarget}")
		
		set(FINAL_OUTPUT_FOLDER "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/${PROJECT_PREFIX}")				
		install(CODE "message(\"Project ${PROJECT_PREFIX} -> running flutter build command with option <${tokenBuild}>\")")	
		install(CODE "
			message(\"Removing folder ${FINAL_OUTPUT_FOLDER}\")
			file(REMOVE_RECURSE \"${FINAL_OUTPUT_FOLDER}\")
			")		
		# if(${JVX_OS} MATCHES "windows")
		if(CMAKE_HOST_WIN32)
			# Cmake 3.29 requires to run it in msys2 - dont know why actually
			install(CODE "execute_process(COMMAND ${myshplus} -use-full-path -c \"${JVX_BASE_ROOT}/software/codeFragments/flutter/install_flutter_app.sh ${CMAKE_CURRENT_SOURCE_DIR} ${tokenTarget} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}; sleep ${JVX_FLUTTER_BUILD_SLEEP_TIME}\")")
		else()
			install(CODE "execute_process(COMMAND ${mysh} -c \"${JVX_BASE_ROOT}/software/codeFragments/flutter/install_flutter_app.sh ${CMAKE_CURRENT_SOURCE_DIR} ${tokenTarget} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild} ${tokenPlatform}; sleep ${JVX_FLUTTER_BUILD_SLEEP_TIME}\")")
		endif()	
		
			if(checkOutput)
				install(CODE "
					if(NOT EXISTS \"${FINAL_OUTPUT_FOLDER}\")
						message(FATAL_ERROR \"--> Error: folder ${FINAL_OUTPUT_FOLDER} was not created!\")
					else()
						message(\"-- done\")
					endif()
					")
			endif()
		
	else()
		if(${JVX_OS} MATCHES "windows")
	
			# pre-processing required to prevent invalid escape characters
			string(REPLACE "\\" "/" SHELL_PATH ${SDK_FOLDER})
		#	windows_to_shell_path(${SDK_FOLDER_SLASHES} SHELL_PATH)
		else()
			set(SHELL_PATH ${SDK_FOLDER})
		endif()
		install(CODE "message(\"Warning building flutter <${PROJECT_PREFIX}>: Could not find valid SDK at ${SHELL_PATH}\")")
	endif()
endif()

# message(FATAL_ERROR "Hier OUT")
## install(CODE "execute_process(COMMAND install_flutter_app.sh)")

