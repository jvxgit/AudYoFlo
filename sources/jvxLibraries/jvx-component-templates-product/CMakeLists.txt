if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-component-templates-product)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxApplications/jvx_init_link_objects_tpl.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioCodecs/CjvxAudioCodec.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioCodecs/CjvxAudioDecoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioCodecs/CjvxAudioEncoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxRichAudioNode.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxAuN2AudioMixer.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioCodecs/CjvxAudioCodec.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioCodecs/CjvxAudioDecoder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioCodecs/CjvxAudioEncoder.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxRichAudioNode.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuN2AudioMixer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuN2AudioMixer_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuN2AudioMixer_test.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuN2AudioMixer_config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuN2AudioMixer_props.cpp
	
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-dsp/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
  )

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioCodecs/CjvxAudioCodec_pcg.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioNodes/CjvxRichAudioNode.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioNodes/CjvxAuN2AudioMixer.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	-I ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base/pcg
	-I ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/pcg
	-I ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/pcg
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/pcg/jvxElements
	)

set(LOCAL_LIBS jvx-component-templates-base_static jvx-dsp_static jvx-audiocodec-helpers_static ayf-node-connections_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
