if(JVX_USE_OPENCV)
	set(PROJECT_NAME ayfViNCameraConvert)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/target
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
		${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

		${OPENCV_LIBRARY_PATH}
	)

	set(LOCAL_PCG_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/exports_node.pcg)
  
	set(LOCAL_SOURCES 
		${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CayfViNCameraConvert.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CayfViNCameraConvert_process.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CayfViNCameraConvert.h
	)

#set(LOCAL_PCG_FILE_OPTIONS 
#	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
#	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay	
#	-D JVX_DECLARE_PROP_REF_INSTALL_CALLBACK
#	)
	
	set(LOCAL_LIBS 
		jvx-component-templates-base_static
		${OPENCV_LIBRARIES}
	)

	set(IS_JVX_COMPONENT TRUE)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

endif()