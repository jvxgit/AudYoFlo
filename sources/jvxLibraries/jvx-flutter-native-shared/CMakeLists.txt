cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-flutter-native-shared)
set(JVX_AUTOMATION_VERBOSE FALSE)
set(JVX_LOCAL_AUTOMATION_ON_START FALSE)
set(AYFBINRENDER_WEBSERVER TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-lib-host-app/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static/include
  )


  # Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES 	

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-flutter-native.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-flutter-forward.cpp
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static/exports/${JVX_OS}/exports-flutter-native.def
  )

set(LOCAL_LIBS 
	ayfAuNBinRenderMixer_static
	jvxSpNAsyncIo_sm_static
	jvxAuTFileReader_static
	jvxAuTFileWriter_static
	jvxAuCPcm_static
	jvxAuNForwardBuffer_static
	jvxAuNBitstreamDecoder_static
	jvxAuNBitstreamEncoder_static
	jvxAuNChannelRearrange_static
	jvxAuNBinauralRender_static
	jvx-flutter-native-static_static)

include_directories(
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
	${JVX_BINARY_DIR}/base/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTWindows
	${JVX_SUBPRODUCT_AYFBINRENDER_LIBS_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/ayfAuNBinRenderMixer/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNBinauralRender/
	${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNAsyncIo
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFileReader
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFileWriter
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioCodecs/jvxAuCPcm/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamDecoder/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamEncoder/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNForwardBuffer/
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNChannelRearrange/
	)

if(JVX_LOCAL_AUTOMATION_ON_START)
	include_directories({JVX_BASE_ROOT}/sources/jvxLibraries/jvx-automate/include)

	set(LOCAL_SOURCES ${LOCAL_SOURCES}

		${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ayfbinrender-defines.h
		${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ayfbinrender-automation.h
		${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ayfbinrender-automation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ayfbinrender-automation-inputfiles.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ayfbinrender-automation-outputfiles.cpp)

	set(LOCAL_LIBS ${LOCAL_LIBS} jvx-automate_static)

else()

	include_directories(${JVX_SUBPRODUCT_AYFBINRENDER_LIBS_BINARY_DIR}/sources/jvxComponents/jvxAutomation/ayfATBinRender/)
	set(LOCAL_LIBS ${LOCAL_LIBS} ayfATBinRender_static)

endif()

if(JVX_AUTOMATION_VERBOSE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_AUTOMATION_VERBOSE")
endif()

# Specify the max number of devices to be mixed during processing per technology. Here we can use inf
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_NUMBER_AUDIO_DEVICES_MIX=JVX_SIZE_UNSELECTED")
## 

if(AYFBINRENDER_WEBSERVER)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};AYFBINRENDER_WEBSERVER")
	include_directories(
		${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAppCtrNodes/jvxAppNWeb/
		${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTWebServer)
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAppNWeb_static jvxTWebServer_static)

endif()

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

## ======================================================================

set(BUILD_SHARED TRUE)
set(BUILD_STATIC FALSE)

# this changes the name of the output to be followed by "_import"
# It does NOT produce an import lib in the SDK for all projects which
# are compiled to form a runtime
set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

# This allows to copy import lib (.lib file) to sdk also
set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# Install the MSVC system libraries
if(JVX_OS MATCHES "windows")
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${INSTALL_PATH_BIN}")
	include(InstallRequiredSystemLibraries)
endif()

# 
# Befehl				=	O:\ayfdevelop\AudYoFlo\flutter\ayfbinrender\ayfbinrender\build\windows\runner\Debug\ayfBinRender.exe
# Befehlsargumente		=   --workdir C:\jvxrt\audio-sdk\release\sdk --preload C:\jvxrt\audio-sdk\release\sdk\bin\zlib.dll --preload C:\jvxrt\audio-sdk\release\sdk\bin\libfftw3-3.dll --jvxdir jvxComponents --config ayfbinrender.jvx --textlog --verbose_dll --textloglev 10
# Arbeitsverzeichnis	=   C:\jvxrt\audio-sdk\release\sdk
#
