if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME_ROOT jvxSpNAsyncIo)
set(PROJECT_NAME jvxSpNAsyncIo)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack/include

   ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  )


set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSPAsyncIo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSPAsyncIo_sync.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSPAsyncIo.h
 )

 set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg)

set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	-I ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/pcg/jvxLogToFile)

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
  jvx-component-templates-base_static
  jvx-audio-stack_static)

set(LOCAL_COMPILE_DEFINITIONS_COMMON "${LOCAL_COMPILE_DEFINITIONS}")

# Project with async module operation with master at primary side
set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}_pm")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}_pm_static")
set(JVX_TARGET_NAME_IMPORT "${PROJECT_NAME_ROOT}_pm_import")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COMMON};JVX_ASYNCIO_MASTER_PRI")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# Project with async module operation with master at secondary side
set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}_sm")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}_sm_static")
set(JVX_TARGET_NAME_IMPORT "${PROJECT_NAME_ROOT}_sm_import")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COMMON}")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# FIXME: installation of files should be configured with variables and handles by ...lib_foot.cmake
#if(NOT JVX_RELEASE_SDK)
#	if(JVX_USE_PART_MATLAB)
#		install(DIRECTORY matlab/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})
#	endif()
#endif()
