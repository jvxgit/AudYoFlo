set(PROJECT_NAME jvxTMswText2Speech)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMSWText2Speech.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_windows_extracted_helpers.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMSWText2Speech.h
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS jvxLthreads_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
