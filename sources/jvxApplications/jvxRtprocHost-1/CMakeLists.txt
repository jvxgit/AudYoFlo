if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxRtprocHost-1)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

#set(LOCAL_UI_SOURCES
#	${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/mainCentral.ui
#	)

set(LOCAL_UI_SOURCE_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxRtprocHost-1.qrc
	)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
	)

include_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNSimple/

   ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
 )

set(LOCAL_LIBS
  jvxAuNSimple_static
  jvxLAudioHost__algostatic_static
  jvx-qt-app-templates_static
)

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
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAlsa_static asound jvx-component-templates-product_static)
endif()
## ======================================================================

set(LOCAL_IMAGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/images)

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
