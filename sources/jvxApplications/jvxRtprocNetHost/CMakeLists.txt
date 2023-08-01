if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxRtprocNetHost)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

#set(LOCAL_UI_SOURCES
#	${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/mainCentral.ui
#	)

set(LOCAL_UI_SOURCE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxRtprocHost.qrc
	)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
  	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
 )

include_directories(
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/network/
   ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_master/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_slave/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
  )

set(LOCAL_LIBS
  jvxAuTNetwork_master_static
  jvxAuTNetwork_slave_static
  jvxLAudioHost__algoshared_static
	jvx-qt-helpers_static
	jvx-qt-component-widgets_static
  jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	jvx-qt-app-templates_static
)

set(LOCAL_IMAGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/images)


if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
