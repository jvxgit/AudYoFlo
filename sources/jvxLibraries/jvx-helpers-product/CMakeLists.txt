if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-helpers-product)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-product/include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_SUBPRODUCT_ROOT}
  )

set(LOCAL_SOURCES
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxAudioFileIo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxGenericWrapperApi.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxAudioFileIo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxGenericWrapperApi.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-helpers-product.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/CjvxRtOutputWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/CjvxRtOutputWriter.h
  )
  
set(LOCAL_LIBS
  jvx-helpers_static
  )
  
set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
if(JVX_RELEASE_SDK)
	install(DIRECTORY ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-product/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/jvx-helpers-product COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
