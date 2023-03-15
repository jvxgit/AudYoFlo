set(PROJECT_NAME jvxAuCPcm)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcm.h
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcm.cpp

	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcmDecoder.h
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcmDecoder.cpp

	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcmEncoder.h
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCPcmEncoder.cpp

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include/jvxAudioCodecs/CjvxAudioCodec.h
	)


set(LOCAL_LIBS jvx-component-templates-product_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
