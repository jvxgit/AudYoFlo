set(PROJECT_NAME jvxTRemoteCall)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-remote-call/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-crc/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxRemoteCall.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxRemoteCall_cb.cpp
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
	jvx-remote-call_static
	jvx-crc_static
	)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
