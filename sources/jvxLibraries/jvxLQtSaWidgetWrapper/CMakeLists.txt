set(PROJECT_NAME jvxLQtSaWidgetWrapper)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${JVX_SUBPRODUCT_ROOT}/software/helpers/
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtSaWidgetWrapper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementBase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementAbstractButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementComboBox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementSlider.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementProgressBar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementLineEdit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementListWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementLabel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementFrame.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementPushButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMaWrapperElementTreeWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxLogFileWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkMasterDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkSlaveDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkSlaveDevice_slots.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxSaWidgetWrapper_elements.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtSaWidgetWrapper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementAbstractButton.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementComboBox.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementSlider.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementProgressBar.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementLineEdit.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementListWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementLabel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementFrame.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementPushButton.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMaWrapperElementTreeWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxLogFileWriter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkSlaveDevice.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSaWrapperElementJvxNetworkMasterDevice.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxSaWidgetWrapper_elements.h
  )

set(LOCAL_LIBS
  ${JVX_QT_LIBS}
  jvxLQtSaWidgets_static
  jvx-qt-helpers_static
)

if(JVX_WIDGET_WRAPPER_INFO_UPDATE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_WIDGET_WRAPPER_FIELD_INFO_UPDATE")
endif()
if(JVX_WIDGET_WRAPPER_PENDING_UPDATE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_DEBUG_DELAYED_CONTROL")
endif()

set(USE_QT_LIBRARIES TRUE)
set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
