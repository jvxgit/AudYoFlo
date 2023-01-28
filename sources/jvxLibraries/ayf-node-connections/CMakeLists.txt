cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME ayf-node-connections)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
#  add_custom_target(jvx-vq-doc ALL
#    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
#    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
#    )
#endif(DOXYGEN_FOUND)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src

   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
   ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-micro-connection/include
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxAuNSubChain.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfConnection.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAuNConnection.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAuNFixedConnection.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAuNFlexibleConnection.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfConnection.cpp  
)

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/CayfAuNSubChain.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/CayfAuNSubChainIf.pcg)

set(LOCAL_LIBS 
	jvx-component-templates-base_static
	jvx-micro-connection_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
