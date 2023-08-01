if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Some common include files
set(PROJECT_NAME jvxopengl)
include(${JVX_CMAKE_DIR}/common_local_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST FALSE)

# Add mainWidget and other ui files
#set(LOCAL_UI_SOURCES
#	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myMainWidget.ui
#	)

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxopengl.qrc
	)

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_sequencer_common_video.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_connection_rules_common_video.cpp
	)

# Some required include paths
include_directories(
  ${JVX_SUBPRODUCT_OPENGL_LIBS_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNOpenGl/
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp/include

    ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
  ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/
   ${JVX_BASE_BINARY_DIR}/sources/jvxComponents/jvxVideoTechnologies/jvxViTMfOpenGL/

  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src

 )

# Add algorithm specific library
set(LOCAL_LIBS
  jvxAuNOpenGl_static
  jvxLQtSaWidgetWrapper_static
  jvxLQtSaWidgets_static
  jvxLexternalCall_static
  jvx-qt-app-templates_static
)

set(LOCAL_COMPILE_DEFINITIONS "")

# Link against either audio host lib or develop host lib
if(JVX_LINK_WITH_AUDIO_HOST)

	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_LINK_WITH_AUDIO_HOST")

	# Link with audio host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLAudioHost__algostatic_static
		)

	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.audiohost.cmake)
	endif()
else()

	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_LINK_WITH_DEVELOP_HOST")

	# Link with develop host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLDevelopHost-shared_static
		)
	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.develophost.cmake)
	endif()

endif()

source_group("ui" FILES "${LOCAL_UI_SOURCES}")
source_group("qrc" FILES "${LOCAL_QT_RESOURCES}")
source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

if(JVX_OS MATCHES "windows")
	set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
endif()

## ======================================================================
## The audio libs
if(JVX_USE_PART_PAUDIO)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_PORTAUDIO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTPortAudio_static)
endif()

if(JVX_USE_PART_ASIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAsio_static)
endif()

if(JVX_USE_PART_COREAUDIO)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTCoreAudio_static ${COREAUDIO} ${COREFOUNDATION} ${AUDIOUNIT} ${COREAUDIOKIT} ${AUDIOTOOLBOX} ${CORESERVICES})
endif()

if(JVX_USE_PART_ALSA)
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxAuTAlsa_static asound)
endif()

if(JVX_OS MATCHES "windows")
  set(LOCAL_LIBS ${LOCAL_LIBS} jvxViTMfOpenGL_static)
endif()

## ======================================================================

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

# Some compile defines
#set(LOCAL_COMPILE_DEFINITIONS "JVX_ADD_HOST_WEBSERVER;JVX_PROJECT_NAMESPACE=JVX_MY_HOST;JVX_NO_SEQUENCER;JVX_NO_RTVIEWER;JVX_NO_MESSAGES")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_ADD_WIDGET_WRAPPER;JVX_PROJECT_NAMESPACE=JVXOPENGL")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
