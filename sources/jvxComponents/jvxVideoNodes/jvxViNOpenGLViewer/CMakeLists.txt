set(PROJECT_NAME jvxViNOpenGLViewer)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

if(JVX_USE_GLEW_GLUT)
	include_directories(
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp/include
		${OPENGL_INCLUDE_DIRS}  
		${GLUT_INCLUDEDIR} 
		${GLEW_INCLUDEDIR} 
		)
endif()

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxViNOpenGLViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxViNOpenGLViewer.h
  )

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_node.pcg
  )

if(JVX_USE_GLEW_GLUT)
	set(LOCAL_PCG_FILE_OPTIONS 
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
		-D JVX_USE_AS_TRIGGERED_MULTIBUFFER
		-D JVX_USE_GLEW_GLUT
		-D JVX_SKIP_VISUAL_DATA_PROPS
		)
else()
	set(LOCAL_PCG_FILE_OPTIONS 
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
		-D JVX_USE_AS_TRIGGERED_MULTIBUFFER
		-D JVX_SKIP_VISUAL_DATA_PROPS
		)
endif()

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  )

  if(JVX_USE_GLEW_GLUT)
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvx-opengl-cpp_static
		)

	if(GLUT_INSTALL_LIB_NAME)
		install(PROGRAMS ${GLUT_INSTALL_LIB_NAME} DESTINATION ${INSTALL_PATH_BIN})
	endif()
	if(GLEW_INSTALL_LIB_NAME)
		install(PROGRAMS ${GLEW_INSTALL_LIB_NAME} DESTINATION ${INSTALL_PATH_BIN})
	endif()
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_GLEW_GLUT")
	
endif()
  
set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
