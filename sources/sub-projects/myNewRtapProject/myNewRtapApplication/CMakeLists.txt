# Some common include files
set(PROJECT_NAME myNewRtapApplication)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Add mainWidget and other ui files
set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc/myMainWidget.ui
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/uisrc/jvxmyNewProject.qrc
	)

# Source code files
set(LOCAL_SOURCES
 
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src/myCentralWidget.h

	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/myNewRtapRtHooks.h
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/myNewRtapRtHooks.cpp

	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
	)

# Some required include paths
include_directories(
  ${JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LIBS_BINARY_DIR}/myNewRtapAudioNode/
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src

  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa/
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio/
  ${JVX_BINARY_DIR}/base/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio/

  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_INCLUDE_PATH}/jvx-component-templates-product/generated
  
	${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapWidgets/src
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/myNewRtapRtHooks/
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src
 )

# Add algorithm specific library
set(LOCAL_LIBS
  myNewRtapAudioNode_static
  jvxLQtSaWidgetWrapper_static
  jvxLQtSaWidgets_static
    jvxLexternalCall_static
	myNewRtapLibrary_static
	jvx-qt-app-templates_static
)

# Link against either audio host lib or develop host lib
if(JVX_LINK_WITH_AUDIO_HOST)

	# Link with audio host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLAudioHost__algostatic_static
		)

	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.audiohost.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
	endif()
else()

	# Link with develop host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLDevelopHost_static
		)
	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.develophost.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
	endif()

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
## ======================================================================

# Some compile defines
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_ADD_WIDGET_WRAPPER;JVX_PROJECT_NAMESPACE=JVX_myNewProject")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

source_group("ui" FILES "${LOCAL_UI_SOURCES}")
source_group("qrc" FILES "${LOCAL_QT_RESOURCES}")
source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

if(JVX_OS MATCHES "windows")
	set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)
endif()

# Copy images from image folder
set(LOCAL_IMAGES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/uisrc/images")


# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)




