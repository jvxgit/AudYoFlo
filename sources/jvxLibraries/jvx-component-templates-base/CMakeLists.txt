if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-component-templates-base)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES

${CMAKE_CURRENT_SOURCE_DIR}/include/jvxDevices/CjvxNBareTaskDevice.h
${CMAKE_CURRENT_SOURCE_DIR}/src/jvxDevices/CjvxNBareTaskDevice.cpp

${CMAKE_CURRENT_SOURCE_DIR}/include/jvxDevices/CjvxMaster_Ntask.h
${CMAKE_CURRENT_SOURCE_DIR}/src/jvxDevices/CjvxMaster_Ntask.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxCompNode1io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxBareNode1io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNodeBase1io.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNodeBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxBareNode1ioRearrange.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNodeBaseNtask.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxBareNodeNtask.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxBareNtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/Cjvx_logfile_core.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxBareNode1io_logtofile.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxCompNode1io.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxBareNode1io.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNodeBase1io.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNodeBase.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxBareNode1ioRearrange.h
 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNodeBaseNtask.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxBareNodeNtask.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxBareNtask.h

   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/Cjvx_logfile_core.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxBareNode1io_logtofile.h
  
	## ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxMaster_Ntask.h
	## ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxInputOutputConnector_Ntask.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxInputOutputConnector_NVtask.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxInputOutputConnector_Ntask.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxInputOutputConnector_Vtask.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxInputOutputConnector_NVtask.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxInputOutputConnector_Vtask.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxInputOutputConnector_Ntask.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSignalProcessingNodes/CjvxSignalProcessingNode.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAppCtrTech/CjvxAppCtrDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAppCtrTech/CjvxAppCtrDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAppCtrTech/CjvxAppCtrTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAppCtrTech/CjvxAppCtrTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAppCtrNodes/CjvxAppCtrNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAppCtrNodes/CjvxAppCtrNode.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxAudioNode.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxAuNTasks.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxAudioNodeBase.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioNodes/CjvxBareAudioNode.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAuNTasks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAudioNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAudioNode_logfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxAudioNodeBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioNodes/CjvxBareAudioNode.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioTechnologies/CjvxAudioDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioTechnologies/CjvxAudioDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioTechnologies/CjvxAudioMasterDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioTechnologies/CjvxAudioMasterDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxAudioTechnologies/CjvxAudioTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioTechnologies/CjvxAudioTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioTechnologies/CjvxMixedDevicesAudioTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxAudioTechnologies/CjvxMixedDevicesAudioDevice.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxCustomTechnologies/CjvxCustomDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxCustomTechnologies/CjvxCustomDevice.h	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxCustomTechnologies/CjvxCustomTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxCustomTechnologies/CjvxCustomTechnology.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxSignalProcessingTechnologies/CjvxSignalProcessingDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSignalProcessingTechnologies/CjvxSignalProcessingDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxSignalProcessingTechnologies/CjvxSignalProcessingTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSignalProcessingTechnologies/CjvxSignalProcessingTechnology.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxSignalProcessingNodes/CjvxSignalProcessingNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSignalProcessingNodes/CjvxSignalProcessingNode.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVideoNodes/CjvxVideoNodeTpl.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVideoTechnologies/CjvxVideoDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVideoTechnologies/CjvxVideoDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVideoTechnologies/CjvxVideoTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVideoTechnologies/CjvxVideoTechnology.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxMinHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxConnectionMinHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxFullHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxInterfaceHostTpl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxComponentHost.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxComponentHostTools.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxFactoryHostTpl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxComConHostTpl.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxMinHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxConnectionMinHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxFullHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxHost-tpl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxHost_components-tpl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxComponentHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxComponentHostTools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxFullHost_callbacks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxFullHost_components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxFullHost_connections.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxFullHost_sequencer.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxUniqueId.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxUniqueId.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxConnectorBridge.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionRule.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionRule.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnections.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnections.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnections_types.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionsCommon.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionsCommon.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionsProcess.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionsProcess.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionsGroup.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionsGroup.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionsGroup_local_master.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionsGroup_local_master.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxDataConnectionsGroup_inout.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxDataConnectionsGroup_inout.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxHostInteraction.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxHostInteraction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxHostInteractionTools.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxHosts/CjvxHostTypeHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxHosts/CjvxHostTypeHandler.cpp

	# These are template classes
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxFactoryHosts/CjvxDefaultInterfaceFactory.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxFactoryHosts/CjvxInterfaceFactory.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTools/jvxPacketFilterRules/CjvxPacketFilterRule.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxTools/jvxPacketFilterRules/CjvxPacketFilterRule.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericConnectionTechnologies/CjvxGenericConnectionDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericConnectionTechnologies/CjvxGenericConnectionDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h

	# This is now a template
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericConnectionTechnologies/CjvxGenericConnectionTextDevice.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericRs232Technologies/CjvxGenericRs232Technology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericRs232Technologies/CjvxGenericRs232Device.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericRs232Technologies/CjvxGenericRs232Technology.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericRs232Technologies/CjvxGenericRs232Device.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericSocketTechnologies/CjvxGenericSocketTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericSocketTechnologies/CjvxGenericSocketTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxGenericSocketTechnologies/CjvxGenericSocketDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxGenericSocketTechnologies/CjvxGenericSocketDevice.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxDevices/CjvxBareDeviceNtask.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxDevices/CjvxBareDeviceNtask.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/jvxCommonTypedefs.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSingleConnectorCommon.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSingleInputConnector.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSingleInputConnector.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSingleOutputConnector.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSingleOutputConnector.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxConnectorCollection.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSingleMaster.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxSingleMaster.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxDataChainInterceptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxDataChainInterceptor.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxDataChainInterceptorGroup.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxDataChainInterceptorGroup.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxNegotiate.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/common/CjvxNegotiate.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSimplePropsPars.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/common/CjvxSimplePropsParsPlusOutChannel.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTechnologies/CjvxFullMasterDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxTechnologies/CjvxFullMasterDevice.cpp
	## ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTechnologies/CjvxTechnologyDevCaps.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTechnologies/CjvxTemplateTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTechnologies/CjvxTemplateTechnologyVD.h	

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxTechnologies/CjvxAudioWithVideoDevice.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxTechnologies/CjvxAudioWithVideoDevice.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNVTasks.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNVTasks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNVTasks_establish.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNVTasks_apis.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNVTasks_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxNodes/CjvxNVTasks_properties.cpp

	)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
	${CMAKE_CURRENT_BINARY_DIR}/generated
  )

set(LOCAL_PCG_FILES
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/common/CjvxSimplePropsAss_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/common/CjvxChannelPlusAss_pcg.pcg

	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNode_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNode_rearrange_pcg.pcg
	 #${CMAKE_CURRENT_SOURCE_DIR}/pcg/
	 #${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSignalProcessingNodes/CjvxSignalProcessingNode_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNVTasks_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioNodes/CjvxAuNTasks_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioNodes/CjvxAudioNode_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioNodes/CjvxAudioNode_logfile_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNode_logfile_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAppCtrTech/CjvxAppCtrDevice_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxCustomTechnologies/CjvxCustomDevice_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioTechnologies/CjvxAudioDevice_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxAudioTechnologies/CjvxAudioMasterDevice_pcg.pcg
	 ## ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxTechnologies/CjvxTechnologyDevCaps_pcg.pcg
	 ## ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxDevices/CjvxDeviceCaps_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxSignalProcessingTechnologies/CjvxSignalProcessingDevice_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxSignalProcessingNodes/CjvxSignalProcessingNode_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxVideoNodes/CjvxVideoNode_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxVideoTechnologies/CjvxVideoDevice_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxDataConnection/CjvxDataConnectionProcess_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxDataConnection/CjvxDataConnectionGroup_pcg.pcg

	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxHosts/CjvxHost_pcg.pcg
	 
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxTools/jvxPacketFilterRules/CjvxPacketFilterRule_pcg.pcg
	 
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericConnectionTechnologies/CjvxGenericConnectionDevice_pcg.pcg
	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericConnectionTechnologies/CjvxGenericConnectionTextDevice_pcg.pcg

	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericRs232Technologies/CjvxGenericRs232Technology_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericRs232Technologies/CjvxGenericRs232Device_pcg.pcg

	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericSocketTechnologies/CjvxGenericSocketTechnology_pcg.pcg
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericSocketTechnologies/CjvxGenericSocketDevice_pcg.pcg

	${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxTechnologies/CjvxFullMasterDevice_pcg.pcg
 )

set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	-I ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxHosts
	-I ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/pcg/jvxLogToFile
	-I ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxDevices
	-I ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxGenericRs232Technologies
	)

## Add some dependencies which cmake seems not to find itsel (pcg -> header -> other header) 
## add_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/include/jvxNodes/CjvxNodeBase1io.h ${CMAKE_CURRENT_SOURCE_DIR}/pcg/jvxNodes/CjvxNode_pcg.pcg)
	
set(LOCAL_LIBS jvx-dsp-base-cpp_static jvxLthreads_static)

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
