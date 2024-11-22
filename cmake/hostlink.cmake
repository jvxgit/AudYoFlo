	if(JVX_LINK_WITH_DEVELOP_HOST)
		# Link with develop host
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxLDevelopHost-static_static
			)
		# In case of standalone builds, we need to pull in some other libraries
		if(DEFINED JVX_SDK_PATH)
			include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.develophost.cmake)
			include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
		endif()
	else()
		if(JVX_LINK_WITH_CONSOLE_HOST)
		
			include_directories(
				# Console linkage
				${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
				${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHJvx/
				${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
				${JVX_BASE_BINARY_INCLUDE_PATH}/jvxLConsoleTools/components-static/generated
				${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLWebConsoleTools/include
				${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
				${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
				${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
				${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
				${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include)
				
			# Link with console host
			set(LOCAL_LIBS ${LOCAL_LIBS}				
				jvxLConsoleHostApp-static_static
				jvxLWebConsoleTools_static
				)
				
			# We need to add this library with special magic to override the weak symbols in GCC
			# In linux, the WHOLE_ARCHIVE/NO_WHOLE_ARCHIVE options resolve to --whole-archive 
			# and --no-whole-archive. That means that the symbols from the jvx-link-frontend_static library may override 
			# the weak symbols. Alternatively, the library could be defined as an OBJECT LIBRARY by setting the flag
			# set(CREATE_OBJECT_LIBRARY TRUE)
			# Haven't tested, I assume that this option will not allow to link with the SDK since the library is just missing.
			if(JVX_LINK_WITH_CONSOLE_HOST_FRONTEND_BACKEND_LIB)
				set(LOCAL_LIBS ${LOCAL_LIBS} ${WHOLE_ARCHIVE} jvx-link-frontend_static ${NO_WHOLE_ARCHIVE})
			endif()

			# In case of standalone builds, we need to pull in some other libraries
			if(DEFINED JVX_SDK_PATH)
				include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.consolehost.cmake)
				include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
			endif()
			force_console_app()
		endif()
	endif()
