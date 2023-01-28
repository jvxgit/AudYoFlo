cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxTWebServer)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

#set(JVX_CIVETWEB_ROOT ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/civetweb)
set(JVX_CIVETWEB_ROOT ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/civetweb/civetweb)
# ${CMAKE_CURRENT_SOURCE_DIR}/extern/civetweb

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_webserver.pcg
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_CIVETWEB_ROOT}/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLsockets_st/include
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebServer.cpp
	${JVX_CIVETWEB_ROOT}/src/CivetServer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebServer.h
	${JVX_CIVETWEB_ROOT}/include/CivetServer.h
  )

if(JVX_CIVETWEB_INCLUDED_AS_SOURCE)

	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${JVX_CIVETWEB_ROOT}/src/civetweb.c
		${JVX_CIVETWEB_ROOT}/include/civetweb.h)
	set_source_files_properties(${JVX_CIVETWEB_ROOT}/src/civetweb.c PROPERTIES LANGUAGE CXX)
	#set(LOCAL_COMPILE_DEFINITIONS "USE_WEBSOCKET;DEBUG;DEBUG_TRACE=printf;DEBUG_TRACE_FUNC=printf;JVX_WEBSERVER_VERBOSE_MODE")
	set(LOCAL_COMPILE_DEFINITIONS "USE_WEBSOCKET")
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};OPENSSL_API_1_1")
endif()

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
	${JVX_SOCKET_LIBRARIES}
	jvxLsockets_st_static
	## jvxTSocket_static
)

if(NOT JVX_CIVETWEB_INCLUDED_AS_SOURCE)
	set(LOCAL_LIBS ${LOCAL_LIBS} civetweb-c-library)
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
