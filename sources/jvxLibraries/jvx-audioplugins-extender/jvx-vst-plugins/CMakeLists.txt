cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-vst-plugins)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
#  add_custom_target(jvx-vq-doc ALL
#    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
#    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
#    )
#endif(DOXYGEN_FOUND)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${JVX_VST_SDK_ROOT}
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugins-extender/jvx-plugin-connector/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVstPluginExtender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxVstPluginExtender.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVstPluginController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxVstPluginController.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVstPluginCommon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxVstPluginCommon.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVstPluginParameter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxVstPluginParameter.h
  )

# set(LOCAL_LIBS jvx-dsp-base_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

if(${JVX_OS} MATCHES "linux")
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};DEVELOPMENT")
endif()	

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
