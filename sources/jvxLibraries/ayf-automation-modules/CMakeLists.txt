cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME ayf-automation-modules)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src

  # ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
  # ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-shared-connection-host/include
  )

set(LOCAL_SOURCES

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModules.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModulesSrc2Snk.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModulesSrc2Snk.cpp  

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModulesSrc2SnkTrigger.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModulesSrc2SnkTrigger.cpp  
)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
#set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
