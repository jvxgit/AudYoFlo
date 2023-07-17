cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxSpNMixChainEnterLeave)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_common.h

	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_output.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_config.cpp
   )

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  )

  
set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
	)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
