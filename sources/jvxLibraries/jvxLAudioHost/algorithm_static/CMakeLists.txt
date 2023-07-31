cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLAudioHost__algostatic)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../uisrc/mainApplication.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../uisrc/configureAudio.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../uisrc/audioAdditionalArgs.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../uisrc)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_specific.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/additionalArgs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_bootup.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_shutdown.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_sequencer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_run.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_ui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/configureAudio.cpp  
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common/realtimeViewer_helpers.cpp
  	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/target/main.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow_specific.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/uMainWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/additionalArgs.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/configureAudio.h

		${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxApplication.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxApplication.h

  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include

    ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/include
    ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/src

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/include

   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreads/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTLogRemoteHandler/
  
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTDataConverter/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTResampler/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileReader/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileWriter/

  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper/

  ${CMAKE_CURRENT_BINARY_DIR}

  ## Static and dynamic lib versions are exported using the same header file (C-entries)
  ## but can be distinguished by using different libraries!
  ${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHAppHost

  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
     ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
	 ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include

	  ${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/src
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

if(JVX_OS MATCHES "windows")
	if(NOT JVX_USE_CONSOLE_APPS)
		set(LOCAL_COMPILE_DEFINITIONS "")
	endif()
endif()

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HOST_USE_ONLY_STATIC_OBJECTS;JVX_CONFIGURE_HOST_STATIC_NODE")

set(LOCAL_LIBS
  jvxLFileIO_static
  jvxLthreads_static

  # Allow only static component linkage
  jvxHAppHost_static_nd
  jvxTconfigProcessor_static
  jvxTSystemTextLog_static
  jvxTThreads_static
  jvxTDataLogger_static
  jvxTDataConverter_static
  jvxTResampler_static
  jvxTrtAudioFileReader_static
  jvxTrtAudioFileWriter_static
  jvxTLogRemoteHandler_static
  jvxAuTGenericWrapper_static
  qwt_static
  jvx-app-host-static_static
  jvx-qt-host-widgets_static
  jvx-qt-helpers_static
  )

if(JVX_USE_PART_PAUDIO)
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPortAudio_static)
else()
	set(LOCAL_COMPILE_DEFINITIONS ${LOCAL_COMPILE_DEFINITIONS}
		JVX_AUDIO_EXCLUDE_PORTAUDIO
		)
endif()

if(JVX_OS MATCHES "windows")
	if(DJVX_USE_PART_CORECONTROL)
		set(LOCAL_LIBS ${LOCAL_LIBS} imm32 Setupapi jvxTThreadController_static)
		set(LOCAL_COMPILE_DEFINITIONS ${LOCAL_COMPILE_DEFINITIONS} "JVX_ADD_THREAD_CONTROLLER")

	endif()
endif()

set(BUILD_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

#if(JVX_INTEGRATE_SOCKET_UI_SUPPORT)
#	set(ADDITIONAL_SOURCES
#		${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/qt-audio-host/src/configureAudio_specific_socket.cpp
#		)
#	endif()

#	if(JVX_USE_DEFAULT_WIDGET)
 #   set(ADDITIONAL_SOURCES ${ADDITIONAL_SOURCES}
  #    ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/qt-audio-host/src/mainCentral.cpp
   #   )
 # endif()
# We may allow network objects in the host
#if(JVX_INTEGRATE_SOCKET_UI_SUPPORT)
#	set(ADDITIONAL_COMPILE_DEFINITIONS "${ADDITIONAL_COMPILE_DEFINITIONS};JVX_AUDIO_HOST_WITH_NETWORK_ACCESS")
#  endif()

#  if(NOT JVX_USE_DEFAULT_WIDGET)
#    set(ADDITIONAL_COMPILE_DEFINITIONS "${ADDITIONAL_COMPILE_DEFINITIONS};QT_WIDGETS_NOT_HANDLED;JVX_MAIN_CENTRAL_LOCAL")
#  endif()
