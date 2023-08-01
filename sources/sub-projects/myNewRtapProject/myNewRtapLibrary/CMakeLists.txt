set(PROJECT_NAME myNewRtapLibrary)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(myNewRtapLibrary-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_myNewProject.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_myNewProject.h
	)

#set(LOCAL_OCTCONF_SCRIPTS
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_diffArray/generate_config_header.m
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_mvdrElko/generate_config_header.m
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_pdm/generate_config_header.m
#  )

set(LOCAL_LIBS jvx-dsp_static jvx-dsp-base_static ${FFT_LIBRARIES})

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};${FFT_COMPILE_DEFINITIONS}")

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
