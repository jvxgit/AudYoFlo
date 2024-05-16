if(JVX_RELEASE_SDK)
	
	# Produce the right entries in the ffigen code lines
	set(FFIGEN_HOST_HEADER_PATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_INCLUDE_STATIC}/jvx-flutter-native-static/include/flutter_native_api.h")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pubspec.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/pubspec.yaml @ONLY)

	# Configure for all tests
	set(AYFBINRENDER_HOSTCONF_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/ayfbinrender-native-config_import.dll")
	set(AYFBINRENDER_FFI_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/jvx-flutter-native-shared_import.dll")
	set(AYFBINRENDER_FFI_PRELOAD_DLLS "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/zlib.dll,${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/libfftw3-3.dll")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/example/.ayf_ffihost_example.env.in ${CMAKE_CURRENT_SOURCE_DIR}/example/.ayf_ffihost_example.env @ONLY)

	set(AYF_FLUTTER_WORKDIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RUNTIME})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/launch.json.in ${CMAKE_CURRENT_SOURCE_DIR}/.vscode/launch.json @ONLY)
else()
	message("Entering folder for runtime build - nothing to be done here!")
endif()