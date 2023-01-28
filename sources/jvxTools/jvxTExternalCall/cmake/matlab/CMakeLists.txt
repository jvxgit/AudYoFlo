set(PROJECT_NAME jvxTExternalCall_matlab)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${Matlab_INCLUDE_DIRS}
  )


set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxMatlabCall.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/componentEntry.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c/CjvxCToMatlabConverter.cpp
  )

link_directories(${MATLAB_PATH_LIB})

set(LOCAL_COMPILE_DEFINITIONS CONFIG_COMPILE_FOR_MATLAB "__STDC_UTF_16__")

set(LOCAL_LIBS ${JVX_SYSTEM_MATLAB_MEX_LIBRARIES})

set(IS_JVX_COMPONENT TRUE)
set(JVX_COMPONENT_ONLY_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


