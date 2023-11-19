# Some common include files
set(PROJECT_NAME jvxMeasureIr)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Source code files
set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/involve_audio_nodes.cpp
	)

# Some required include paths
include_directories(
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/
 )


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

	# Link with develop host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLDevelopHost_static
		)
	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.develophost.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
	endif()

endif()

set(LOCAL_LIBS_COMMON ${LOCAL_LIBS})

# Add algorithm specific library
set(LOCAL_LIBS ${LOCAL_LIBS_COMMON}
	jvx-qt-measure-ir-1dev-app-lib_static
	jvxSpNSpeakerEqualizer_static
	)

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/start_mat)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

# Here, the version for 2 devices
set(JVX_TARGET_NAME ${PROJECT_NAME}2Dev)
set(JVX_TARGET_NAME_STATIC ${PROJECT_NAME}2Dev)
set(LOCAL_LIBS ${LOCAL_LIBS_COMMON}
	jvx-qt-measure-ir-2dev-app-lib_static
	jvxSpNSpeakerEqualizer_static
	)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
