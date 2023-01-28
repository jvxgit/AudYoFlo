cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-component-widgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(USE_QT_LIBRARIES TRUE)

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_master_dev.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_master_dev.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_slave_dev.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_slave_dev.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_tech.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxQtComponentWidget_socket_tech.h
	# ${UI_HEADERS}

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxQtComponentWidgets.h
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  )

set(LOCAL_COMPILE_DEFINITIONS "COMPILE_FOR_CONSOLE")

set(LOCAL_LIBS
  jvx-qt-helpers_static
  jvxLQtSaWidgetWrapper_static
  jvxLQtSaWidgets_static
  )

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
