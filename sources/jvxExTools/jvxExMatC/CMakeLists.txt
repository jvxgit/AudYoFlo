set(PROJECT_NAME jvxExMatC)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories(
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(LOCAL_SOURCES
  ${JVX_BASE_ROOT}/software/codeFragments/parser/entryParserTools_out_h2.cpp
  src/textProcessor_core.cpp
  src/textProcessor_genC.cpp
)

set(LOCAL_LIBS jvxTconfigProcessor_static)

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
