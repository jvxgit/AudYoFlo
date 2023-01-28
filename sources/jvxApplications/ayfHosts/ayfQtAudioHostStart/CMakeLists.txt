set(PROJECT_NAME ayfQtAudioHostStart)
include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Add mainWidget and other ui files
set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc/myMainWidget.ui
	)

set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc)

# Qt resource file add
set(LOCAL_QT_RESOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/uisrc/ayfQtAudioStart.qrc
	)

# Source code files
set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/ayf_involved_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ayf_involved_sequencer_steps.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayf_involved_connection_rules.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayf_involved_components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/myCentralWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/myCentralWidget.cpp
	)

include_directories(
	    ${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxAudioNodes/ayfAuNMatlab
	)
set(LOCAL_LIBS 
	ayfAuNMatlab_static
	)

# Add the files to build a qt audio host
include(${JVX_SUBPRODUCT_ROOT}/cmake/qt-audio-host-tpl.cmake)

# Some compile defines
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_PROJECT_NAMESPACE=QTSTARTHOST;JVX_ADD_WIDGET_WRAPPER")

# Some common include files
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
