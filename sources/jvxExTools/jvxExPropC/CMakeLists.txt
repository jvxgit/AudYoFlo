cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxExPropC)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(LOCAL_SOURCES
  ${JVX_BASE_ROOT}/software/codeFragments/parser/entryParserTools_out_h1.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textProcessor_core.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textProcessor_helpers_c.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/textProcessor_helpers_mat.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/src/textProcessor_core.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/scan_token_defines.h
)

set(LOCAL_LIBS
 jvxTconfigProcessor_static
)

force_console_app()
set(LOCAL_COMPILE_DEFINITIONS "JVX_PRODUCT_CODEGEN_TAG=\"${JVX_PRODUCT}_\"")
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)

set(JVX_TARGET_NAME jvxExPropMat)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROP_MATLAB_OUTPUT")
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
