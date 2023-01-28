# Some common include files
set(PROJECT_NAME jvxTestOpenGl)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories( 
	${OPENGL_INCLUDE_DIRS}  
	${GLUT_INCLUDEDIR} 
	${GLEW_INCLUDEDIR} 
	)

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
	)

set(LOCAL_LIBS 
	${OPENGL_LIBRARIES} 
	${GLUT_LIBRARIES} 
	${GLEW_LIBRARIES} 
	)

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
