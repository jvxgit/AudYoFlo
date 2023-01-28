cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-deploy-signals)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  )

set(LOCAL_SOURCES

  )

set(LOCAL_LIBS
	jvx-component-templates-base_static
  )

set(BUILD_STATIC TRUE)

set(LOCAL_PCG_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/CjvxSignalProcessingDeviceDeploy.pcg
	)

#set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
#set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
