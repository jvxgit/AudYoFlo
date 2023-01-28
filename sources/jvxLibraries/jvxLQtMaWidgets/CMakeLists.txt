set(PROJECT_NAME jvxLQtMaWidgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${JVX_SUBPRODUCT_ROOT}/software/helpers/
  ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
  ${CMAKE_CURRENT_BINARY_DIR}
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt/qwt/src
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtMaLogFileWriter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxQtMaLogFileWriter.h
  ${UI_HEADERS}
  )

set(LOCAL_LIBS
	jvxLQtSaWidgetWrapper_static
	jvxLQtSaWidgets_static
	qwt_static 
)

set(USE_QT_LIBRARIES TRUE)
set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
