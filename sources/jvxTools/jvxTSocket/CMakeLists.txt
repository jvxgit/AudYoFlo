set(PROJECT_NAME jvxTSocket)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLsockets_st/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocket_client_cb.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocket_cb.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSocket_server_cb.cpp
  )

set(LOCAL_LIBS
  jvxLsockets_st_static
  jvxLthreads_static
  )
set(IS_JVX_COMPONENT TRUE)

if(JVX_USE_PCAP)
  set(LOCAL_COMPILE_DEFINITIONS "JVX_WITH_PCAP")

  if(${JVX_OS} MATCHES "windows")
		if(NOT JVX_RELEASE_SDK)
			jvx_install_dedicated_file("C:/Windows/System32/Npcap/Packet.dll" ${INSTALL_PATH_BIN})
			jvx_install_dedicated_file("C:/Windows/System32/Npcap/wpcap.dll" ${INSTALL_PATH_BIN})
		endif()
	endif()
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
