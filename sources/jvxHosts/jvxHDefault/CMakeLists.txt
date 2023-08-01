if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxHDefault)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates/include
  
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )


set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  )

set(LOCAL_COMPILE_DEFINITIONS_SHARED "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-DLL")
set(LOCAL_COMPILE_DEFINITIONS_STATIC "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-LIB")

set(LOCAL_LIBS jvxLthreads_static jvx-component-templates-base_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
