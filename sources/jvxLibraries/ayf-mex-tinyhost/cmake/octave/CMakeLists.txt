set(PROJECT_NAME ayf-mex-tinyhost_o)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src

  ${JVX_BASE_ROOT}/sources/jvxLibraries/ayf-mex-connect/include
	
  ${Octave_INCLUDE_DIRS}
  
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/CayfTinyMexComponentHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/CayfTinyMexComponentHost.cpp  
)

set(LOCAL_LIBS
	ayf-mex-connect_o_static
	jvxTconfigProcessor_static)
	
set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
