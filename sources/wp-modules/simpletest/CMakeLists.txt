set(PROJECT_NAME SimpleKernel)
cmake_minimum_required(VERSION 3.4...3.18)

set(WP_COMMON_SOURCES 
	src/SimpleKernel.h
	src/SimpleKernel.cpp
	src/main.cpp
	${JVX_BASE_ROOT}/software/codeFragments/wp-modules/interface/main_header.h
	CMakeLists.python.txt
	CMakeLists.webassembly.txt
	)

set(WP_COMMON_INCLUDES
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${JVX_BASE_ROOT}/software/codeFragments/wp-modules/interface
		)

if(${JVX_OS} MATCHES "Emscripten")
	set(WP_COMMON_DEFINITIONS "JVX_WP_MODULE_EMSCRIPTEN")
	include(CMakeLists.webassembly.txt)
else()
	if(JVX_USE_PYTHON)
		set(WP_COMMON_DEFINITIONS "JVX_WP_MODULE_PYTHON")
		include(CMakeLists.python.txt)
	endif()
endif()

