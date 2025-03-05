set(PROJECT_NAME jvxConfigWriter)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  ${JVX_BASE_ROOT}/sources/jvxLibraries/ayf-mex-connect/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mexConfigWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/target/entryMex.cpp
  )

link_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  )
set(LOCAL_M_FILES_SOURCE_DIR "m-files/matlab;${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/m-files/mex")

set(LOCAL_LIBS jvxTconfigProcessor_static)

set(BUILD_MEX_MATLAB TRUE)

set(JVX_RENAME_RELEASE "${PROJECT_NAME}")

set(INSTALL_TARGET_NAME_MATLAB "jvxConfigWrite")
set(LOCAL_MAT_LIBS ayf-mex-connect_m_static)
set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/startConfigWriter_mat)
set(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME jvx-cfgw-mat)
install(PROGRAMS ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/scripts/m-files/jvx_start_rtproc.m DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT "release")

if(JVX_USE_PART_MATLAB)
	set(JVX_TARGET_NAME "${PROJECT_NAME}_m")
	set(LOCAL_MAT_LIBS ayf-mex-connect_m_static)
	set(BUILD_MEX_MATLAB TRUE)
	set(BUILD_MEX_OCTAVE FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
endif()

if(JVX_USE_PART_OCTAVE)
	
	set(JVX_TARGET_NAME "${PROJECT_NAME}_o")
	set(LOCAL_OCT_LIBS ayf-mex-connect_o_static)
	set(BUILD_MEX_OCTAVE TRUE)
	set(BUILD_MEX_MATLAB FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
	if(JVX_USE_PART_MATLAB)
		add_dependencies(${PROJECT_NAME}_o ${PROJECT_NAME}_m)
	endif()
endif()



