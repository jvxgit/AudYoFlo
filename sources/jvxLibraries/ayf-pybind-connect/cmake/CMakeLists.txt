set(PROJECT_NAME ayf-pybind-connect)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../src

  ${pybind11_INCLUDE_DIRS}
  ${Python_INCLUDE_DIRS}
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxCToPythonConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxPropertiesToPythonConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxPythonToCConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/localPybindIncludes.h

	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxCToPythonConverter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxPropertiesToPythonConverter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxPythonToCConverter.cpp
)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
