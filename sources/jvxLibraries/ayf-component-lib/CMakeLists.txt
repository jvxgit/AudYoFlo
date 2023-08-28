if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME ayf-component-lib)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  # ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-shared-connection-host/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-embedding-proxy/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-embedding-proxy/include/${JVX_OS}
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfComponentLib.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfComponentLib.cpp  
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfComponentLib_process.cpp 
)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/CayfComponentLib.pcg)

set(LOCAL_LIBS 

	# Both libraries loaded at runtime via LOADLIBRARY:
	# ayf-shared-connection-host_import
	# ayf-embedding-proxy_import
	)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
#set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
