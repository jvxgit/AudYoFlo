if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxPacketFilterBridge)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)
# https://stackoverflow.com/questions/40630820/how-to-place-header-and-ui-file-in-different-folders-using-autouic-in-cmake

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxPacketCaptureEndpoint.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOnePacketQueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOneQueueGateway.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxPacketFilterUi.ui
  )
	  
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLsockets_st/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  )

# Project is only built if PCAP is used
set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE;JVX_WITH_PCAP")

set(LOCAL_LIBS
  jvxLsockets_st_static
  jvxLthreads_static
	qwt_static
	jvx-app-host-shared_static
	jvx-qt-helpers_static
	${JVX_QT_LIBS})


set(BUILD_STATIC TRUE)
set(CONFIGURE_LOCAL_START_SCRIPT true)
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
