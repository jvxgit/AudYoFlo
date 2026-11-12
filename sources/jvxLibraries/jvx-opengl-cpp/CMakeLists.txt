if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Text output of the overlay: download stb_truetype + DejaVu Sans and generate the font header.
# Sets JVX_STB_DEJAVU_INCLUDE_DIR
include(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/web/stb-dejavu/prepare.cmake)

if(JVX_USE_GLEW_GLUT)

	set(PROJECT_NAME jvx-opengl-cpp)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${JVX_STB_DEJAVU_INCLUDE_DIR}
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

	# Downloaded / generated headers next to the own headers in the SDK
	if((JVX_RELEASE_SDK OR JVX_USES_SDK) AND NOT JVX_NO_WRITE_SDK)
		install(DIRECTORY ${JVX_STB_DEJAVU_INCLUDE_DIR}/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/jvx-opengl-cpp/include COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
	endif()
else()
	if(JVX_RELEASE_SDK)
		# Copy the header file to include folder since it is used event though no lib is compile
		get_filename_component(suffix ${CMAKE_CURRENT_SOURCE_DIR}  NAME )
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix} FILES_MATCHING PATTERN "*.h")
		message("Copying <${CMAKE_CURRENT_SOURCE_DIR}/include> to <${INSTALL_PATH_INCLUDE_STATIC}/${suffix}>")
		install(DIRECTORY ${JVX_STB_DEJAVU_INCLUDE_DIR}/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix}/include FILES_MATCHING PATTERN "*.h")
	endif()
endif()
