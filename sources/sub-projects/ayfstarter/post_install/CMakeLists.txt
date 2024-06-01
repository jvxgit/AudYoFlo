project (ayfstarter)

set(PROJECT_PREFIX starter_app)
set(SDK_FOLDER $ENV{AYF_SDK_PATH})

if(JVX_ALLOW_FLUTTER_SIMPLE_INSTALLERS)
	if(EXISTS ${SDK_FOLDER}/.bv)
		file(READ ${SDK_FOLDER}/.bv contentFile)
		if(${contentFile} MATCHES "DEBUG")
			set(tokenBuild "--debug")
		else()
			set(tokenBuild "--release")
		endif()
		message("Runing flutter build command with option <${tokenBuild}>")
		install(CODE "execute_process(COMMAND ${mysh} -c 
					\"${CMAKE_CURRENT_SOURCE_DIR}/install_flutter_app.sh ${CMAKE_CURRENT_SOURCE_DIR} ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example ${tokenBuild}; exit\")")
	else()
		if(${JVX_OS} MATCHES "windows")
			windows_to_shell_path(${AYF_AWE_SHARED_LIBRARY_TARGET_FOLDER} SHELL_PATH)
		else()
			set(SHELL_PATH ${AYF_AWE_SHARED_LIBRARY_TARGET_FOLDER})
		endif()
		install(CODE "message(\"Warning building flutter <${PROJECT_PREFIX}>: Could not find valid SDK at ${SHELL_PATH}\")")
	endif()
endif()

## install(CODE "execute_process(COMMAND install_flutter_app.sh)")

