if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Some common include files
set(PROJECT_NAME jvx-qt-measure-ir-2dev-app-lib)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Add mainWidget and other ui files
set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc/myMainWidget.ui
	)

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc/jvxMeasureIr.qrc
	)

## 
if(NOT JVX_USES_SDK)
	#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.local.cmake)
	include(${JVX_CMAKE_DIR}/paths.cmake)
	include(${JVX_SUBPRODUCT_ROOT}/cmake/paths.${JVX_PRODUCT}.cmake)
else()
	#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.sdk.local.cmake)
	include(${JVX_BASE_ROOT}/cmake/cmake-sdk/paths.sdk.cmake)
	include(${JVX_SUBPRODUCT_ROOT}/cmake/cmake-sdk/paths.sdk.${JVX_PRODUCT}.cmake)
endif()

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mySpecificCentralWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/mySpecificCentralWidget.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_common_2audio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connection_rules_MeasureIr_audio.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/myCentralWidget_ui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/myCentralWidget.h

	)

# Some required include paths
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-qt-mixmatrix/include

	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src

	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTMSWText2Speech

  	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/src

	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNAsyncIo

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNPlayChannelId/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingTechnologies/jvxSpTDeploySignal

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer/

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNPlayChannelId/

  	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_master/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_slave/

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNMeasureIr
 
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated 
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src

	${JVX_DATALOGGER_BINARY_DIR}

	${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/qcustomplot-${JVX_QCUSTOMPLOT_VERSION}/src
	
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNMeasureIr
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNMeasureIr2Dev/

	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-qt-measure-ir/include

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-central-widgets/include

	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTLocalTextLog

 )


# Add algorithm specific library
set(LOCAL_LIBS
	#jvx-qt-hoa_static
	jvxAuTNetwork_master_static
	jvxAuTNetwork_slave_static
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	jvxAuNMeasureIr2Dev_static
	jvxSpNAsyncIo_sm_static
	jvxSpTDeploySignal_static
	jvxAuNPlayChannelId_static
	jvx-qt-component-widgets_static
	jvx-qt-app-templates_static
	jvxTDataLogger_static
	jvxTLocalTextLog_static
	jvx-qt-central-widgets_static
	jvx-qt-measure-ir_static
	)
	# jvxSpNHoaLocate_static

if(${JVX_OS} MATCHES "windows")
	set(LOCAL_LIBS ${LOCAL_LIBS}
	  jvxTMswText2Speech_static)
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
set(BUILD_STATIC TRUE)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
