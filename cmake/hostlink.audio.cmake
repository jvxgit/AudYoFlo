# Link against either audio host lib or develop host lib
if(JVX_LINK_WITH_AUDIO_HOST)

	# Link with audio host
	set(LOCAL_LIBS ${LOCAL_LIBS}
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