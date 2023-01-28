cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-app-host-static)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_BINARY_DIR}/sources/jvxHosts/jvxHJvx/
 ${JVX_BASE_BINARY_DIR}/sources/jvxFactoryHosts/jvxFHDefault
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxAppHostBase.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxAppHostBase.h

	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxAppFactoryHostBase_file.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxAppFactoryHostBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxAppFactoryHostBase.h

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/src/CjvxAppHostProduct.cpp
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include/CjvxAppHostProduct.h

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/src/CjvxAppFactoryHostProduct.cpp
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include/CjvxAppFactoryHostProduct.h

  )

# Generate header file for configuration setup of library
if(JVX_RELEASE_SDK OR JVX_USES_SDK)
	if(NOT JVX_NO_WRITE_SDK)
		set(JVX_CONFIGURE_HOST_STATIC_NODE TRUE)
	endif()
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../include/jvx-app-host-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx-app-host-config.h)
set(LOCAL_COMPILE_DEFINITIONS "JVX_HOST_USE_ONLY_STATIC_OBJECTS;JVX_CONFIGURE_HOST_STATIC_NODE")
set(LOCAL_LIBS 
	jvxHJvx_static_nd
	jvxFHDefault_static_nd)
	
set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

if(JVX_RELEASE_SDK OR JVX_USES_SDK)
	if(NOT JVX_NO_WRITE_SDK)
	       message("---> Copy all header files from folder <${CMAKE_CURRENT_SOURCE_DIR}/../include/> to <${INSTALL_PATH_INCLUDE_STATIC}/${INSTALL_DEDICATED_INCLUDE_TREE_SFOLFER_SUFFIX}> for ${INSTALL_COMPONENT}") 
	       message("---> Copy all header files from folder <${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include/> to <${INSTALL_PATH_INCLUDE_STATIC}/${INSTALL_DEDICATED_INCLUDE_TREE_SFOLFER_SUFFIX}> for ${INSTALL_COMPONENT}") 
	       install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
	       install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
	       install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jvx-app-host-config.h DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${PROJECT_NAME}/include COMPONENT ${INSTALL_COMPONENT})
	endif()
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
