jvx_exclude_platform("Emscripten")

set(PROJECT_NAME jvxConsoleApp)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
  )

include_directories(
  ${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
  ${JVX_BASE_BINARY_INCLUDE_PATH}/jvxLConsoleTools/components-static/generated
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleHost/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include

   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
	jvxLConsoleHostApp-shared_static
	)

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
