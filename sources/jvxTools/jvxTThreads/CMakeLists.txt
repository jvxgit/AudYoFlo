if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxTThreads)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/target
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
)

set(LOCAL_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxThreads.cpp
 )

set(LOCAL_LIBS jvxLthreads_static)

set(IS_JVX_COMPONENT TRUE)

# this changes the name of the output to be followed by "_import"
# It does NOT produce an import lib in the SDK for all projects which
# are compiled to form a runtime
set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

# This allows to copy import lib (.lib file) to sdk also
set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
