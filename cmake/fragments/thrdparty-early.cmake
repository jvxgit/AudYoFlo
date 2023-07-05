if(JVX_USE_QT)
  
	if(JVX_INSTALL_QT_ON_BUILD)
		set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qt)
	endif()
endif()