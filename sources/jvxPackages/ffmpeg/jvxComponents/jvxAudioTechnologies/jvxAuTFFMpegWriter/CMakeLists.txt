cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTFFMpegWriter)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
	)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  # cpp helper functions
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include

  # WAV File reader
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLFileIO/include

  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterTechnology_properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterTechnology_configuration.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterDevice.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioFFMpegWriterDevice.h

  )

set(LOCAL_LIBS
	jvx-component-templates-base_static
	
	# Helpers to provide some wav string conversions
	jvx-helpers-product_static

	# WAV File reader
	jvxLFileIO_static

	jvx-audiocodec-helpers_static
  )

  set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/pcg
	-D JVX_WAV_PROPERTIES_EDITABLE)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
