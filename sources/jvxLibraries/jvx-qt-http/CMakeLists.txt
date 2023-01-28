set(PROJECT_NAME jvx-qt-http)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES   ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx-qt-webcontrol-widget.ui)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
	${JVX_SUBPRODUCT_ROOT}/software/helpers/
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${CMAKE_CURRENT_BINARY_DIR}
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include
	${CMAKE_CURRENT_SOURCE_DIR}/src/
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebHostControl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtWebControlWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxWebHostControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtWebControlWidget.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtWebControlWidget.h
  )

# set(LOCAL_QT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainApplication.qrc)

set(LOCAL_LIBS
	${JVX_QT_LIBS}
	${JVX_QT_NET_LIB}
	jvx-json_static
	# jvx-app-host-shared_static
	jvx-qt-helpers_static
)

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
