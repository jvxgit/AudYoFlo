set(PROJECT_NAME ayf-mex-connect_m)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src

  ${Matlab_INCLUDE_DIRS}
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/../../include/CjvxCToMatlabConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/CjvxPropertiesToMatlabConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/CjvxMatlabToCConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/localMexIncludes.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxCToMatlabConverter.cpp  
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxPropertiesToMatlabConverter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/CjvxMatlabToCConverter.cpp
)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
