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

	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-connectionrules.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-hostconfig.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-sequencer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h
	)

# Some required include paths
include_directories(
   ${SUBPRODUCT_AYFSTARTER_LIBS_BINARY_DIR}/sources/Components/AudioNodes/ayfAuNStarter/
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
   ${CMAKE_CURRENT_SOURCE_DIR}/src
 )

# Add algorithm specific library
set(LOCAL_LIBS
  ayfAuNStarter_static
  jvx-qt-app-templates_static
)

# Link against either audio host lib or develop host lib
include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

## ======================================================================
## Prepare the QT workspace settings in VS
set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

# Some compile defines
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROJECT_NAMESPACE=AYF_STARTER_QT")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
