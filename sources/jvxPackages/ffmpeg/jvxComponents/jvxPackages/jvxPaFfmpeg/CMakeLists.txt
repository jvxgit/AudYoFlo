set(PROJECT_NAME jvxPaFfmpeg)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated

  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include

  # ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-ffmpeg-helpers/include
  ${JVX_FFMPEG_ROOT_PATH}/jvxLibraries/jvx-ffmpeg-helpers/include

    ${JVX_FFMPEG_INCLUDE_DIR}

	#${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFFMpegReader
	#${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFFMpegWriter
	#${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioCodecs/jvxAuCFfmpeg
) 

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPaFfmpeg.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPaFfmpeg.cpp
	)


set(LOCAL_LIBS 
	jvxAuTFFMpegReader_static 
	jvxAuTFFMpegWriter_static
	jvxAuCFfmpeg_static
	${JVX_FFMPEG_SYS_LIB_SMIIDS}

	jvx-component-templates-product_static
	jvx-ffmpeg-helpers_static)

set(IS_JVX_COMPONENT TRUE)
set(BUILD_SHARED FALSE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
