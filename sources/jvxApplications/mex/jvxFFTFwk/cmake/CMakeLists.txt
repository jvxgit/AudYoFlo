set(PROJECT_NAME jvxFFTFwk)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${JVX_BASE_ROOT}/sources/jvxLibraries/ayf-mex-connect/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
	)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../target/entryMex.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxFFTFwk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxFFTFwk.h
  )

  set(LOCAL_LIBS "jvx-dsp_static")

set(INSTALL_COMMON_MEX_TARGET_NAME "${PROJECT_NAME}")

if(JVX_USE_PART_MATLAB)
	set(JVX_TARGET_NAME "${PROJECT_NAME}_m")
	set(LOCAL_MAT_LIBS ayf-mex-connect_m_static)
	set(INSTALL_TARGET_NAME_MATLAB ${JVX_TARGET_NAME})
	set(BUILD_MEX_MATLAB TRUE)
	set(BUILD_MEX_OCTAVE FALSE)
	set(JVX_RENAME_RELEASE "${PROJECT_NAME}")
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
endif()

if(JVX_USE_PART_OCTAVE)
	
	set(JVX_TARGET_NAME "${PROJECT_NAME}_o")
	set(LOCAL_OCT_LIBS ayf-mex-connect_o_static)
	set(INSTALL_TARGET_NAME_OCTAVE ${JVX_TARGET_NAME})
	set(BUILD_MEX_OCTAVE TRUE)
	set(BUILD_MEX_MATLAB FALSE)
	set(JVX_RENAME_RELEASE "${PROJECT_NAME}")
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
	if(JVX_USE_PART_MATLAB)
		add_dependencies(${PROJECT_NAME}_o ${PROJECT_NAME}_m)
	endif()
endif()



