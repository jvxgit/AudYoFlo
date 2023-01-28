set(PROJECT_NAME jvxEStandalone)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/target
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxEStandalone.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxEStandalone.h
  )

#set(LOCAL_PCG_FILES
#  ${JVX_BASE_ROOT}/software/templates/jvxHosts/CjvxHost_pcg.pcg
#  )

set(LOCAL_LIBS jvxLthreads_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

