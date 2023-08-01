if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLsockets_st)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

if(JVX_USE_PCAP)
include_directories(
  ${PCAP_INCLUDEDIR}
  )
  set(LOCAL_COMPILE_DEFINITIONS "JVX_WITH_PCAP")
endif()

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connect_client_st.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connect_server_st.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_connect_client_st.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_connect_server_st.h
  )

if(JVX_USE_PCAP)
set(LOCAL_LIBS
   ${PCAP_LIBRARIES}
   ${PCAPPA_LIBRARIES}
    ${JVX_SOCKET_LIBRARIES}
   )
else()
   set(LOCAL_LIBS
    ${JVX_SOCKET_LIBRARIES}
   )
endif()

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
