cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-qt-widgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(USE_QT_LIBRARIES TRUE)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxCircbufferPlot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxCircbufferPlot.h
	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/third_party/web/qcustomplot/qcustomplot
  )
  
 set(LOCAL_LIBS 
	 qcustomplot_static
 )

set(BUILD_STATIC TRUE)
set(LOCAL_ADDITIONAL_INSTALL_PATH "src")
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
