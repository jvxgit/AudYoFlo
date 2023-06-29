cmake_minimum_required (VERSION 2.8.11)

set(PROJECT_NAME jvxLTextDeviceControl)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxFlexibleTextControlDevice.cpp
  ## ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxFlexibleRs232TextDevice.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxFlexibleTextControlDevice.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxFlexibleConnectionTextDevice.h

  ${CMAKE_CURRENT_SOURCE_DIR}/pcgf/CjvxFlexibleTextControlDevice.pcg
  )

set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/pcgf/CjvxFlexibleTextControlDevice.pcg)

set(LOCAL_PCG_FILE_OPTIONS
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	)
set(LOCAL_LIBS jvx-component-templates-base_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
