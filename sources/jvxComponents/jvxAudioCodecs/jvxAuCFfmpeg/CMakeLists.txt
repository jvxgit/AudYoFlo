set(PROJECT_NAME jvxAuCFfmpeg)
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
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-ffmpeg-helpers/include

    ${JVX_FFMPEG_INCLUDE_DIR}
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpeg.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpeg.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpegEncoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpegEncoder.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpegDecoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuCFfmpegDecoder.cpp

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include/jvxAudioCodecs/CjvxAudioCodec.h
	)


set(LOCAL_LIBS 
	jvx-component-templates-product_static
	jvx-ffmpeg-helpers_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
