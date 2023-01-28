cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTIos)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${CMAKE_CURRENT_SOURCE_DIR}/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/emb-audio-host
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxIosAudioTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxIosAudioDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxIosAudioDevice_ios.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/jvxAudioProcessor.mm
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioProcessor_cpp.mm
  )

set(LOCAL_LIBS
  ${FOUNDATION} ${AVFOUNDATION} ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES}
  jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_COMPILE_DEFINITIONS "JVX_IOS_COMPILE_RTPROC_FWK")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
