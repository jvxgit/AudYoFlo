set(PROJECT_NAME jvxReadMat)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(${Matlab_INCLUDE_DIRS})

set(LOCAL_LIBS ${JVX_SYSTEM_MATLAB_MEXEX_LIBRARIES})

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../target/main.cpp
  # ${JVX_BASE_ROOT}/software/codeFragments/matlab_c/CjvxCToMatlabConverter.cpp
  )

set(LOCAL_LIBS ${JVX_SYSTEM_MATLAB_MEXEX_LIBRARIES})

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
