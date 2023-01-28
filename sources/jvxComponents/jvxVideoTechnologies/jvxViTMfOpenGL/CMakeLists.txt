set(PROJECT_NAME jvxViTMfOpenGL)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

if(JVX_USE_OPENGL_X11)
	set(LOCAL_PCG_FILE_OPTIONS 
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
		-D JVX_USE_AS_TRIGGERED_MULTIBUFFER
		-D JVX_USE_OPENGL_X11
		)
else()
	set(LOCAL_PCG_FILE_OPTIONS 
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
		-D JVX_USE_AS_TRIGGERED_MULTIBUFFER
		)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
	 ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

if(JVX_USE_OPENGL_X11)
	include_directories(
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp/include
		${OPENGL_INCLUDE_DIRS}  
		${GLUT_INCLUDEDIR} 
		${GLEW_INCLUDEDIR} 
		)
endif()

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLDevice.cpp

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLDevice.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLDevice_properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLDevice_com.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfOpenGLDevice_link.cpp
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS 
	jvx-component-templates-base_static
	runtimeobject
	mfplat
	mf
	mfreadwrite
	mfuuid
	shlwapi
	)

if(JVX_USE_OPENGL_X11)
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvx-opengl-cpp_static
		)

	if(GLUT_INSTALL_LIB_NAME)
		install(PROGRAMS ${GLUT_INSTALL_LIB_NAME} DESTINATION ${INSTALL_PATH_BIN})
	endif()
	if(GLEW_INSTALL_LIB_NAME)
		install(PROGRAMS ${GLEW_INSTALL_LIB_NAME} DESTINATION ${INSTALL_PATH_BIN})
	endif()
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_OPENGL_X11")
	
endif()
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
