if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-net-helpers)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLsockets_st/include
)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxPropertyStreamUdpSocket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxPropertyStreamUdpSocket.h
  )

set(LOCAL_LIBS
  jvxLsockets_st_static
  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
