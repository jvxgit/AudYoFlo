jvx_exclude_platform("Emscripten")

set(PROJECT_NAME ayf-starter-emb-host)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(AYFBINRENDER_HEADTRACKER_RS232 TRUE)
set(AYFBINRENDER_HEADTRACKER_ART TRUE)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
  ${CMAKE_CURRENT_BINARY_DIR}

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxMinHosts/jvxMHAppMinHostCon/

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLConsoleHost/include

  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAutomationComponents/ayfATTypical
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/ayf-setup-hooks.cpp
  ${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-shared-connection-emb-host/src/exports/${JVX_OS}/exports-connect-host.def
  ${CMAKE_CURRENT_SOURCE_DIR}/install/ayf-proxy.ini.in
  
)

set(BUILD_SHARED TRUE)

# this changes the name of the output to be followed by "_import"
# It does NOT produce an import lib in the SDK for all projects which
# are compiled to form a runtime
# set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

# This allows to copy import lib (.lib file) to sdk also
# set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

set(LOCAL_LIBS
	
	# We can either hookup the version with static or shared objects
	# jvxLConsoleHost-shared_static
	jvxLConsoleHost-static_static

	ayf-shared-connection-emb-host_static
	ayfATTypical_static
	)


# Note: this dll is installed in the runtime folder. This could also go to the AWE folder,
# however, anyway, the proxy dll must be loaded first and is NOT located in the AWE folder
# As a consequence, you need to add the runtime folder to the PATH environment
## set(JVX_SHARED_LIB_TARGET_FOLDER "${AYF_AWE_SHARED_LIBRARY_TARGET_FOLDER}")

set(JVX_COMPONENT_DLLS_PATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RELEASE_RUNTIME}/jvxComponents")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/install/ayf-proxy.ini.in ${CMAKE_CURRENT_BINARY_DIR}/ayf-proxy.ini)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ayf-proxy.ini" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../Python/python/runtime)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# message(FATAL_ERROR "Import lib")
