cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-system-product)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
 
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_product.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/IjvxExternalAudioChannels.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxCommunication/IjvxAudioCodec.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxCommunication/IjvxAudioDecoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxCommunication/IjvxAudioEncoder.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxTools/IjvxDataConverter.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxTools/IjvxResampler.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxTools/IjvxRtAudioFileReader.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/jvxTools/IjvxRtAudioFileWriter.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/qt-audio-host/configureQtAudioHost_features.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/helpers/HpjvxCast.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtenderStreamAudioPlugin.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtenderHrtfDispenser.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtenderChainControl.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtenderHeadTrackerProvider.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces/property-extender/IjvxPropertyExtenderSpatialDirectionDirect.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxBinaryFastDataControl_ids.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxConfiglines.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxConfigTokens.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxProperties_entries.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_components.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_componentsubtypes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_dataProcessorHintDescriptor.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_dataProcessorHintTypes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_dataGroups.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_datatypes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_errorcodes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_interfaces.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_linkdata.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/TpjvxTypes_tools.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/property-extender/TpjvxPropertyExtender.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/jvxTools/TjvxAudioFileIo.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/jvxTools/TjvxGenericWrapperApi.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/typedefs/config-host/configHostFeatures_common.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/config-host/configHostFeatures_common.cpp
  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
