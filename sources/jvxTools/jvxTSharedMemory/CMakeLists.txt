if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxTSharedMemory)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/target
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLSharedMemory/include
)

set(LOCAL_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSharedMemory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSharedMemory.h
 )

# set(LOCAL_LIBS jvxLthreads_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
