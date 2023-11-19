set(PROJECT_NAME ayf-hdf5io)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${AYF_HDF5_INCLUDE_DIR}
	)

set(LOCAL_SOURCES
 	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CayfHdf5Io.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfHdf5Io.cpp
  )

set(LOCAL_LIBS	
	${AYF_HDF5_LIBRARIES}
	 )

# link_directories(${AYF_HDF5_LIBRARY_DIR})
# message(FATAL_ERROR "-> ${AYF_HDF5_LIBRARY_DIR}")
set(BUILD_STATIC TRUE)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
