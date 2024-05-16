## Why only in the SDK build? Since the path to the dlls is set in the .env files which are only built in the SDK build!

if(JVX_RELEASE_SDK)
	# set(AYFBINRENDER_FFI_DLL ${CMAKE_CURRENT_BINARY_DIR}/sources/jvxLibraries/ayfbinrender-native/Debug/ayfbinrender-native.dll)
	# set(AYFBINRENDER_FFI_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/ayfbinrender-native_import.dll")
	set(AYFBINRENDER_HOSTCONF_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/ayfbinrender-native-config_import.dll")
	set(AYFBINRENDER_FFI_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/jvx-flutter-native-shared_import.dll")
	set(AYFBINRENDER_FFI_PRELOAD_DLLS "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/zlib.dll,${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/libfftw3-3.dll")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/example/.ayf_pack_example.env.in ${CMAKE_CURRENT_SOURCE_DIR}/example/.ayf_pack_example.env @ONLY)

	set(AYF_FLUTTER_WORKDIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RUNTIME})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/launch.json.in ${CMAKE_CURRENT_SOURCE_DIR}/.vscode/launch.json @ONLY)
	
else()
	message("Entering folder for runtime build - nothing to be done here!")
endif()

