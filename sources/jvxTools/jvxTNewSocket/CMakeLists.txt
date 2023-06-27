set(PROJECT_NAME jvxTNewSocket)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-sockets/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNewSocket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxNewSocket.h
  )

set(LOCAL_LIBS jvx-sockets_static
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
