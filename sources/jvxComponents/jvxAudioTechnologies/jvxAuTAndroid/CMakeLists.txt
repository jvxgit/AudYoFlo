set(PROJECT_NAME jvxAuTAndroidGlEs)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
	)
if(NOT JVX_USE_PART_ANDROIDAUDIO)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PART_ANDROIDAUDIO_NO_GLES")
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  # cpp helper functions
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAndroidTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAndroidDevice.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAndroidTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAndroidDevice.h

  )

set(LOCAL_LIBS
	jvx-component-templates-base_static
	
	# Helpers to provide some wav string conversions
	jvx-helpers-product_static

	jvx-audiocodec-helpers_static
	
  )

# set(LOCAL_PCG_FILE_OPTIONS 
# 	-I ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/pcg)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
