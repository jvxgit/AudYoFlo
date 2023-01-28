cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTOfflineAudio)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  #${JVX_BASE_ROOT}/software/templates/jvxObjects
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  include_directories(${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include)
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOfflineAudioTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOfflineAudioDevice.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOfflineAudioTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOfflineAudioDevice.h
  )

set(LOCAL_COMPILE_DEFINITIONS "JVX_EXTERNAL_CALL_ENABLED")

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

set(LOCAL_MCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_matlab.mcg
  )

set(LOCAL_LIBS
  jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
