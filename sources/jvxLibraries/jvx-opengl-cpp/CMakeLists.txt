cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

if(JVX_USE_OPENGL_X11)

	set(PROJECT_NAME jvx-opengl-cpp)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${OPENGL_INCLUDE_DIRS}  
		${GLUT_INCLUDEDIR} 
		${GLEW_INCLUDEDIR} 
		)

	set(LOCAL_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVideoRenderCore.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVideoRenderCore.h
		)

	set(LOCAL_LIBS
		${OPENGL_LIBRARIES} 
		${GLUT_LIBRARIES} 
		${GLEW_LIBRARIES} 
		)

	set(BUILD_STATIC TRUE)

	set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
	#set(INSTALL_DEDICATED_PCG_TREE TRUE)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
else()
	if(JVX_RELEASE_SDK)
		# Copy the header file to include folder since it is used event though no lib is compile
		get_filename_component(suffix ${CMAKE_CURRENT_SOURCE_DIR}  NAME )
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix} FILES_MATCHING PATTERN "*.h")
		message("Copying <${CMAKE_CURRENT_SOURCE_DIR}/include> to <${INSTALL_PATH_INCLUDE_STATIC}/${suffix}>")
	endif()
endif()
