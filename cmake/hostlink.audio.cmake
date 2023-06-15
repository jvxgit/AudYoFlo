# Link against either audio host lib or develop host lib
if(JVX_LINK_WITH_AUDIO_HOST)

	include_directories(
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-central-widgets/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src)	
			
	# Link with audio host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLQtSaWidgetWrapper_static
		jvxLQtSaWidgets_static
		jvx-qt-app-templates_static
		jvx-qt-central-widgets_static
		jvxLAudioHost__algostatic_static
		)

	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.audiohost.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
	endif()
else() 
	include(${JVX_CMAKE_DIR}/hostlink.cmake)
endif(JVX_LINK_WITH_AUDIO_HOST)