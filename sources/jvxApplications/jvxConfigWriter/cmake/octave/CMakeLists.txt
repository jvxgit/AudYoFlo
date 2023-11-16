if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxConfigWriter_octave)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/mexConfigWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/entryMex.cpp
  )

  link_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  )
  
 set(LOCAL_M_FILES_SOURCE_DIR "${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/m-files/mex")

set(LOCAL_LIBS jvxTconfigProcessor_static)

set(BUILD_MEX_OCTAVE TRUE)

set(INSTALL_TARGET_NAME_OCTAVE "jvxConfigWrite")
set(LOCAL_START_SCRIPT startConfigWriter_oct)
install(PROGRAMS ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/scripts/m-files/jvx_start_rtproc.m DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT "release")

include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
