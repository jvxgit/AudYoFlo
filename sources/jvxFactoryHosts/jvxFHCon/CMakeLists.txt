cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxFHCon)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${JVX_BASE_ROOT}/sources/jvxFactoryHosts/jvxFHDefault/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxConnectionFHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConnectionFHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	)
set(LOCAL_LIBS 
	jvxFHDefault_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

set(JVX_TARGET_NAME ${JVX_TARGET_NAME}_nd)
set(JVX_TARGET_NAME_STATIC ${JVX_TARGET_NAME_STATIC}_nd)
set(JVX_TARGET_NAME_IMPORT ${JVX_TARGET_NAME_IMPORT}_nd)
set(LOCAL_LIBS jvxFHDefault_static_nd)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
