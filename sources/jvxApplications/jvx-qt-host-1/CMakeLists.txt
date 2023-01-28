cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)


set(PROJECT_NAME jvx-qt-host-1)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_QT_RESOURCES
  uisrc/jvx-qt-host.qrc
  )

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
  )

set(LOCAL_SOURCES
  ## ${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/mainCentral.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHost_hooks.cpp

  	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio_video.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio_video.cpp
  )

#Define private namespace
set(LOCAL_COMPILE_DEFINITIONS "JVX_PROJECT_NAMESPACE=JVX_QT_DEVELOP_HOST_1")


set(LOCAL_LIBS 
	jvxLDevelopHost-shared_static 
	jvx-qt-app-templates_static
	)
# runtimeobject)

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
  set(JVX_MAC_OS_CREATE_BUNDLE_AUDIO_PLUGINS TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

## ==============================================================
## Create the same project output with another compile option
## ==============================================================

set(dep_target ${PROJECT_NAME})

set(PROJECT_NAME "${PROJECT_NAME}-c")
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_COMPILE_DEFINITIONS "JVX_PROJECT_NAMESPACE=JVX_QT_DEVELOP_HOST_1;JVX_ALLOW_ONLY_OVERLAYS")
#set(LOCAL_COMPILE_DEFINITIONS "JVX_PROJECT_NAMESPACE=JVX_QT_DEVELOP_HOST_1;JVX_ALLOW_ONLY_OVERLAYS;JVX_NO_SEQUENCER;JVX_NO_RTVIEWER;JVX_NO_MESSAGES;JVX_NO_EXPERT_MENUES")
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
add_dependencies(${PROJECT_NAME} ${dep_target})
