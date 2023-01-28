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
	else(JVX_LINK_WITH_DEVELOP_HOST)
		if(JVX_LINK_WITH_CONSOLE_HOST)
			# Link with develop host
			set(LOCAL_LIBS ${LOCAL_LIBS}
				jvxLConsoleHost-static_static
				jvxLWebConsoleTools_static
				)
			# In case of standalone builds, we need to pull in some other libraries
			if(DEFINED JVX_SDK_PATH)
				include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.consolehost.cmake)
				include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
			endif()
			force_console_app()
		endif(JVX_LINK_WITH_CONSOLE_HOST)
	endif(JVX_LINK_WITH_DEVELOP_HOST)