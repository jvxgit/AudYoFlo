set(PROJECT_NAME jvxLWebConsoleTools)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	    ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
		${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHJvx/
		${JVX_INTEGRATE_SOURCES_PATH}/software/codeFragments/jvxHosts/common
		${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTWebServer
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include

		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
  )

set(LOCAL_SOURCES
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebControl_fe.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxWebControl_fe.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxBinaryWebSockets.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxBinaryWebSockets.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxTextWebSockets.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxTextWebSockets.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxWebControl_fe_types.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebControl_fe_private.h
  )

#set(LOCAL_PCG_FILES
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_mexcall.pcg
#  )

set(LOCAL_LIBS
	jvx-net-helpers_static
	jvxTWebServer_static
	)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
