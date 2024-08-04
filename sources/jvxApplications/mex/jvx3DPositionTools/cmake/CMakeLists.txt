if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx3DPositionTools)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
	${JVX_BASE_ROOT}/sources/jvxLibraries/ayf-mex-connect/include	
	)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../target/entryMex.cpp
  )

set(JVX_RENAME_RELEASE "${PROJECT_NAME}")

if(JVX_USE_EIGEN)
	include_directories(${EIGEN_INCLUDEDIR})
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_EIGEN")
endif()

# message(FATAL_ERROR "->${EIGEN_INCLUDEDIR}")
if(JVX_USE_PART_MATLAB)
	set(JVX_TARGET_NAME "${PROJECT_NAME}_m")
	set(LOCAL_MAT_LIBS ayf-mex-connect_m_static)
	set(BUILD_MEX_MATLAB TRUE)
	set(BUILD_MEX_OCTAVE FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
endif()

if(JVX_USE_PART_OCTAVE)
	
	set(JVX_TARGET_NAME "${PROJECT_NAME}_o")
	set(LOCAL_OCT_LIBS ayf-mex-connect_o_static)
	set(BUILD_MEX_OCTAVE TRUE)
	set(BUILD_MEX_MATLAB FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
	if(JVX_USE_PART_MATLAB)
		add_dependencies(${PROJECT_NAME}_o ${PROJECT_NAME}_m)
	endif()
endif()



