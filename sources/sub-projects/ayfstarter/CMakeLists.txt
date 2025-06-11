option(AYFSTARTER_SIMPLE_INSTALLER "Build a simple installer (zip) for flutter code" OFF)
option(AYFSTARTER_USE_RUST_LIBS "Build rust libs instead of C libs where possible" OFF)

if(NOT JVX_USES_SDK)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.local.cmake)
	include(${JVX_BASE_CMAKE_DIR}/paths.cmake)
	include(${JVX_SUBPRODUCT_CMAKE_DIR}/paths.${JVX_PRODUCT}.cmake)
else()
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.sdk.local.cmake)
	include(${JVX_BASE_ROOT}/cmake/cmake-sdk/paths.sdk.cmake)
	include(${JVX_SUBPRODUCT_ROOT}/cmake/cmake-sdk/paths.sdk.${JVX_PRODUCT}.cmake)
endif()

if(JVX_USE_ORC)
    set(AYFSTARTER_SUB_PROJECT_COMPILE_DEFINITIONS "USE_ORC" CACHE INTERNAL "local project compile options")
	set(AYFSTARTER_SUB_PROJECT_PCG_FLAGS "-D;JVX_ORC_PROPERTIES" CACHE INTERNAL "local project PCG file options")
endif()

if(AYFSTARTER_USE_RUST_LIBS)
	if(JVX_USE_RUST)
		set(AYF_STARTER_STATIC_LIBS
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Libraries/rust/ayfstarterlib
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Libraries/rust/ayfstarterlib_debug
			)
	else()
		message("Option AYFSTARTER_USE_RUST_LIBS is active but rust is not enabled.Deactivating option AYFSTARTER_USE_RUST_LIBS!")
		set(AYFSTARTER_USE_RUST_LIBS OFF)
	endif()
endif()

if(NOT AYFSTARTER_USE_RUST_LIBS)
	set(AYF_STARTER_STATIC_LIBS ${AYF_STARTER_STATIC_LIBS}
		${CMAKE_CURRENT_SOURCE_DIR}/sources/Libraries/c/ayfstarterlib
		)
endif()


jvx_addSubdirs("Configuring static library for ayfstarter core functionality" "${AYF_STARTER_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/Library")

set(AYF_STARTER_AUDIONODE
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/Components/AudioNodes/ayfAuNStarter
  )
jvx_addSubdirs("Configuring shared library for ayfstarter audio node" "${AYF_STARTER_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/AudioNode")
	
if(NOT JVX_RELEASE_SDK)
	set(AYF_STARTER_APPLICATION "")
	if(JVX_USE_PART_WEBSERVER)
	
		set(AYF_STARTER_APPLICATION ${AYF_STARTER_APPLICATION}
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Applications/ayfstarter-web)
		
	endif()
	
	if(JVX_USE_QT)
		
		set(AYF_STARTER_APPLICATION ${AYF_STARTER_APPLICATION} 
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Applications/ayfstarter-qt)
				
	endif()	
endif()

set(AYF_STARTER_APPLICATION ${AYF_STARTER_APPLICATION} ${CMAKE_CURRENT_SOURCE_DIR}/sources/Applications/ayfstarter-native-config)
jvx_addSubdirs("Configuring application ayfstarter " "${AYF_STARTER_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/Application")

if(JVX_RELEASE_SDK)
		
	if(JVX_FLUTTER_UI)
		
		if(${JVX_OS} MATCHES "windows")
			message("Flutter code generation for Windows")
			set(AYFSTARTER_HOSTCONF_DLL_CORE "ayfstarter-native-config_import.dll")
			set(AYFSTARTER_HOSTCONF_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/${AYFSTARTER_HOSTCONF_DLL_CORE}")
			set(AYFSTARTER_FFI_DLL_CORE "jvx-flutter-native-shared_import.dll")
			set(AYFSTARTER_FFI_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/${AYFSTARTER_FFI_DLL_CORE}")
			
			set(AYFSTARTER_FFI_PRELOAD_DLLS "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/zlib.dll,${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/libfftw3-3.dll")		
			
			install(FILES ${AYFSTARTER_HOSTCONF_DLL} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/local_assets)
			install(FILES ${AYFSTARTER_FFI_DLL} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/local_assets)
		elseif(${JVX_OS} MATCHES "linux")
			message("Flutter code generation for linux")
			set(AYFSTARTER_FFI_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}/libjvx-flutter-native-shared_import.so")
			set(AYFSTARTER_FFI_PRELOAD_DLLS "")		
		elseif(${JVX_OS} MATCHES "Android")
			message("Flutter code generation for linux")
			set(AYFSTARTER_FFI_DLL "libjvx-flutter-native-shared_import.so")
			set(AYFSTARTER_FFI_PRELOAD_DLLS "")		
		else()
			message(FATAL_ERROR "Error: Platform ${JVX_OS} currently not supported!")
		endif()
		
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/example/.starter_app.env.in ${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/example/.starter_app.env @ONLY)
		message("${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/example/.starter_app.env.in -> ${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/example/.starter_app.env")
		
		set(AYF_FLUTTER_WORKDIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RUNTIME})
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/launch.json.in ${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/.vscode/launch.json @ONLY)
		message("${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/launch.json.in -> ${CMAKE_CURRENT_SOURCE_DIR}/flutter/starter_app/.vscode/launch.json")	
		
		# Prepare installer of config dll or so
		set(SDK_FOLDER $ENV{AYF_SDK_PATH})
		
		# Empty placeholders MATTER!!!
		
		# set(AYF_FLUTTER_SDK_ASSETS "  - local_assets/${AYFSTARTER_HOSTCONF_DLL_CORE}\n  - local_assets/${AYFSTARTER_FFI_DLL_CORE}")
		# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/pubspec.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/pubspec.yaml)
		
		# message("${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/pubspec.yaml.in -> ${CMAKE_CURRENT_SOURCE_DIR}/flutter/ayf_starter_corepack/pubspec.yaml.new")	
		
		if(AYFSTARTER_SIMPLE_INSTALLER)
			message("Preparing post_install script in folder <${CMAKE_CURRENT_SOURCE_DIR}/post_install>")
			if(${JVX_OS} MATCHES "windows")
				# if(NOT ${CMAKE_BUILD_TYPE} MATCHES "^(Release)$")
				#	message("WARNING: building flutter installer in debug mode: ffi dll will not work without VS installed")
				#endif()
			endif()
			add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/post_install)
		endif()

	endif()
		
endif()

# message(FATAL_ERROR "Hier -- JVX_FLUTTER_UI = ${JVX_FLUTTER_UI} -- AYFSTARTER_SIMPLE_INSTALLER = ${AYFSTARTER_SIMPLE_INSTALLER}")
