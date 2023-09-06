set(PROJECT_NAME jvx-link-frontend)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
  
  # Console linkage
	${JVX_BINARY_DIR}/base/sources/jvxEventLoop/CjvxEStandalone
	${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleTools/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLWebConsoleTools/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-link-frontend.cpp
  )

set(LOCAL_LIBS ${LOCAL_LIBS}
	# jvxLConsoleHost-static_static <- here, we leave the involved console lib open. Then, we can link to "shared" or "static" also
	jvxLWebConsoleTools_static
	)
	
set(BUILD_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
