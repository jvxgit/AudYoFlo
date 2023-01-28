cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxCuTExample)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_SUBPRODUCT_EXAMPLE_LIBS_INCLUDE_PATH}/jvx-example/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxExampleTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxExampleDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxExampleDevice_callbacks.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxExampleTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxExampleDevice.h
  )

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_example.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS
	-I ${JVX_SUBPRODUCT_EXAMPLE_LIBS_INCLUDE_PATH}/jvx-example/pcg/jvx-example)

set(LOCAL_LIBS 
	jvx-example_static
	jvx-component-templates-base_static
	jvxLthreads_static
	)

if(JVX_CROSS_COMPILE)
  # set(LOCAL_COMPILE_DEFINITIONS "JVX_CROSS_COMPILE;JVX_INSPIRE_DEVICE_DEBUG")
  #set(LOCAL_COMPILE_DEFINITIONS "JVX_CROSS_COMPILE;JVX_INSPIRE_SIMULATE_OSZI")
  set(LOCAL_COMPILE_DEFINITIONS "JVX_CROSS_COMPILE")
else()
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_EXAMPLE_SIMULATE")
  # set(LOCAL_COMPILE_DEFINITIONS "JVX_NO_FAIL_IF_DRIVER_UNAVAIL")
endif()

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
