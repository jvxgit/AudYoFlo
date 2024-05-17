# Decide which host library to link with
set(JVX_LINK_WITH_AUDIO_HOST TRUE)

# Link against either audio host lib or develop host lib
include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

# Some required include paths
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src

	${JVX_BASE_ROOT}/software/templates/jvxHosts/qt-host-ui-extensions/
    ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
    ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
    ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-component-widgets/include

	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-component-templates-product/include
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated


	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-app-templates/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-base/include/interfaces/qt
	# ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-helpers/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-qt-central-widgets/include
 )

# Add algorithm specific library
set(LOCAL_LIBS ${LOCAL_LIBS}
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	# jvx-qt-component-widgets_static

	jvx-qt-app-templates_static
	jvx-qt-central-widgets_static
	)

if(JVX_USES_SDK)
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxLQtMaWidgets_static)
endif()

# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

# Macos x specific configurations
if(JVX_OS MATCHES "macosx")
  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
else()
  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
endif()

set(JVX_PREPARE_QT_WORKSPACE_VS TRUE)