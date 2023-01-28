# Some common include files
set(PROJECT_NAME jvxQtWebControl)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(LOCAL_QT_RESOURCES uisrc/mainApplication.qrc
  )
  set(CMAKE_AUTOMOC ON)

#set(LOCAL_UI_SOURCES
 #   ${CMAKE_CURRENT_SOURCE_DIR}/uisrc/mainApplication.ui
 #   )
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
	)

# Source code files
set(LOCAL_SOURCES
    #${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/target/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/uMainWindow.cpp
	)

	# Add algorithm specific library
set(LOCAL_LIBS
	qwt_static
	${JVX_QT_LIBS}
	${JVX_QT_NET_LIB}
	jvx-qt-helpers_static	
	)

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
