set(PROJECT_NAME jvxSpTDeploySignal)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingDeviceDeploy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingDeviceDeploy_deploy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingDeviceDeploy.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingTechnologyDeploy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingTechnologyDeploy.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/IjvxSignalProcessingDeploy.h
  )
	
set(LOCAL_LIBS 
  jvx-component-templates-base_static
  )
  
set(LOCAL_COMPILE_DEFINITIONS "USE_STANDALONE_SKELETON")

set(LOCAL_PCG_FILES
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/CjvxSignalProcessingDeviceDeploy.pcg)

set(IS_JVX_COMPONENT TRUE)
set(JVX_PROPERTY_ACCESS_HEADER_COMPONENT ${CMAKE_CURRENT_SOURCE_DIR}/include/IjvxSignalProcessingDeploy.h)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

