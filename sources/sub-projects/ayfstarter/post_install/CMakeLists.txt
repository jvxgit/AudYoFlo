project (ayfstarter)

set(PROJECT_PREFIX starter_app)
install(CODE "execute_process(COMMAND ${mysh} -c 
				\"${CMAKE_CURRENT_SOURCE_DIR}/install_flutter_app.sh ${CMAKE_CURRENT_SOURCE_DIR} ${JVX_OS} ${PROJECT_PREFIX} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN} example; exit\")")
## install(CODE "execute_process(COMMAND install_flutter_app.sh)")

