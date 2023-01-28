set(PROJECT_NAME jvx-host-json)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-crc/include
	)

set(LOCAL_SOURCES
 	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonProcessCommand.h
 	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonCommandsShow.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonCommandsConfigShow.h
 	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonCommandsProperties.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonCommandsActShow.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxHostJsonDefines.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHostJsonProcessCommand.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHostJsonCommandsShow.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHostJsonCommandsProperties.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHostJsonCommandsActShow.cpp
  )


set(LOCAL_LIBS	
	jvx-json_static
	jvx-crc_static
	 )

# Include the project specific references
include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
