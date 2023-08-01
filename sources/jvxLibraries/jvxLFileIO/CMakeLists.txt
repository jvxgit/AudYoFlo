if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxLFileIO)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${SNDFILE_INCLUDEDIR}
)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxFileReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxFileWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxWavReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxWavWriter.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxFileReader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxFileWriter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxWavReader.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxWavWriter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxWavCommon.h
  )

set(BUILD_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
