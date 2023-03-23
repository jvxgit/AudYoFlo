cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxTrtAudioFileReader)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_PATH}/software/templates/jvxObjects
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLFileIO/include
  ${SNDFILE_INCLUDEDIR}
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxRtAudioFileReader.cpp
  )

set(LOCAL_LIBS jvxLFileIO_static) # FLAC vorbis ogg vorbisenc vorbisfile

 link_directories(${SNDFILE_LIBDIR})

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
