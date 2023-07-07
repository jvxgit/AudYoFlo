set(PROJECT_NAME flutter)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(JVX_FLUTTER_VERSION "3.7.4")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/flutter)
	message("flutter sub project not available. Starting script to download it.") 
	message("Make sure you will add ${CMAKE_CURRENT_SOURCE_DIR}/flutter-${JVX_FLUTTER_VERSION}/bin to the PATH environment variable!") 
	message("Make sure you will set FLUTTER_SDK_PATH=${CMAKE_CURRENT_SOURCE_DIR}/flutter-${JVX_FLUTTER_VERSION} in your compile script!") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh; exit")
	message("flutter installation complete.")
endif()

