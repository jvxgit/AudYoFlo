cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxMerConcept)
include(${JVX_CMAKE_DIR}/common_local_head.cmake)


if(NOT JVX_USE_PART_ASIO)
  error("${JVX_TARGET_NAME} only works for ASIO at the moment")
endif()

#set(LOCAL_UI_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainWindow.ui
#    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/loadsaveFileDialog.ui
#    )
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_SOURCES
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_shutdown.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_config.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_bootup.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_specific.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_sequencer.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_iohandler.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_algorithm.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_user.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_plot.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_device.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_ui.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_fileio.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxRtAudioBuffers.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxOneAudioBuffer.cpp
	${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/uLoadSaveDialog.cpp
 )


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/config
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/qt-develop/src
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwt-${JVX_QWT_VERSION}/src
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper/
  ${JVX_BINARY_DIR}/base/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/
  ${JVX_BINARY_DIR}/base/sources/jvxHosts/jvxHJvx
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNNoiseReductionMc/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${SNDFILE_INCLUDEDIR}
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE;JVX_HOST_USE_ONLY_STATIC_OBJECTS")

set(LOCAL_LIBS
  jvxHJvx_static
  jvxLthreads_static
  jvxAuNNoiseReductionMc_static
  jvxAuTGenericWrapper_static
  jvxLFileIO_static
  qwt-${JVX_QWT_VERSION}
)

set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAsio_static)

set(LOCAL_IMAGES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/images)

if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
  set(JVX_MAC_OS_CREATE_BUNDLE_AUDIO_PLUGINS TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
