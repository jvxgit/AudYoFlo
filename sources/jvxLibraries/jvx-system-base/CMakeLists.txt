cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-system-base)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
  
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_ns.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_small.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxBinaryFastDataControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConfiguration.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConfigurationExtender.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConfigurationLine.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConfigurationAttach.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConnectionMaster.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxConnector.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxDataConnections.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxEventLoop.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxEventLoopObject.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxFactoryHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxHiddenInterface.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxHostInteraction.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxHostTypeHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxHttpApi.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxInformation.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxInterface.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxInterfaceFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxLinkDataControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxManipulate.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxNode.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxObject.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxPrintf.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxPropertyAttach.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxReport.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxReportOnConfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxScheduler.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxSequencer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxSequencerControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxStateMachine.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxAutoDataConnect.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxMainComponent.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxSimpleComponent.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxBootSteps.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxCommandInterpreter.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/console/IjvxEventLoop_backend_ctrl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/console/IjvxEventLoop_frontend_ctrl.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/hosts/IjvxMainWindowControl.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/properties_hosts/IjvxAccessProperties.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt/IjvxQtSpecificWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt/IjvxQtWidgetFrameDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt/mainWindow_UiExtension.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/all-hosts/configHostFeatures_common.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/all-hosts/jvxHostHookupEntries.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt-develop-host/configureQtDevelopHost_features.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt-factory-host/configureQtFactoryHost_features.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxConfigProcessor.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxConnection.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxCrypt.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxDataLogger.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxDataLogReader.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxExternalCall.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxExternalCallTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxPacketFilter.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxRemoteCall.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxText2Speech.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxSystemTextLog.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxLocalTextLog.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxThreadController.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxToolsHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/tools/IjvxWebServer.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtender.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/platform/${JVX_OS}/jvx_platform.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxAccess.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxBinaryFastDataControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxBitFieldN.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxConfiglines.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxConfigTokens.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxConnector.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxDataConnections.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxLinkDataDescriptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxEventLoop.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxEventLoopElement.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxHttpApi.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxManipulate.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxProperties.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertyExternalBuffer.h 
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertyDescriptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertyAddress.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertyContainer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertyRawPointer.h 
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxPropertySpecifics.h 

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxReport.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxSequencer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxSequencer_cpp.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxSequencerControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxTypes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxTypes_cpp.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxTypes_str.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxValue.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxVariant.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TjvxReportCommandRequest.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/property-extender/TjvxPropertyExtender.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/console/TjvxFrontendBackend.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/hosts/TjvxMainWindowControl.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/misc/TjvxCircBufferDataCollector.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/network/TjvxProtocol.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxConfigProcessor.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxConnection.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxDataLogger.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxExternalCall.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxPacketFilter.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxRemoteCall.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxRs232.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxSocket.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxText2Speech.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxTextLog.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/tools/TjvxWebServer.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/typedefs/TjvxTypes_cpp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/typedefs/TjvxTypes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${JVX_OS}/jvx_system_specific.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
if(JVX_RELEASE_SDK)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_sdk.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_sdk.h)
	set(LOCAL_ADDITIONAL_INSTALL_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/jvx_sdk.h)
	## install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jvx_sdk.h DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_IMPORT} COMPONENT ${INSTALL_COMPONENT})
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
