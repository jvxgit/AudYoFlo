# Additional information:
# https://pybind11.readthedocs.io/en/stable/cmake/index.html

set(PROJECT_NAME jvx_dsp_python) # requires underscore to be loaded in python!
include(${JVX_CMAKE_DIR}/common_local_python_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include)

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_dw_ola.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_dw_ola.h
	)

set(LOCAL_LIBS 
  jvx-dsp-cpp_static)

set(JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/python/runtime/local.py")
set(LOCAL_PYTHON_SCRIPT "python/runtime/test-jvx-dsp-python.py")

# message(FATAL_ERROR "-x-> ${JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE}")

include(${JVX_CMAKE_DIR}/common_local_python_foot.cmake)