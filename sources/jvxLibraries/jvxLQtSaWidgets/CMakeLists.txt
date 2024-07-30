set(PROJECT_NAME jvxLQtSaWidgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtSpectrumViewer.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtLogFileWriter.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtNetworkMasterDevice.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtNetworkSlaveDevice.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxQtNetworkTechnology.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
  ${JVX_SUBPRODUCT_ROOT}/software/helpers/
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
  )

 ## Does this require x11 headers?
include_directories(
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp/include
	)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaSpectrumViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaCircbufferViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLinearbufferViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaWebServerWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLightOnOffWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLogFileWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkMasterDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkSlaveDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkTechnology.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaSpectrumViewer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaCircbufferViewer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLinearbufferViewer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaWebServerWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLightOnOffWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaLogFileWriter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkMasterDevice.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkSlaveDevice.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaNetworkTechnology.h
  )

	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaGlWidget.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtSaGlWidget.H
		)
set(LOCAL_LIBS
  qwt_static
  jvx-qt-helpers_static
)
set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
