cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxConfigReader_matlab)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/mexConfigReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/entryMex.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c/CjvxCToMatlabConverter.cpp
  )

set(LOCAL_M_FILES_SOURCE_DIR "../../m-files/matlab;${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/m-files/mex")

link_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  )

set(LOCAL_LIBS jvxTconfigProcessor_static)

set(BUILD_MEX_MATLAB TRUE)

set(INSTALL_TARGET_NAME_MATLAB "jvxConfigRead")
set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/${JVX_OS}/startConfigReader_mat)
set(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME jvx-cfgr-mat)
install(PROGRAMS ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/scripts/m-files/jvx_start_rtproc.m DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT "release")

include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)



