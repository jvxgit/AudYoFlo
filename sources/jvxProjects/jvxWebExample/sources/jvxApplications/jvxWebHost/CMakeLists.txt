if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxWebHost)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/jvx_hooks.cpp
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLWebConsoleTools/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
   ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
    ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTGlobalLock/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTWebServer
  ${JVX_SUBPRODUCT_EXAMPLE_LIBS_BINARY_DIR}/sources/jvxComponents/jvxCustomTechnologies/jvxCuTExample
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	jvxLConsoleHost-static_static
	jvxLWebConsoleTools_static

	jvxTconfigProcessor_static
	jvxTSystemTextLog_static
	jvxTWebServer_static
	jvxCuTExample_static

	)

# set(LOCAL_START_SCRIPT "start-jvxWebHost")

set(JVX_PREPARE_WORKSPACE_VS TRUE)
set(CONFIGURE_LOCAL_START_SCRIPT true)

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
