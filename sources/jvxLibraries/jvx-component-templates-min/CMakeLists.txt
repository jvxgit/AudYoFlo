cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-component-templates-min)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
 
	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxConfigurationLine.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxConfigurationLine.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxConnectionMaster.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxConnectionMaster.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxConnectorFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxConnectorFactory.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxConnectorMasterFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxConnectorMasterFactory.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxInputConnectorCore.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxInputConnectorCore.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxOutputConnectorCore.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxOutputConnectorCore.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxInputOutputConnectorCore.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxInputOutputConnectorCore.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxInputOutputConnector.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxInputOutputConnector.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxInputConnectorLink.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxInputConnectorLink.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxOutputConnectorLink.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxOutputConnectorLink.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxObjectLog.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxObjectMin.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxObjectMin.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxObject.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxObject.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxProperties.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxPropertiesTypeConvert.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxPropertiesTypeConvert.cpp

	## ${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxPropertiesP.h
	## ${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxPropertiesP.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxPropertiesSimple.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxPropertiesSimple.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxRealtimeViewer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxRealtimeViewer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxReportMessageQueue.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxReportMessageQueue.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSequencer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSequencer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSequencerControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSequencerControl.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxTechnology.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxWebServerHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/common/TjvxWebServerHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxWebServerHost.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSimpleMasterDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSimpleMasterDevice.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxDevice.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/commandline/CjvxCommandLine.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/commandline/CjvxCommandLine.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxPropertyPool.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxPropertyPool.cpp

  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  )
set(LOCAL_LIBS jvxLthreads_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
