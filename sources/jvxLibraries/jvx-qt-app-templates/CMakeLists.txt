cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-app-templates)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory_ww.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory_ww_http.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory_ww.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_hostfactory_ww_http.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww_log.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww_log.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww_log_web.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mainCentral_host_ww_log_web.h
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgets/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtSaWidgetWrapper/src
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLQtMaWidgets/src
  )

set(LOCAL_LIBS
  jvx-qt-helpers_static
  jvxLQtSaWidgetWrapper_static
  jvxLQtSaWidgets_static
  jvxLQtMaWidgets_static
  )

set(USE_QT_LIBRARIES TRUE)
set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
