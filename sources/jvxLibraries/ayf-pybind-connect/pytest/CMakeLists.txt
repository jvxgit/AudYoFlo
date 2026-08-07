# Smoke test module for ayf-pybind-connect - not part of the SDK's public
# API surface, only used to verify the ported converters compile and behave
# correctly against real pybind11 objects.
set(PROJECT_NAME ayf_pybind_connect_smoketest)
include(${JVX_CMAKE_DIR}/common_local_python_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	)

set(LOCAL_LIBS
  ayf-pybind-connect_static
  jvx-dsp-cpp_static)

include(${JVX_CMAKE_DIR}/common_local_python_foot.cmake)
