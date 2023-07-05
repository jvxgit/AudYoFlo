if(JVX_USE_QT)
  
	if(JVX_INSTALL_QT_ON_BUILD)
		set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY} 
			${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qt)
	endif()
endif()

if(JVX_FLUTTER_UI)
	# Check for invalid Flutter activation on win32 builds
    if(${JVX_OS} MATCHES "windows")
		if(${JVX_PLATFORM} MATCHES "32bit")
			message(FATAL_ERROR "Flutter is not supported for win32! Please deactivate the option <JVX_FLUTTER_UI>.")
		endif()
	endif()
	
	if(JVX_INSTALL_FLUTTER_ON_BUILD)
		set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY} 
			${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/flutter)
	endif()
endif()
	
