if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxWebControlApp)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
  )

include_directories(
  ${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLWebConsoleTools/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BINARY_DIR}/base/sources/jvxTools/jvxTWebServer
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	jvxLConsoleHost-shared_static
	jvxLWebConsoleTools_static
	)

set(JVX_PREPARE_WORKSPACE_VS TRUE)
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
