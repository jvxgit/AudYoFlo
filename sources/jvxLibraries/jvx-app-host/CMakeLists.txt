if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-app-host)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHAppHost/
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxAppHostBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxAppHostBase_file.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxAppHostBase.h
  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
