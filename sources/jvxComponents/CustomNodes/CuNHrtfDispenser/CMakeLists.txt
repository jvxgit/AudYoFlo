set(PROJECT_NAME CuNHrtfDispenser)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  # These two library folders for HRTF functionality
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/third_party/git/mysofa/libmysofa/src/hrtf
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/ayf-hrtf-dispenser/include
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHrtfDispenser.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxHrtfDispenser.cpp
  )

set(LOCAL_LIBS 
	jvx-component-templates-base_static
	ayf-hrtf-dispenser_static
	)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
