cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuTGenericWrapper)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_props.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_config.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_fileio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_setup.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_prepare.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_hostRelocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_start.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_prepare_subfunctions.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperTechnology_hostRelocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperTechnology_callbacks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCircularBuffer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice_hostRelocator.h
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioDevice.h
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxAudioTechnologies/CjvxAudioTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperDevice.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxGenericWrapperTechnology_hostRelocator.h
  )

if(JVX_MATLAB_PROPERTY_GENERATOR)
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_DEVICE" "dev" "${LOCAL_PCG_FILES}")
endif()

set(LOCAL_LIBS jvx-dsp-base_static 
	jvxLthreads_static  
	jvx-component-templates-base_static
	)

link_directories(${FFT_LIBDIR})

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
