cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
  include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNSynthesizer)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-audiotools-cpp/include
  ${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-audiotools/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  ${CMAKE_CURRENT_BINARY_DIR}/../../../jvxLibraries/jvx-audiotools-cpp/generated/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNSynthesizer.cpp)


set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

# FIXME: to be handled via LOCAL_MATPCG_FILES variable
if(JVX_MATLAB_PROPERTY_GENERATOR)
  jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}" "${LOCAL_PCG_FILE_OPTIONS}")
endif()

set(LOCAL_LIBS
  jvx-audiotools-cpp_static
  jvx-audiotools_static
  jvx-dsp-cpp_static
  jvx-dsp_static
  jvx-dsp-base_static
  jvx-component-templates-base_static
  ${FFT_LIBRARIES}
  )

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
