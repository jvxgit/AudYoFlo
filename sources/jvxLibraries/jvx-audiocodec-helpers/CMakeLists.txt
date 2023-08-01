if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-audiocodec-helpers)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
  )

set(LOCAL_SOURCES
  
${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_audiocodec_helpers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_audiocodec_helpers.h  )
  
set(LOCAL_LIBS
  )
  
set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
#if(JVX_RELEASE_SDK)
#	install(DIRECTORY ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-product/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/jvx-helpers-product COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
#endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
