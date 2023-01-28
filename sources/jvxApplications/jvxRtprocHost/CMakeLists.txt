cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxRtprocHost)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

#set(LOCAL_UI_SOURCES
#	${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/mainCentral.ui
#	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myMainWidget.ui
#	)

set(LOCAL_UI_SOURCE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxRtprocHost.qrc
	)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
  ## ${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/mainCentral.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp

		${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio_video.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio_video.cpp
 )

include_directories(
	# ${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/
	${JVX_BINARY_DIR}/base/sources/jvxComponents/jvxAudioNodes/jvxAuNSimple/
    ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
  )

set(LOCAL_LIBS
  jvxLAudioHost__algoshared_static
  jvxLQtSaWidgetWrapper_static
  jvxLQtSaWidgets_static
  jvx-qt-app-templates_static
)

set(LOCAL_COMPILE_DEFINITIONS "JVX_ADD_WIDGET_WRAPPER")
set(LOCAL_IMAGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/images)

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
  set(JVX_MAC_OS_CREATE_BUNDLE_AUDIO_PLUGINS TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
