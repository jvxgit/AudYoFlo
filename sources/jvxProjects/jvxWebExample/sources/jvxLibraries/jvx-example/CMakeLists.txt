cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-example)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-example-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}  
  ${JVX_EXAMPLE_DRIVER_FOLDER}
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_example_field1.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_example_field1.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_example_field2.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_example_field2.h
  )

set(LOCAL_LIBS jvx-dsp-base_static)

if(JVX_CROSS_COMPILE)
  # set(LOCAL_COMPILE_DEFINITIONS "${FFT_COMPILE_DEFINITIONS}")
  set(LOCAL_COMPILE_DEFINITIONS "JVX_NO_FAIL_IF_DRIVER_UNAVAIL")
else()
  #  set(LOCAL_COMPILE_DEFINITIONS "JVX_NO_FAIL_IF_DRIVER_UNAVAIL;JVX_DARWIN_LIB_DEBUG")
    set(LOCAL_COMPILE_DEFINITIONS "JVX_NO_FAIL_IF_DRIVER_UNAVAIL")
endif()

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
