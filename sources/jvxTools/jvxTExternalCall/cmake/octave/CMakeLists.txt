if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxTExternalCall_octave)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  ${OCTAVE_INCLUDE_PATH}
  )


set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxMatlabCall.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../target/componentEntry.cpp
  )

link_directories(${OCTAVE_PATH_LIB})

set(LOCAL_COMPILE_DEFINITIONS CONFIG_COMPILE_FOR_OCTAVE)

set(LOCAL_LIBS ${JVX_SYSTEM_OCTAVE_MEX_LIBRARIES})

set(IS_JVX_COMPONENT TRUE)
set(JVX_COMPONENT_ONLY_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
