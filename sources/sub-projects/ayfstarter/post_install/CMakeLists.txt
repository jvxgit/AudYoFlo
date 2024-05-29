project (ayfstarter)

set(PROJECT_PREFIX starter_app)
set(SDK_FOLDER $ENV{AYF_SDK_PATH})

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
endif()

## install(CODE "execute_process(COMMAND install_flutter_app.sh)")

