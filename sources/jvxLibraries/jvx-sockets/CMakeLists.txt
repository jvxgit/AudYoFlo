if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

jvx_exclude_platform("Emscripten")

set(PROJECT_NAME jvx-sockets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
#	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
)

if(JVX_USE_PCAP)
include_directories(
  ${PCAP_INCLUDEDIR}
  )
  set(LOCAL_COMPILE_DEFINITIONS "JVX_WITH_PCAP")
endif()

# There is a windows implementation of the tcp/udp loop. The difference is that the linux loop 
# can be stopped by shutting down the socket but this seems to not work under certain conditions in Windows.
# I do not exactly know when the linux version works for Windows and when not.
# However, I switch on the Windows implementation as the windows default now since it works in a more reliable way!
if(JVX_OS MATCHES "windows")
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_WINDOWS_SOCKET_IMPL")
endif()

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsCommon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientFactory.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientInterface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientInterfaceUnix.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientInterfaceTcp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientInterfaceTcpUdp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsPcap.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsConnection.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSockets.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsCommon.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsApi.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsServer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsConnection.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsInterfaceTcpUdp.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientInterface.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientInterfaceUnix.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientInterfaceTcpUdp.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientInterfaceTcp.h
  )

if(JVX_USE_PCAP)
	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsPcap.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsPcap.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocketsClientInterfacePcap.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSocketsClientInterfacePcap.h)
	set(LOCAL_LIBS
		${PCAP_LIBRARIES}
		${PCAPPA_LIBRARIES})
endif()
set(LOCAL_LIBS ${LOCAL_LIBS} ${JVX_SOCKET_LIBRARIES})

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
