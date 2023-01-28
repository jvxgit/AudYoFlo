cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
  include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNSynthesizer)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../../jvxLibraries/jvx-dsp-cpp/generated/
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNSynthesizer.cpp)


set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

# FIXME: to be handled via LOCAL_MATPCG_FILES variable
if(JVX_MATLAB_PROPERTY_GENERATOR)
  jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}")
endif()

set(LOCAL_LIBS
  jvx-dsp-cpp_static
  jvx-dsp_static
  jvx-dsp-base_static
  jvx-component-templates-base_static)

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
