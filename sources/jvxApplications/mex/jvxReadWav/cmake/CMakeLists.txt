if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxReadWav)
include(${JVX_CMAKE_DIR}/common_local_mex_head.cmake)

include_directories(${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLFileIO/include)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../target/entryMex.cpp
  ${JVX_BASE_ROOT}/software/codeFragments/matlab_c/CjvxCToMatlabConverter.cpp
  )

set(LOCAL_LIBS jvxLFileIO_static jvx-dsp-base_static jvx-dsp_static)

set(INSTALL_COMMON_MEX_TARGET_NAME "${PROJECT_NAME}")

set(JVX_RENAME_RELEASE "${PROJECT_NAME}")

if(JVX_USE_PART_MATLAB)
	set(JVX_TARGET_NAME "${PROJECT_NAME}_m")
	set(BUILD_MEX_MATLAB TRUE)
	set(BUILD_MEX_OCTAVE FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
endif()

if(JVX_USE_PART_OCTAVE)
	
	set(JVX_TARGET_NAME "${PROJECT_NAME}_o")
	set(BUILD_MEX_OCTAVE TRUE)
	set(BUILD_MEX_MATLAB FALSE)
	include(${JVX_CMAKE_DIR}/common_local_mex_foot.cmake)
	if(JVX_USE_PART_MATLAB)
		add_dependencies(${PROJECT_NAME}_o ${PROJECT_NAME}_m)
	endif()
endif()



