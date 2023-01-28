set(PROJECT_NAME jvxLConsoleTools-shared)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../include/platform/${JVX_OS}
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHJvx/
	${JVX_INTEGRATE_SOURCES_PATH}/software/codeFragments/jvxHosts/common
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
	
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-host-json/include

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-crc/include

	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreads/
	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSocket/
  )

set(LOCAL_SOURCES
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost.cpp

	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_help.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_sys_cb.cpp

	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_commands.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_configfiles.cpp
	 # ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConsoleHost_be_drivehost_shutdown.cpp
	 # ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConsoleHost_be_drivehost_boot.cpp	 
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_callbacks.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_rem_control.cpp

	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_show.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_act.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_drivehost_file.cpp

	 ${JVX_INTEGRATE_SOURCES_PATH}/sources/jvxLibraries/jvxLConsoleTools/src/CjvxConsoleHost_be_drivehost_boot_specific.cpp
	 ${JVX_INTEGRATE_SOURCES_PATH}/sources/jvxLibraries/jvxLConsoleTools/src/CjvxConsoleHost_be_drivehost_shutdown_specific.cpp

	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_be_print.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/CjvxConsoleHost_fe_console.cpp

	 ${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxConsoleHost_be_drivehost.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxConsoleHost_be_print.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/../include/CjvxConsoleHost_fe_console.h
	 
	 ${CMAKE_CURRENT_SOURCE_DIR}/../src/platform/${JVX_OS}/jvx_console_tools.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/../include/platform/${JVX_OS}/jvx_console_tools.h
)

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_CONSOLE_WITH_END_TRIGGER")
if(${JVX_OS} MATCHES "windows")
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_CONSOLE_KEYMAP_V1")
endif()

#set(LOCAL_PCG_FILES
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_mexcall.pcg
#  )

set(LOCAL_LIBS	
	jvx-host-json_static
	jvx-json_static
	jvx-crc_static
	jvx-app-host-shared_static
	jvxTconfigProcessor_static
	jvxTSystemTextLog_static
	jvxTDataLogger_static
	jvxTThreads_static
	jvxTSocket_static
	 )

# Include the project specific references
include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
