if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Some common include files
set(PROJECT_NAME jvxMeasureIr)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Source code files
set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/involve_audio_nodes.cpp
	)

# Some required include paths
include_directories(
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/
 )


# Add algorithm specific library
set(LOCAL_LIBS
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
set(LOCAL_LIBS
	jvx-qt-measure-ir-2dev-app-lib_static
	jvxSpNSpeakerEqualizer_static
	)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
