if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-m-host-1-octave)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
  ${OCTAVE_INCLUDE_PATH}
  ${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHAppHost/
  ${JVX_BASE_ROOT}/software/typedefs/tools/
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTExternalCall/cmake/octave/
  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTOfflineAudio/
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_DSP_BASE_INCLUDE_PATH}
  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper/

   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
   ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  )

set(LOCAL_SOURCES
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_config.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_connections.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_mex.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_components.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_sequencer.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/src/mexJvxHost_properties.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/mex/target/entryMex.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common/realtimeViewer_helpers.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/mexJvxHost_specific.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/mexJvxHost_hooks.cpp

  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
  )


set(LOCAL_LIBS
  jvxAuTGenericWrapper_static
  jvxLFileIO_static
  jvxTExternalCall_octave_static
   jvx-app-host-shared_static
  jvxHAppHost_static
  jvxLthreads_static
  )

if(JVX_USE_PART_PAUDIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPortAudio_static)
endif()
if (JVX_USE_PART_RS232)
  	if(${JVX_OS} MATCHES "windows")
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxTRs232Win32_static
			)
	endif()
  	if(${JVX_OS} MATCHES "linux")
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxTRs232Glnx_static
			)
	endif()
endif()
if(JVX_USE_PART_ASIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAsio_static)
endif()
if(JVX_USE_PART_COREAUDIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTCoreAudio_static ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES})
endif()
if(JVX_OS MATCHES "windows")
	if(DJVX_USE_PART_CORECONTROL)
		set(LOCAL_LIBS ${LOCAL_LIBS} imm32 Setupapi jvxTThreadController_static)
		set(LOCAL_COMPILE_DEFINITIONS ${LOCAL_COMPILE_DEFINITIONS} "JVX_ADD_THREAD_CONTROLLER")
	endif()
endif()
if(JVX_USE_PART_ALSA)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAlsa_static asound)
endif()

set(BUILD_MEX_OCTAVE TRUE)
set(LOCAL_OCT_LIBS ayf-mex-connect_o_static)
set(LOCAL_M_FILES_SOURCE_DIR "../../m-files/octave;${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/m-files/mex")
set(JVX_RENAME_RELEASE "jvxHostRt")
set(LOCAL_COMPILE_DEFINITIONS "CONFIG_COMPILE_FOR_OCTAVE;JVX_MATLAB_RT_AUDIO")

# set(LOCAL_START_SCRIPT startJvxHost_oct)
# install(PROGRAMS ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/scripts/m-files/jvx_start_rtproc.m DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT "release")

include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)

# once again for offline version
set(JVX_RENAME_RELEASE "jvxHostOff")
set(JVX_TARGET_NAME ${JVX_TARGET_NAME}_off)
set(LOCAL_LIBS
	jvxAuTOfflineAudio_static
	jvxTExternalCall_octave_static
	 jvx-app-host-shared_static
	jvxHAppHost_static
	jvx-dsp-base_static
	)
set(LOCAL_COMPILE_DEFINITIONS "CONFIG_COMPILE_FOR_OCTAVE;JVX_MATLAB_OFFLINE_AUDIO")
set(LOCAL_START_SCRIPT_OCTAVE ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/${JVX_OS}/startJvxHost_oct)
set(LOCAL_IMAGES_MATLAB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../images)
set(LOCAL_START_SCRIPT_OCTAVE_SCRIPT_NAME jvx-oct)

include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
