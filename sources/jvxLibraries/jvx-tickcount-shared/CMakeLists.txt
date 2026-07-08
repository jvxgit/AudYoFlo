if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-tickcount-shared)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-tickcount-shared-doc ALL
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
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_tickcount_shared.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_tickcount_shared.h
  )


set(LOCAL_LIBS jvx-dsp-base_static)

add_export_definition_file(EXPORT_SOURCES EXPORT_LINKER_FLAGS ${CMAKE_CURRENT_SOURCE_DIR}/exports/${JVX_OS}/exports.def)
#set(LOCAL_COMPILE_DEFINITIONS "${FFT_COMPILE_DEFINITIONS}")


if(JVX_OS MATCHES "windows")
	set(BUILD_SHARED TRUE)
	set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)
	set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

	set(LOCAL_SOURCES ${LOCAL_SOURCES} ${EXPORT_SOURCES})
	set(LOCAL_LINKER_FLAGS ${LOCAL_LINKER_FLAGS} ${EXPORT_LINKER_FLAGS})
else()
	set(BUILD_STATIC TRUE)
endif()

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
