if(DEFINED JVX_SDK_PATH)
	cmake_minimum_required (VERSION 2.8.11)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

# Some common include files
set(PROJECT_NAME jvxAnalyzeAudio)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

set(JVX_LINK_AUDIO_STATIC FALSE)

# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Add mainWidget and other ui files
set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/myMainWidget.ui
	)
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvxAnalyzeAudio.qrc
	)

# Source code files
set(LOCAL_SOURCES

	# Always add master network option: this app involves only dynamic lib devices
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioHost_hooks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/myCentralWidget.h

	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_sequencer_common_audio.cpp
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common/jvx_connection_rules_common_audio.cpp
)

# Some required include paths
include_directories(
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt
  ${JVX_SUBPRODUCT_SIGNALS_LIBS_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNAnalyze/
  #${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLQtAudioWidgets/include
  ${JVX_SUBPRODUCT_SIGNALS_LOCAL_QT_LIBS_INCLUDE_DIR}
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtMaWidgets/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-helpers/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qwt/qwt/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/qcustomplot-${JVX_QCUSTOMPLOT_VERSION}/${JVX_QCUSTOMPLOT_SRC_FOLDER}

  # For the network extensions
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/network/

  # data logging
  ${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTDataLogger
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
  ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets/src

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates/src

  )
  
# Add algorithm specific library
set(LOCAL_LIBS
	#jvxLQtAudioWidgets_static
	jvx-qt-audiotools_static
	jvxAuNAnalyze_static
	jvxLQtMaWidgets_static
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	jvx-qt-helpers_static
	jvx-qt-component-widgets_static
	${FFT_LIBRARIES}
	jvx-qt-app-templates_static
)

# Link against either audio host lib or develop host lib
if(JVX_LINK_WITH_AUDIO_HOST)

	if(JVX_LINK_AUDIO_STATIC)
		# Link with audio host
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxLAudioHost__algostatic_static
			)
	else()
		# Link with audio host
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxLAudioHost__algoshared_static
			)
	endif()

	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.audiohost.cmake)
	endif()
else()

	# Link with develop host
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLDevelopHost_static
		)
	# In case of standalone builds, we need to pull in some other libraries
	if(DEFINED JVX_SDK_PATH)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.network.cmake)
		include(${JVX_CMAKE_DIR}/cmake-sdk/cmake-lib-packages.develophost.cmake)
	endif()

endif()

include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

set(LOCAL_COMPILE_DEFINITIONS "JVX_PROJECT_NAMESPACE=JVX_MY_HOST;JVX_ADD_WIDGET_WRAPPER;JVX_LINK_MA_LOGFILE_WRITER")

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
  set(JVX_MAC_OS_CREATE_BUNDLE_AUDIO_PLUGINS TRUE)
else(JVX_OS MATCHES "macosx")
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif(JVX_OS MATCHES "macosx")

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)

if(JVX_COMPILE_QT_EGLFS_TAG)
	set(LOCAL_START_SCRIPT "start-jvxanalyzepi-qt")
endif()

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
