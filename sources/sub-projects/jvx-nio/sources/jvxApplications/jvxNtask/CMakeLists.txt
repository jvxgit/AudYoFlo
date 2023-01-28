if(DEFINED JVX_SDK_PATH)
	cmake_minimum_required (VERSION 2.8.11)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Some common include files
set(PROJECT_NAME jvxNtask)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Add mainWidget and other ui files
#set(LOCAL_UI_SOURCES
#	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myMainWidget.ui
#	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxNtask.qrc
	)

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h

	${CMAKE_CURRENT_SOURCE_DIR}//src/jvx_sequencer_common_2audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
	)

# Some required include paths
include_directories(
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-qt-mixmatrix/include
	

	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src

  	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/src

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTMSWText2Speech

  	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_master/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_slave/
 
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated 
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src

	${JVX_SUBPRODUCT_NTASK_LIBS_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNNtask/
	${JVX_SUBPRODUCT_NTASK_LIBS_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNVtask/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNAsyncIo

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-central-widgets/include
 )

if(JVX_COMPILE_NTASK_WITH_LOGFILEWRITER)	
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_NTASK_WITH_DATALOGGER")
	include_directories(${JVX_DATALOGGER_BINARY_DIR})
endif()

# Add algorithm specific library
set(LOCAL_LIBS
	#jvx-qt-hoa_static
	jvxAuTNetwork_master_static
	jvxAuTNetwork_slave_static
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	jvxSpNAsyncIo_sm_static
	jvx-qt-component-widgets_static
	jvx-qt-app-templates_static
	jvx-qt-central-widgets_static
	)

if(${JVX_OS} MATCHES "windows")
	set(LOCAL_LIBS ${LOCAL_LIBS}
	  jvxTMswText2Speech_static)
endif()
	
if(JVX_COMPILE_NTASK_WITH_LOGFILEWRITER)
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxTDataLogger_static)
endif()

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

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

## ======================================================================

# Some compile defines
#set(LOCAL_COMPILE_DEFINITIONS "JVX_ADD_HOST_WEBSERVER;JVX_PROJECT_NAMESPACE=JVX_MY_HOST;JVX_NO_SEQUENCER;JVX_NO_RTVIEWER;JVX_NO_MESSAGES")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROJECT_NAMESPACE=JVX_MY_HOST;JVX_ADD_WIDGET_WRAPPER")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/start_mat)

set(LOCAL_COMPILE_DEFINITIONS_COMMON "${LOCAL_COMPILE_DEFINITIONS}")
set(LOCAL_LOCAL_LIBS_COMMON "${LOCAL_LIBS}")

set(LOCAL_LIBS ${LOCAL_LIBS}
	jvxAuNNtask_static)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

# Project with async module operation with master at secondary side
set(JVX_TARGET_NAME "jvxVtask")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COMMON};JVX_LINK_VTASK;JVX_SPECIFIC_MAIN_NODE=JVX_COMPONENT_SIGNAL_PROCESSING_NODE,1,0")
set(LOCAL_LIBS "${LOCAL_LOCAL_LIBS_COMMON}")
set(LOCAL_LIBS ${LOCAL_LIBS} jvxSpNVtask_static)
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
