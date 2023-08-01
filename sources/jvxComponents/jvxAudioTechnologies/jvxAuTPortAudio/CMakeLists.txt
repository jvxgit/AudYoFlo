if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTPortAudio)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/portaudio/portaudio/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPortAudioTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPortAudioDevice.cpp

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPortAudioTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPortAudioDevice.h
  )

set(LOCAL_LIBS portaudio)

set(LOCAL_LIBS ${LOCAL_LIBS}
  jvx-component-templates-base_static
  )

if(JVX_USE_PART_COREAUDIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES})
endif()

if(JVX_USE_PART_ALSA)
  set(LOCAL_LIBS ${LOCAL_LIBS} asound)
endif()


set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
