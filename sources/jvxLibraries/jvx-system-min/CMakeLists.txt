if(DEFINED JVX_SDK_PATH)
    include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-system-min)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(JVX_GENERATE_VERSION_TAG_DUMMY)
	configure_file(include/jvx_system_config_dummy.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h)
else()
	if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h" OR JVX_FORCE_GENERATE_NEW_VERSION_TAG)
		message("${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h generated since tags have changed.")
		configure_file(include/jvx_system_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h)
	else()
		message("${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h NOT generated to avoid build dependency, use <JVX_FORCE_GENERATE_NEW_VERSION_TAG> to force generation.")
	endif()
endif()

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-system-min-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include/platform/${JVX_OS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_defines.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_dataformats.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_common.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_error.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_error_types.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_helpers.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_allocate.h
	${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_system_config.h.in

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_system_error.c

	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${JVX_OS}/jvx_system_assert.c
	)

if(${JVX_OS} MATCHES "windows")
	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${JVX_OS}/jvx_system_specific.c
		${CMAKE_CURRENT_SOURCE_DIR}/include/platform/${JVX_OS}/jvx_system_mutex.h

	)
else()
	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/platform/common_ux/jvx_system_specific.c)
endif()
		

set(LOCAL_ADDITIONAL_INSTALL_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/jvx_system_config.h)

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
