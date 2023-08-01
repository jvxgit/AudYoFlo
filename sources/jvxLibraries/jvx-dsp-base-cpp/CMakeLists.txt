if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-dsp-base-cpp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMultiChannelCircBufferPlotResampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMaStatAnalysis.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxLogSignalsToFile.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_qsort_ext.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_qsort_ext.h
  )

set(LOCAL_LIBS
  jvxLthreads_static
  jvx-dsp-base_static
  )

set(BUILD_STATIC TRUE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
