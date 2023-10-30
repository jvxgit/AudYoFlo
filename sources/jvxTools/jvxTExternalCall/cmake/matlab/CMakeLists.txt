set(PROJECT_NAME jvxTExternalCall_matlab)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/ayf-mex-connect/include
  ${Matlab_INCLUDE_DIRS}
  )


set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxMatlabCall.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/componentEntry.cpp
  )

link_directories(${MATLAB_PATH_LIB})

set(LOCAL_COMPILE_DEFINITIONS CONFIG_COMPILE_FOR_MATLAB "__STDC_UTF_16__")

set(LOCAL_LIBS 
	ayf-mex-connect_m_static
	${JVX_SYSTEM_MATLAB_MEX_LIBRARIES})


set(IS_JVX_COMPONENT TRUE)
set(JVX_COMPONENT_ONLY_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


