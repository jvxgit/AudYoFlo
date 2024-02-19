
set(PROJECT_NAME ayfStarterWeb)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Decide which host library to link with
set(JVX_AUTOMATION_VERBOSE TRUE)
set(JVX_LOCAL_AUTOMATION_ON_START FALSE)

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-connectionrules.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-hostconfig.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-sequencer.cpp
	)

# Some required include paths
include_directories(

	${SUBPRODUCT_AYFSTARTER_LIBS_BINARY_DIR}/sources/Components/AudioNodes/ayfAuNStarter/    
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated

 )

# Add algorithm specific library
set(LOCAL_LIBS
	ayfAuNStarter_static
)

# ====================================================================================
# Console Host specific part
# ====================================================================================

set(JVX_LINK_WITH_CONSOLE_HOST TRUE)
set(JVX_LINK_WITH_CONSOLE_HOST_FRONTEND_BACKEND_LIB TRUE)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};COMPILE_FOR_CONSOLE")
force_console_app()

# ====================================================================================
# ====================================================================================

# Link against either audio host lib or develop host lib
include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

## ======================================================================

# Some compile defines
#set(LOCAL_COMPILE_DEFINITIONS "JVX_ADD_HOST_WEBSERVER;JVX_PROJECT_NAMESPACE=JVX_MY_HOST;JVX_NO_SEQUENCER;JVX_NO_RTVIEWER;JVX_NO_MESSAGES")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROJECT_NAMESPACE=AYF_STARTER_WEB")

# Specify the max number of devices to be mixed during processing per technology. The QT host assumes at least a fixed number other than inf
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_NUMBER_AUDIO_DEVICES_MIX=6")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

# Activate the VS project settings to run applications by F5
set(JVX_PREPARE_WORKSPACE_VS TRUE)

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)


