if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLConsoleHostApp-shared)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../target/main.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleHost/include
  #${JVX_BASE_BINARY_DIR}/sources/jvxEventLoop/CjvxEStandalone
  #${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHAppHost/
  #${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
  #${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleHost/include
  #${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	jvxLConsoleHost-shared_static
	)

set(BUILD_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
