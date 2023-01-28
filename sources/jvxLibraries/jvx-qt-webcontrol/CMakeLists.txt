set(PROJECT_NAME_ROOT jvx-qt-webcontrol)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx-qt-webcontrol-widget.ui
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainApplication.ui
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
	${JVX_SUBPRODUCT_ROOT}/software/helpers/
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets/src
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
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-http/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-http/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-net-helpers/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-crc/include

	${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTconfigProcessor/
   ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSystemTextLog/
   ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTSocket/
   ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger/
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAccessProperties_Qhttp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow_config.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAccessProperties_Qhttp.h
	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/jvxQtApplications/target/main.cpp
  )

set(LOCAL_QT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainApplication.qrc)

set(LOCAL_LIBS_ALL
	${JVX_QT_LIBS}
	${JVX_QT_NET_LIB}
	jvx-json_static
	jvx-qt-helpers_static
	jvx-qt-host-widgets_static
	jvx-qt-http_static
	jvx-crc_static
	jvx-net-helpers_static
	jvxTconfigProcessor_static
    jvxTSystemTextLog_static
	jvxTSocket_static
	jvxTDataLogger_static
)

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(PROJECT_NAME "${PROJECT_NAME_ROOT}-shared")
set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-shared_static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-shared_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-shared_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# JVX_HOST_USE_ONLY_STATIC_OBJECTS
set(JVX_TARGET_NAME "${PROJECT_NAME_ROOT}-static_static")
set(PROJECT_NAME "${PROJECT_NAME_ROOT}-static")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME_ROOT}-static_static")
set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HOST_USE_ONLY_STATIC_OBJECTS")
set(LOCAL_LIBS ${LOCAL_LIBS_ALL} jvx-app-host-static_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
