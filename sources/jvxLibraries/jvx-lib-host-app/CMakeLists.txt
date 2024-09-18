if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME_ROOT jvx-lib-host-app)
set(PROJECT_NAME ${PROJECT_NAME_ROOT})

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
  
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxLibHost.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_report.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_report_on_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_report_state_switch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_report_sequencer.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_sys_cb.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_default.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_helpers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_processes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_sequencer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_periodic.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_components.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_async_forward.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_show.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_pixbuf.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxLibHost_hosthooks.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-host-json/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common

  #${JVX_SUBPRODUCT_ROOT}/software/interfaces/qt-develop-host
  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  #${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  
  #${CMAKE_CURRENT_BINARY_DIR}
  #${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwt-${JVX_QWT_VERSION}/src
  #${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwtPolar-${JVX_QWT_POLAR_VERSION}/src
  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  #${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/src

  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/include
  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/src

  #${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include

  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTThreads/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTLogRemoteHandler/
  )

#if(JVX_OS MATCHES "windows")#
#	if(NOT JVX_USE_CONSOLE_APPS)
#		set(LOCAL_COMPILE_DEFINITIONS "")
#	endif()
#endif()

set(LOCAL_LIBS
	jvxTconfigProcessor_static
    jvxTSystemTextLog_static
	jvxTDataLogger_static
	jvxTThreads_static
	jvxTLogRemoteHandler_static
	jvx-host-json_static
	jvx-app-host-static_static
	)

include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)
set(LOCAL_LIBS_ALL ${LOCAL_LIBS})

# set(LOCAL_PCG_FILE_OPTIONS
#		-P
#		-L
#		-D JVX_WITH_AUDIO_PLUGIN_IF
#		-I ${JVX_SUBPRODUCT_mbc_LIBS_INCLUDE_PATH}/jvx-mbc/pcg)
set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_lib.pcg
  )

set(LOCAL_COMPILE_DEFINITIONS_COMMON "${LOCAL_COMPILE_DEFINITIONS}")
set(BUILD_STATIC TRUE)

#set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-binary-shared_static")
#set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-binary-shared_static")
#set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
#set(LOCAL_COMPILE_DEFINITIONS_COMMON "${LOCAL_COMPILE_DEFINITIONS};JVX_BINARY_API")
#set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-shared_static)
# include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-binary-static_static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-binary-static_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COMMON};JVX_HOST_USE_ONLY_STATIC_OBJECTS;JVX_BINARY_API")
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-static_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-plugin-static_static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-plugin-static_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COMMON};JVX_HOST_USE_ONLY_STATIC_OBJECTS;JVX_PLUGIN_API")
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-static_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
