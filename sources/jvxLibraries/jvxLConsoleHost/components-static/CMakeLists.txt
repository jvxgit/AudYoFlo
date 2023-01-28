cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLConsoleHost-static)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../target/main.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
	${JVX_BASE_BINARY_DIR}/sources/jvxEventLoop/CjvxEStandalone
  ${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHJvx/
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
    ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include

  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	jvxEStandalone_static
	jvxLConsoleTools-static_static
	)

set(BUILD_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
