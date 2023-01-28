cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-dsp-cpp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/correlation2Lsfs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/openLoopPitchEstimation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/autocorrelation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/lpcref.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/lsflpcUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/miscSignalUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/SignalUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/speech2Lsf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/vocoder.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioChannelHandler/CjvxExtInputChannelHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioChannelHandler/CjvxExtOutputChannelHandler.cpp
	
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvxText2Speech/CjvxTxt2Speech_mix.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/correlation2Lsfs.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/openLoopPitchEstimation.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/autocorrelation.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/lpcref.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/lsflpcUtils.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/miscSignalUtils.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/signalUtils.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVocoder/speech2Lsf.h
	
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioChannelHandler/CjvxExtInputChannelHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioChannelHandler/CjvxExtOutputChannelHandler.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioFftEqualizer/CjvxAudioFftEqualizer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioFixedFftN1Filter/CjvxAudioFixedFftN1Filter.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxFastConvolution/CjvxFastConvolution.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVocoder/vocoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVocoder/configVocoder.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioChannelHandler/IjvxExtInputChannelHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioChannelHandler/IjvxExtOutputChannelHandler.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxText2Speech/CjvxTxt2Speech_mix.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioFftEqualizer/CjvxAudioFftEqualizer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioFixedFftN1Filter/CjvxAudioFixedFftN1Filter.h


	)

#set(LOCAL_PCG_FILES
#	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGeneratorWave/CjvxIntegrateWaveGenerators.pcg
#	)

set(BUILD_STATIC TRUE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
