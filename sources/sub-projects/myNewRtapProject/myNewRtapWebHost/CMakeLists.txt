cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME myNewRtapWebHost)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(JVX_LINK_WITH_CONSOLE_HOST TRUE)

set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/jvx_hooks.cpp

		${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/myNewRtapRtHooks.h
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/myNewRtapRtHooks.cpp

		${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp

	)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLWebConsoleTools/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include

  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_INCLUDE_PATH}/jvx-component-templates-product/generated

    ${JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LIBS_BINARY_DIR}/myNewRtapAudioNode/

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
	${JVX_BINARY_DIR}/base/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/

	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/
   )


set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	myNewRtapAudioNode_static
	myNewRtapLibrary_static
	)

include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

## ======================================================================
## The audio libs
if(JVX_USE_PART_PAUDIO)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PORTAUDIO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPortAudio_static)
endif()

if(JVX_USE_PART_ASIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAsio_static)
endif()

if(JVX_USE_PART_COREAUDIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTCoreAudio_static ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES})
endif()

if(JVX_USE_PART_ALSA)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAlsa_static asound)
endif()
## ======================================================================

set(LOCAL_START_SCRIPT "start-myNewRtapWebHost")
force_console_app()

set(JVX_PREPARE_WORKSPACE_VS TRUE)
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
