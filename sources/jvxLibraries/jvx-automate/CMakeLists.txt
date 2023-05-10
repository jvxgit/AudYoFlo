cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-automate)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
find_host_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-automate ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModules.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModules.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAutomationReport.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxAutomationReport.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAutomationReportConnect.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxAutomationReportConnect.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModulesSrc2Snk.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModulesSrc2Snk.cpp  

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModulesSrc2SnkPreChain.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModulesSrc2SnkPreChain.cpp  

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModulesSyncedIo.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModulesSyncedIo.cpp  

	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfAutomationModuleHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAutomationModuleHandler.cpp
  )

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
