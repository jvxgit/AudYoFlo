if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-host-widgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx_connections.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx_sequencer.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx_widget_in_frame.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx_connect_results.ui

	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/showInformation.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/floatProperties.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/integerProperties.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/stringAssignmentProperties.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/stringFilenameProperties.ui
   )

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connect_results.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connections_widget_rule.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connections_widget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connections_widget_ui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_widget_in_frame_dialog.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connect_results.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_widget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_widget_ui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_widget_default.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_mainwindow_control.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_mainwindow_control.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_newvalue_dialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_newvalue_dialog.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_eventloop_object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_eventloop_object.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_menu_host_bridge_components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_menu_host_bridge_components.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_menu_host_bridge_components.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connections_widget.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connections_widget_const.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_widget_in_frame_dialog.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_widget.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtHostWidgets.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/stringAssignmentProperties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/stringFilenameProperties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/integerProperties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/floatProperties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/showInformation.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/stringAssignmentProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/stringFilenameProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/integerProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/floatProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/showInformation.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/baseUiProperties.h

	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
   ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTWebServer/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/

  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTDataConverter/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTResampler/

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSocket/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRemoteCall/

  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileReader/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileWriter/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper/

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRs232/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreadController/

  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHAppHost

  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common

  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
  jvx-qt-helpers_static
  )

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
