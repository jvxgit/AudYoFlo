# Additional information:
# https://pybind11.readthedocs.io/en/stable/cmake/index.html

set(PROJECT_NAME ayf_starter_python) # requires underscore to be loaded in python!
include(${JVX_CMAKE_DIR}/common_local_python_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src

	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include
	
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-component-lib/include
	${JVX_SUBPRODUCT_BINARY_INCLUDE_PATH}/ayf-component-lib/generated

	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include)

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ayf_starter_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ayf_starter_python.h
	)

set(LOCAL_LIBS 
	jvx-dsp-cpp_static
	ayf_connect-starter_static)

set(AYF_PYTHON_PACKAGES "${AYF_PYTHON_PACKAGES_ROOT}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python/runtime/test-ayf-starter.py.in ${CMAKE_CURRENT_SOURCE_DIR}/python/runtime/test-ayf-starter.py @ONLY)
set(JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/python/runtime/local.py")
set(LOCAL_PYTHON_SCRIPT "python/runtime/test-ayf-starter.py")

# message(FATAL_ERROR "-x-> ${JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE}")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};AYF_PROJECT_POSTFIX=starter")

include(${JVX_CMAKE_DIR}/common_local_python_foot.cmake)