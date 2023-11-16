if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxDataLogReader_octave)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogReader
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/mexFileReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/entryMex.cpp
  )

link_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogReader
  )

  set(LOCAL_M_FILES_SOURCE_DIR "${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/m-files/mex")

set(LOCAL_LIBS jvxTDataLogReader_static)

set(BUILD_MEX_OCTAVE TRUE)

set(LOCAL_M_FILES_SOURCE_DIR "../../m-files")
set(INSTALL_TARGET_NAME_OCTAVE "jvxLogRead")
set(LOCAL_START_SCRIPT_MATLAB "../../scripts/${JVX_OS}/startLogReader_mat")
set(LOCAL_START_SCRIPT_OCTAVE "../../scripts/${JVX_OS}/startLogReader_oct")
install(PROGRAMS ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/scripts/m-files/jvx_start_rtproc.m DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT "release")

include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
