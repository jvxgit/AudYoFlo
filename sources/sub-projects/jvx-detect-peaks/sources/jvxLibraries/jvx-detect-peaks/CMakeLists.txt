set(PROJECT_NAME jvx-detect-peaks)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_smooth_sliding_window.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_detect_peaks.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_detect_peaks_helpers.c

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_smooth_sliding_window.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_detect_peaks.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_detect_peaks_helpers.h
  )

#set_source_files_properties(
#		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_smooth_sliding_window.c
#		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_detect_peaks.c
#		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_detect_peaks_helpers.c
#		PROPERTIES LANGUAGE CXX)

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
