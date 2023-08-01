if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME ayf-hrtf-dispenser)
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
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/third_party/git/mysofa/libmysofa/src/hrtf
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ayf-hrtf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ayf-hrtf.h
  )

set(LOCAL_LIBS mysofa-static)


set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

if(${JVX_OS} MATCHES "windows")

	if(${JVX_PLATFORM} MATCHES "64bit")
		jvx_install_dedicated_file(${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/third_party/git/mysofa/libmysofa/windows/third-party/zlib/msvc2017_64/lib/zlib/zlib.dll ${INSTALL_PATH_BIN})
	else()
		jvx_install_dedicated_file(${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/third_party/git/mysofa/libmysofa/windows/third-party/zlib/msvc2017_32/lib/zlib/zlib.dll ${INSTALL_PATH_BIN})
	endif()
endif()

# Install sofa source files since they are in install subfolder. This option installs also in
# SDK build mode.
set(JVX_FORCE_INSTALL_PROJECT_SDK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
