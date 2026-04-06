set(PROJECT_NAME jvxAuTAndroid)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
	)

# if(NOT JVX_USE_PART_ANDROIDAUDIO)
if(NOT ${JVX_OS} MATCHES "Android")
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PART_ANDROIDAUDIO_NO_API")
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

set(LOCAL_SOURCES ${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAndroidTechnology_jni.cpp)

# On Android, fetch Oboe and link it together with the logging library
if(${JVX_OS} MATCHES "Android")

	

	include(FetchContent)
	FetchContent_Declare(
		oboe
		GIT_REPOSITORY https://github.com/google/oboe.git
		GIT_TAG        1.9.3
	)
	FetchContent_MakeAvailable(oboe)

	list(APPEND LOCAL_LIBS oboe log)
	include_directories(${oboe_SOURCE_DIR}/include)
endif()

# set(LOCAL_PCG_FILE_OPTIONS 
# 	-I ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/pcg)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
