if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTAlsa)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${ASOUND_INCLUDEDIR}
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAlsaTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAlsaDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAlsaDevice_alsa.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAlsaTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAlsaDevice.h

  )

link_directories(${ASOUND_LIBDIR})
set(LOCAL_LIBS
  ${ASOUND_LIBRARIES}
  jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
