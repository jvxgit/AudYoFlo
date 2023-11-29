if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNMixMatrix)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-mixmatrix-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/templates
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNMixMatrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNMixMatrix.h
  )


set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

if(JVX_MATLAB_PROPERTY_GENERATOR)
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}" "${LOCAL_PCG_FILE_OPTIONS}")
endif()

set(LOCAL_LIBS
  jvx-mixmatrix-cpp_static
  jvx-mixmatrix_static
  jvx-dsp_static
  jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
