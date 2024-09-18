# Some common include files
set(PROJECT_NAME ayfStarterQt)
include(${JVX_CMAKE_DIR}/common_local_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Add mainWidget and other ui files
set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myMainWidget.ui
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/ayfStarter.qrc
	)

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h
	)

# Some required include paths
include_directories(
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
   ${CMAKE_CURRENT_SOURCE_DIR}/src
 )

# Add algorithm specific library
set(LOCAL_LIBS
  ayfAuNStarter_static
  jvx-qt-app-templates_static
)
if(${JVX_OS} MATCHES "windows")
  set(LOCAL_LIBS
    ${LOCAL_LIBS}
    Ws2_32.Lib
    ntdll.lib
    Userenv.lib
  )
endif()

include(../common/local_config.cmake)

## ======================================================================
## Prepare the QT workspace settings in VS
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

# Some compile defines
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROJECT_NAMESPACE=AYF_STARTER_QT")

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
