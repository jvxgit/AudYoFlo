if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-plugin-connector)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-plugin-connector/src

  ${JVX_BASE_BINARY_DIR}/sources/jvxFactoryHosts/jvxFHAppFactoryHostCon
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-micro-connection/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include

   ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-node-connections/include

  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTGlobalLock/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreads/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
  
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTDataConverter/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTResampler/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileReader/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxTools/jvxTrtAudioFileWriter/
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPluginProcessor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxPluginProcessor.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPluginCommon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxPluginCommon.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/TjvxPluginCommon.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/IjvxPluginCommon.h
  )

set(LOCAL_LIBS 
	jvxFHAppFactoryHostCon_static_nd
	jvxTconfigProcessor_static
	jvxTSystemTextLog_static
	jvxTThreads_static
	jvxTDataLogger_static
	jvxTGlobalLock_static
	jvxTDataConverter_static
	jvxTResampler_static
	jvxTrtAudioFileReader_static
	jvxTrtAudioFileWriter_static
    jvx-micro-connection_static
	ayf-node-connections_static
    jvx-helpers_static)
	
set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
