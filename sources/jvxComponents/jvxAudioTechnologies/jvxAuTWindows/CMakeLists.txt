if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTWindows)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsTechnology_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsDevice_api.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioWindowsDevice.h

  )

set(LOCAL_LIBS
	avrt
	jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
