cmake_minimum_required (VERSION 2.8.11)

set(PROJECT_NAME jvx-subqueue)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxSubQueue.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSubQueue.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSubQueue_hooks.cpp
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

	 ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-micro-connection/include
  )

set(LOCAL_PCG_FILES
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/CjvxSubQueue_pcg.pcg
 )

#set(LOCAL_PCG_FILE_OPTIONS 
#	-I ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxDevices
#	)

## Add some dependencies which cmake seems not to find itsel (pcg -> header -> other header) 
## add_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNodeBase1io.h ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNode_pcg.pcg)
	
set(LOCAL_LIBS
	jvx-component-templates-base_static
	jvx-micro-connection_static
	)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
