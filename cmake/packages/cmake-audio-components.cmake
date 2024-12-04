## ======================================================================
## The audio libs
if(JVX_USE_PART_PAUDIO)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PORTAUDIO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPortAudio_static)
endif()

if(JVX_USE_PART_ASIO)
  include_directories(${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio)
  set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_ASIO")
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAsio_static)
endif()

if(JVX_USE_PART_COREAUDIO)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_COREAUDIO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTCoreAudio_static ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES})
endif()

if(JVX_USE_PART_ALSA)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_ALSA")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAlsa_static asound)
endif()

if(JVX_USE_PART_PIPEWIRE)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPipewire)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PIPEWIRE")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPipewire_static)
endif()
	
if(JVX_USE_PART_WASAPI)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTWindows)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_WASAPI")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTWindows_static)
	
	# Additional dependency for WASAPI projects in SDK build
	if(JVX_USES_SDK)
		set(LOCAL_LIBS ${LOCAL_LIBS} avrt)
	endif()
endif()

## ======================================================================
