cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNMcas)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_MCAS_LOCAL_LIBS_INCLUDE_DIR}
 ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/templates
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

# Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNMcas.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNMcas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/playControl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/playControl.cpp
  )


set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

if(JVX_MATLAB_PROPERTY_GENERATOR) 
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}")
endif()

set(LOCAL_LIBS 
	jvx-dsp_static
	jvx-component-templates-base_static
	)
	
set(LOCAL_COMPILE_DEFINITIONS JVX_EXTERNAL_CALL_ENABLED)
if(LOCAL_COMPILE_DEFINITIONS MATCHES "JVX_EXTERNAL_CALL_ENABLED")
  include_directories(${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include)
  set(LOCAL_MCG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/export_project.mcg
    )
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLexternalCall_static
	)
	source_group("mcg" FILES "${LOCAL_MCG_FILES}")
endif()


set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

# Group pcg files in subfolders of VS project/solution
source_group("pcg" FILES "${LOCAL_PCG_FILES}")
source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


