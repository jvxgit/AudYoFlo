# Some common include files
set(PROJECT_NAME jvxTestCameraOpenGl)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

include_directories( 
	${OPENGL_INCLUDE_DIRS}  
	${OpenCV_INCLUDE_DIRS}
	${GLUT_INCLUDEDIR} 
	${GLEW_INCLUDEDIR} 
    ${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/escapi-master/include

	${CMAKE_CURRENT_SOURCE_DIR}/src
	)

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoDevice_properties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoDevice_com.cpp	)

set(LOCAL_LIBS 
	${OPENGL_LIBRARIES} 
	${GLUT_LIBRARIES} 
	${GLEW_LIBRARIES} 
	${OpenCV_LIBS}
	runtimeobject
	mfplat
	mf
	mfreadwrite
	mfuuid
	shlwapi
	)

if(JVX_OS MATCHES "windows")
	set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
endif()

# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
