set(PROJECT_NAME sv56Demo)
include(${JVX_CMAKE_DIR}/common_local_head.cmake)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/sv56demo.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sv-p56.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ugst-utl.c
 )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
