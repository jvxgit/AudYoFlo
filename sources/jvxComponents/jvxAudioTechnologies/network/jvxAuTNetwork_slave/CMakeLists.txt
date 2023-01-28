cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTNetwork_slave)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLsockets_st/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-dsp/generated
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/
)

  set(LOCAL_PCG_FILE_OPTIONS
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-cpp/pcg/jvxLogToFile
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxConnectNetwork
	-D JVX_ALL_AUDIO_TO_LOGFILE)
	
set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/CjvxNetworkTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice_helpers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice_link.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice_mqueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice_threads.cpp

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/CjvxNetworkTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveDevice.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNetworkSlaveTechnology.h
  )

set(LOCAL_COMPILE_DEFINITIONS "JVX_PROJECT_NAMESPACE=${PROJECT_NAME}")

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
  )

if(JVX_USE_PCAP)
  set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_WITH_PCAP")
endif()

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
  jvxLsockets_st_static
  jvxLthreads_static
  jvx-dsp-base-cpp_static
  jvx-dsp_static
  jvx-component-templates-base_static
  )
  
if(JVX_USE_PART_MATLAB OR JVX_USE_PART_OCTAVE) 
	set(LOCAL_M_FILES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/m-files/matlab")
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
