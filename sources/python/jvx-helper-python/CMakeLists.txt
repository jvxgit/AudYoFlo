cmake_minimum_required(VERSION 3.4...3.18)

# More info given in project jvx_dsp_python
set(PROJECT_NAME jvx_helper_python) 
include(${JVX_CMAKE_DIR}/common_local_python_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	)

#set(LOCAL_LIBS 
#  jvx-dsp-cpp_static)

include(${JVX_CMAKE_DIR}/common_local_python_foot.cmake)