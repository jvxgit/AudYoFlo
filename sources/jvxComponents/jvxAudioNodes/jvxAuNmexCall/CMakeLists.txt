cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNmexCall)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )


set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioNode_mexCall.cpp
 )

set(LOCAL_LIBS jvx-component-templates-base_static)

set(LOCAL_COMPILE_DEFINITIONS JVX_EXTERNAL_CALL_ENABLED)
if(LOCAL_COMPILE_DEFINITIONS MATCHES "JVX_EXTERNAL_CALL_ENABLED")
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLexternalCall_static
	)
endif()

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# FIXME: installation of files should be configured with variables and handles by ...lib_foot.cmake
if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_PART_MATLAB)
		install(DIRECTORY matlab/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})
	endif()
endif()
