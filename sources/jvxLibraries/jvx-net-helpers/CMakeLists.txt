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
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-net-helpers.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxPropertyStreamUdpSocket.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxPropertyStreamUdpSocket.cpp    

	${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxNetworkDevices.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxNetworkDevices.cpp
  )

 set(LOCAL_LIBS
  jvxLsockets_st_static
  )

 if(${JVX_OS} MATCHES "windows")
	# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/platform/windows)
  	set(LOCAL_SOURCES ${LOCAL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/include/platform/windows/HjvxNetworkDevices_os.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/windows/HjvxNetworkDevices_os.cpp)
	set(LOCAL_LIBS ${LOCAL_LIBS} Iphlpapi)  
endif()

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
