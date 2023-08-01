if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNConvert)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include

  # ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-micro-connection/include
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNConvert.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNConvert.h
  )

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS  
	-I ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/pcg/jvxNodes)

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  # jvx-micro-connection_static
  jvx-dsp-cpp_static
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
