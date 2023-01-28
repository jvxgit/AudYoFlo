cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNOpenGl)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/templates
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNOpenGl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNOpenGl.h
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
	jvx-opengl_static
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
endif()

set(LOCAL_SOURCES ${LOCAL_SOURCES}
	${LOCAL_MCG_FILES}
	${LOCAL_PCG_FILES}
	)

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


