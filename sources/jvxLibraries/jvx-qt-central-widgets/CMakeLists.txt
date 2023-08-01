if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-central-widgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_UI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/uisrc/jvx_property_tree_widget.ui
   )
set(LOCAL_UI_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/uisrc)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtCentralWidgets.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_property_tree_widget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_property_tree_widget.h
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/interfaces/qt

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ## ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
  jvx-qt-helpers_static
  jvxLQtSaWidgetWrapper_static
  )

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
