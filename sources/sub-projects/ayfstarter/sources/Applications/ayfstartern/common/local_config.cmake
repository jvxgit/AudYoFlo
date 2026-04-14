set(LOCAL_SOURCES ${LOCAL_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstartern-components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstartern-connectionrules.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstartern-hostconfig.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstartern-sequencer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstartern-common.h
	)
	
include_directories(

	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/ayfAuNMixer
	${SUBPRODUCT_AYFSTARTER_COMPONENTS_BINARY_DIR}/Automation/ayfATStarter/
	${SUBPRODUCT_AYFSTARTER_COMPONENTS_BINARY_DIR}/AudioNodes/ayfAuNStarter/   
	
	# Standard components
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTSyncClock/	
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNMixChainEnterLeave/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNAsyncIo
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSynchronize
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNForwardBuffer/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNConvert/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamDecoder/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamEncoder/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNLevelControl
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxVideoNodes/jvxViNMixer/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxVideoNodes/jvxViNCameraConvert/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxVideoNodes/jvxViNOpenGLViewer/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxVideoNodes/jvxViNDefault/

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated
 )
 
 set(LOCAL_LIBS ${LOCAL_LIBS}
	jvxAuTSyncClock_static
	ayfAuNMixer_static
	ayfATStarter_static
	ayfAuNStarter_static
	jvxSpNMixChainEnterLeave_static
	jvxSpNAsyncIo_sm_static	
	jvxSpNSynchronize_static
	jvxAuNForwardBuffer_static
	jvxAuNConvert_static	
	jvxAuNBitstreamDecoder_static
	jvxAuNBitstreamEncoder_static
	jvxSpNLevelControl_static
	jvxViNMixer_static
	ayfViNCameraConvert_static
	jvxViNOpenGLViewer_static
	jvxViNDefault_static
	)
  
if(AYFSIMPLEPHONE_BARESIP)
	include_directories(${AYF_BARESIP_BINARY_PATH})
	set(LOCAL_LIBS ${LOCAL_LIBS} ayfAuTBaresip_static)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};AYFSIMPLEPHONE_BARESIP")
endif()

	## File reader with either ffmpeg or JVX reader/writer
if(JVX_USE_FFMPEG)

	include_directories(
		${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxPackages/ffmpeg/jvxComponents/jvxPackages/jvxPaFfmpeg)
	set(LOCAL_LIBS ${LOCAL_LIBS} 
		jvxPaFfmpeg_static
		${JVX_FFMPEG_SYS_LIB_SMIIDS})
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_FFMPEG_FILE_IO")
endif()

# Link against either audio host lib or develop host lib
include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

## ======================================================================
## The video libs
# Pull in all video components
include(${JVX_CMAKE_DIR}/packages/cmake-video-components.cmake)
