#ifndef __HJVXCAST_H__
#define __HJVXCAST_H__

#include "helpers/HpjvxCast.h"

template <typename T>
jvxInterfaceType getInterfaceType()
{
	jvxInterfaceType tp = JVX_INTERFACE_UNKNOWN;
	/*
		JVX_INTERFACE_CONNECTOR_FACTORY,
		JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
		JVX_INTERFACE_CONFIGURATION,
		JVX_INTERFACE_PROPERTIES,
		JVX_INTERFACE_SEQUENCERCONTROL,
		JVX_INTERFACE_SEQUENCER,
		JVX_INTERFACE_CONFIGURATIONLINE,
		JVX_INTERFACE_PROPERTY_POOL,
		JVX_INTERFACE_SCHEDULE,
		JVX_INTERFACE_HOST,
		JVX_INTERFACE_HOSTTYPEHANDLER,
		JVX_INTERFACE_EVENTLOOP,
		JVX_INTERFACE_QT_WIDGET_HOST,
		JVX_INTERFACE_QT_WIDGET_UI,
		JVX_INTERFACE_BINARY_FAST_DATA_CONTROL,
		JVX_INTERFACE_DATAPROCESSOR_CONNECTOR,
		JVX_INTERFACE_REMOTE_CALL_ASYNC,
		JVX_INTERFACE_PRINTF,
		JVX_INTERFACE_CONFIGURATION_EXTENDER,
		JVX_INTERFACE_DATA_CONNECTIONS,
		JVX_INTERFACE_AUTO_DATA_CONNECT,
		JVX_INTERFACE_HTTP_API,
		JVX_INTERFACE_MANIPULATE,
		JVX_INTERFACE_COMMAND_LINE,
		JVX_INTERFACE_BOOT_STEPS
		*/
	if (std::is_same<T, IjvxReport>::value)
	{
		tp = JVX_INTERFACE_REPORT;
	}
	else if (std::is_same<T, IjvxReportSystem>::value)
	{
		tp = JVX_INTERFACE_REPORT_SYSTEM;
	}
	else if (std::is_same<T, IjvxToolsHost>::value)
	{
		tp = JVX_INTERFACE_TOOLS_HOST;
	}
	else if (std::is_same<T, IjvxConnectorFactory>::value)
	{
		tp = JVX_INTERFACE_CONNECTOR_FACTORY;
	}
	else if (std::is_same<T, IjvxConnectionMasterFactory>::value)
	{
		tp = JVX_INTERFACE_CONNECTOR_MASTER_FACTORY;
	}
	else if (std::is_same<T, IjvxConfiguration>::value)
	{
		tp = JVX_INTERFACE_CONFIGURATION;
	}
	else if (std::is_same<T, IjvxProperties>::value)
	{
		tp = JVX_INTERFACE_PROPERTIES;
	}
	else if (std::is_same<T, IjvxSequencerControl>::value)
	{
		tp = JVX_INTERFACE_SEQUENCERCONTROL;
	}
	else if (std::is_same<T, IjvxSequencer>::value)
	{
		tp = JVX_INTERFACE_SEQUENCER;
	}
	else if (std::is_same<T, IjvxConfigurationLine>::value)
	{
		tp = JVX_INTERFACE_CONFIGURATIONLINE;
	}
	else if (std::is_same<T, IjvxPropertyPool>::value)
	{
		tp = JVX_INTERFACE_PROPERTY_POOL;
	}
	else if (std::is_same<T, IjvxSchedule>::value)
	{
		tp = JVX_INTERFACE_SCHEDULE;
	}
	else if (std::is_same<T, IjvxHost>::value)
	{
		tp = JVX_INTERFACE_HOST;
	}
	else if (std::is_same<T, IjvxHostTypeHandler>::value)
	{
		tp = JVX_INTERFACE_HOSTTYPEHANDLER;
	}
	else if (std::is_same<T, IjvxEventLoop>::value)
	{
		tp = JVX_INTERFACE_EVENTLOOP;
	}
	/*
	else if (std::is_same<T, IjvxQtWidgetHost>:value)
	{
		tp = JVX_INTERFACE_QT_WIDGET_HOST;
	}
	else if (std::is_same<T, IjvxQtWidgetUi>:value)
	{
		tp = JVX_INTERFACE_QT_WIDGET_UI;
	}
	*/
	else if (std::is_same<T, IjvxBinaryFastDataControl>::value)
	{
		tp = JVX_INTERFACE_BINARY_FAST_DATA_CONTROL;
	}
	/*
	else if (std::is_same<T, IjvxPr>::value)
	{
		tp = JVX_INTERFACE_DATAPROCESSOR_CONNECTOR;
	}
	*/
	else if (std::is_same<T, IjvxRemoteCall_async>::value)
	{
		tp = JVX_INTERFACE_REMOTE_CALL_ASYNC;
	}
	else if (std::is_same<T, IjvxPrintf>::value)
	{
		tp = JVX_INTERFACE_PRINTF;
	}
	else if (std::is_same<T, IjvxConfigurationExtender>::value)
	{
		tp = JVX_INTERFACE_CONFIGURATION_EXTENDER;
	}
	else if (std::is_same<T, IjvxConfigurationDone>::value)
	{
		tp = JVX_INTERFACE_CONFIGURATION_DONE;
	}
	else if (std::is_same<T, IjvxDataConnections>::value)
	{
		tp = JVX_INTERFACE_DATA_CONNECTIONS;
	}
	else if (std::is_same<T, IjvxToolsHost>::value)
	{
		tp = JVX_INTERFACE_AUTO_DATA_CONNECT;
	}
	else if (std::is_same<T, IjvxHttpApi>::value)
	{
		tp = JVX_INTERFACE_HTTP_API;
	}
	else if (std::is_same<T, IjvxManipulate>::value)
	{
		tp = JVX_INTERFACE_MANIPULATE;
	}
	else if (std::is_same<T, IjvxCommandLine>::value)
	{
		tp = JVX_INTERFACE_COMMAND_LINE;
	}
	else if (std::is_same<T, IjvxBootSteps>::value)
	{
		tp = JVX_INTERFACE_BOOT_STEPS;
	}
	else if (std::is_same<T, IjvxCommandInterpreter>::value)
	{
		tp = JVX_INTERFACE_COMMAND_INTERPRETER;
	}
	return tp;
}

template <typename T>
T* reqInterface(IjvxHiddenInterface* refObj)
{
	jvxInterfaceType tp = getInterfaceType<T>();
	T* theif = nullptr;
	if (refObj)
	{
		if (tp != JVX_INTERFACE_UNKNOWN)
		{
			refObj->request_hidden_interface(tp, reinterpret_cast<jvxHandle**>(&theif));
		}
	}
	return theif;
}

template <typename T>
T* reqInterfaceObj(IjvxObject* obj)
{
	T* theif = nullptr;
	IjvxInterfaceFactory* ifac = nullptr;
	obj->interface_factory(&ifac);
	if (ifac)
	{
		theif = reqInterface<T>(static_cast<IjvxHiddenInterface*>(ifac));
	}
	return theif;
}

template <typename T>
void retInterface(IjvxHiddenInterface* refObj, T* theif)
{
	jvxInterfaceType tp = getInterfaceType<T>();
	if (refObj)
	{
		if (tp != JVX_INTERFACE_UNKNOWN)
		{
			refObj->return_hidden_interface(tp, reinterpret_cast<jvxHandle*>(theif));
		}
	}
}

template <typename T>
void retInterfaceObj(IjvxObject* obj, T* theif)
{
	IjvxInterfaceFactory* ifac = nullptr;
	obj->interface_factory(&ifac);
	if (ifac)
	{
		retInterface<T>(static_cast<IjvxHiddenInterface*>(ifac), theif);
	}
}

	// =====================================================================================

template <typename T>
jvxBool checkComponentType(jvxComponentType tp)
{
	jvxBool result = false;
	switch (tp)
	{
	case JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY:
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
	case JVX_COMPONENT_VIDEO_TECHNOLOGY:
	case JVX_COMPONENT_CUSTOM_TECHNOLOGY:
	case JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY:
#ifdef JVX_COMPONENT_CASE_TECHNOLOGY
	JVX_COMPONENT_CASE_TECHNOLOGY
#endif

		if (std::is_same<T, IjvxTechnology>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE:
	case JVX_COMPONENT_AUDIO_DEVICE:
	case JVX_COMPONENT_VIDEO_DEVICE:
	case JVX_COMPONENT_CUSTOM_DEVICE:
	case JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE:

#ifdef JVX_COMPONENT_CASE_DEVICE
	JVX_COMPONENT_CASE_DEVICE
#endif
		if (std::is_same<T, IjvxDevice>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
	case JVX_COMPONENT_AUDIO_NODE:
	case JVX_COMPONENT_VIDEO_NODE:
	case JVX_COMPONENT_CUSTOM_NODE:

#ifdef JVX_COMPONENT_CASE_NODE
	JVX_COMPONENT_CASE_NODE
#endif

		if (std::is_same<T, IjvxNode>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_MIN_HOST:
		if (std::is_same<T, IjvxMinHost>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_FACTORY_HOST:

		if (std::is_same<T, IjvxFactoryHost>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_HOST:
		if (std::is_same<T, IjvxHost>::value)
		{
			result = true;
		}
		break;

	case JVX_COMPONENT_EVENTLOOP:
		if (std::is_same<T, IjvxEventLoop>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_DATALOGGER:
		if (std::is_same<T, IjvxDataLogger>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_DATALOGREADER:
		if (std::is_same<T, IjvxDataLogReader>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CONNECTION:
		if (std::is_same<T, IjvxConnection>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		if (std::is_same<T, IjvxConfigProcessor>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_EXTERNAL_CALL:
		if (std::is_same<T, IjvxExternalCall>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_THREADCONTROLLER:
		if (std::is_same<T, IjvxThreadController>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_THREADS:
		if (std::is_same<T, IjvxThreads>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_SHARED_MEMORY:
		if (std::is_same<T, IjvxSharedMemory>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_TEXT2SPEECH:
		if (std::is_same<T, IjvxText2Speech>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
		if (std::is_same<T, IjvxTextLog>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_LOCAL_TEXT_LOG:
		if (std::is_same<T, IjvxLocalTextLog>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_CRYPT:
		if (std::is_same<T, IjvxCrypt>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_WEBSERVER:
		if (std::is_same<T, IjvxWebServer>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_REMOTE_CALL:
		if (std::is_same<T, IjvxRemoteCall>::value)
		{
			result = true;
		}
		break;
	case JVX_COMPONENT_PACKETFILTER_RULE:
		if (std::is_same<T, IjvxPacketFilterRule>::value)
		{
			result = true;
		}
		break;

		// Add all specific component types here
#ifdef JVX_COMPONENT_CAST_PRODUCT
		JVX_COMPONENT_CAST_PRODUCT
#endif
	case JVX_COMPONENT_SYSTEM_AUTOMATION:
		if (std::is_same<T, IjvxSimpleComponent>::value)
		{
			result = true;
		}
		break;
		
	default:
		assert(0);
	}
	return result;
}

// Convert an object to the main component reference
template <typename T>
T* castFromObject(IjvxObject* obj)
{
	T* retPtr = nullptr;
	jvxHandle* vPtr = nullptr;
	jvxComponentIdentification cTp = JVX_COMPONENT_UNKNOWN;
	if(obj)
	{
		obj->request_specialization(&vPtr, &cTp, nullptr);
		if (checkComponentType<T>(cTp.tp))
		{
			retPtr = (T*)vPtr;
		}
	}
	return retPtr;
}

template <typename T>
struct refComp
{
	T* cpPtr = nullptr;
	IjvxObject* objPtr = nullptr;
};

template <typename T>
refComp<T> reqRefTool(IjvxToolsHost* tHost, const jvxComponentIdentification& tp, jvxSize fId = 0, const char* fTag = nullptr, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE)
{
	refComp<T> inst;
	if (tHost)
	{
		if (checkComponentType<T>(tp.tp))
		{
			tHost->reference_tool(tp, &inst.objPtr, fId, fTag, filter_stateMask);
			if (inst.objPtr)
			{
				inst.objPtr->request_specialization(reinterpret_cast<jvxHandle**>(&inst.cpPtr), nullptr, nullptr);
			}
		}
	}
	return inst;
}

template <typename T>
void retRefTool(IjvxToolsHost* tHost, jvxComponentType tp, refComp<T>& inst)
{
	if (tHost)
	{
		if (checkComponentType<T>(tp))
		{
			tHost->return_reference_tool(tp, inst.objPtr);	
			inst.objPtr = nullptr;
			inst.cpPtr = nullptr;
		}
	}
}

template <typename T>
refComp<T> reqInstTool(IjvxToolsHost* tHost, jvxComponentType tp, jvxSize fId = 0, const char* fTag = nullptr)
{
	refComp<T> inst;
	if (tHost)
	{
		if (checkComponentType<T>(tp))
		{
			tHost->instance_tool(tp, &inst.objPtr, fId, fTag);
			if (inst.objPtr)
			{
				inst.objPtr->request_specialization(reinterpret_cast<jvxHandle**>(&inst.cpPtr), nullptr, nullptr);
			}
		}
	}
	return inst;
}

template <typename T>
jvxErrorType retInstTool(IjvxToolsHost* tHost, jvxComponentType tp, refComp<T>& inst, jvxSize fId = 0, const char* fTag = nullptr)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (tHost)
	{
		if (checkComponentType<T>(tp))
		{
			res = tHost->return_instance_tool(tp, inst.objPtr, fId, fTag);
			inst.objPtr = nullptr;
			inst.cpPtr = nullptr;
		}
	}
	return res;
}

// =====================================================================================

// Ok, I had this with a returned reference already. However, I prefer a pointer 
// to have some kind of type selection involved.
template <typename T>
const T* castCommandRequest(const CjvxReportCommandRequest& in)
{
	const T* ret = nullptr;
	jvxReportCommandDataType tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_BASE;

	if (std::is_same<T, CjvxReportCommandRequest_id>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT;
	}
	else if (std::is_same<T, CjvxReportCommandRequest_rm>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE;
	}
	else if (std::is_same<T, CjvxReportCommandRequest_uid>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID;
	}
	else if (std::is_same<T, CjvxReportCommandRequest_seq>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ;
	}
	else if (std::is_same<T, CjvxReportCommandRequest_ss>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS;
	}
	in.specialization(reinterpret_cast<const jvxHandle**>(&ret), tp);
	return ret;
}

// =====================================================================================

template <typename T>
T* castPropDescriptor(jvx::propertyDescriptor::IjvxPropertyDescriptor* in)
{
	T* ret = nullptr;
	if (!in)
		return ret;

	jvx::propertyDescriptor::descriptorEnum tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CORE;

	if (std::is_same<T, jvx::propertyDescriptor::CjvxPropertyDescriptorMin>::value)
	{
		tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_MIN;
	}
	else if (std::is_same<T, jvx::propertyDescriptor::CjvxPropertyDescriptorCore>::value)
	{
		tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CORE;
	}
	else if (std::is_same<T, jvx::propertyDescriptor::CjvxPropertyDescriptorControl>::value)
	{
		tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CONTROL;
	}
	else if (std::is_same<T, jvx::propertyDescriptor::CjvxPropertyDescriptorFull>::value)
	{
		tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL;
	}
	else if (std::is_same<T, jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus>::value)
	{
		tp = jvx::propertyDescriptor::descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL_PLUS;
	}

	jvxErrorType res = in->specialization(reinterpret_cast<jvxHandle**>(&ret), tp);
	if (res == JVX_NO_ERROR)
	{
		return ret;
	}
	return nullptr;
}

template <typename T>
T* castPropAddress(const jvx::propertyAddress::IjvxPropertyAddress* in)
{
	T* ret = nullptr;
	if (!in)
		return ret;

	jvx::propertyAddress::jvxPropertyAddressEnum tp = jvx::propertyAddress::jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_NONE;
	if (std::is_same<T, const jvx::propertyAddress::CjvxPropertyAddressLinear>::value)
	{
		tp = jvx::propertyAddress::jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_LINEAR;
	}
	else if (std::is_same<T, const jvx::propertyAddress::CjvxPropertyAddressDescriptor>::value)
	{
		tp = jvx::propertyAddress::jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_DESCRIPTOR;
	}
	else if (std::is_same<T, const jvx::propertyAddress::CjvxPropertyAddressGlobalId>::value)
	{
		tp = jvx::propertyAddress::jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_GLOBAL_INDEX;
	}

	jvxErrorType res = in->specialization(reinterpret_cast<const jvxHandle**>(&ret), tp);
	if (res == JVX_NO_ERROR)
	{
		return ret;
	}
	return nullptr;
}

template <typename T>
T* castPropSpecific(const jvx::propertySpecifics::IjvxConnectionType* in)
{
	T* ret = nullptr;
	if (!in)
		return ret;

	jvx::propertySpecifics::connectTypeEnum tp = jvx::propertySpecifics::connectTypeEnum::JVX_PROPERTY_SPECIFIC_NONE;
	if (std::is_same<T, const jvx::propertySpecifics::CjvxConnectionTypeDefault>::value)
	{
		tp = jvx::propertySpecifics::connectTypeEnum::JVX_PROPERTY_SPECIFIC_DEFAULT;
	}
	else if (std::is_same<T, const jvx::propertySpecifics::CjvxConnectionTypeHttp>::value)
	{
		tp = jvx::propertySpecifics::connectTypeEnum::JVX_PROPERTY_SPECIFIC_HTTP;
	}

	jvxErrorType res = in->specialization(reinterpret_cast<const jvxHandle**>(&ret), tp);
	if (res == JVX_NO_ERROR)
	{
		return ret;
	}
	return nullptr;
}

// =====================================================================================

template <typename T>
T* castPropRawPointer(const jvx::propertyRawPointerType::IjvxRawPointerType* ptrRaw, jvxDataFormat form)
{
	jvxErrorType res = JVX_NO_ERROR;
	T* ret = nullptr;
	if (!ptrRaw)
		return ret;

	jvx::propertyRawPointerType::elementType etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_NONE;
	jvx::propertyRawPointerType::apiType atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_NONE_SPECIFIC;

	if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<jvxExternalBuffer> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER_POINTER;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
	}
	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL;
	}

	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE_POINTER;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
	}
	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxPropertyExtender> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL;
	}

	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxLocalTextLog> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG_POINTER;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
	}
	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog> >::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL;
	}
	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeCallbackPointer>::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK_POINTER;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
	}
	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerTypeCallback>::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_CALLBACK;
		atp = jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL;
	}

	else if (std::is_same < T, const jvx::propertyRawPointerType::CjvxRawPointerType>::value)
	{
		etp = jvx::propertyRawPointerType::elementType::JVX_FIELD_RAW_POINTER_GENERIC;
	}

	res = ptrRaw->specialization(reinterpret_cast<const jvxHandle**>(&ret), etp, atp, form);
	if (res == JVX_NO_ERROR)
	{
		return ret;
	}
	return nullptr;
}

// =====================================================================================
template <class T>
T* castPropIfPointer(jvx::propertyRawPointerType::IjvxRawPointerType* iPtr, jvxDataFormat format)
{
	T** ptrMemIf = nullptr;
	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T>* ptrRawInstalled =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<T> >(
			iPtr, format);
	if (ptrRawInstalled)
	{
		ptrMemIf = ptrRawInstalled->typedPointer();
		if (ptrMemIf)
		{
			return *ptrMemIf;
		}
	}
	return nullptr;
}

template <class T>
T* castPropIfExtender(IjvxPropertyExtender* pExt)
{
	T* ptrRet = nullptr;
	if (0)
	{
	}

	// From sub project
	JVX_PROPERTY_EXTENDER_CAST_CASES
	return ptrRet;
}

// =====================================================================================

template <typename T>
T* castDataPointer(jvxHandle* in, jvxDataFormat tpIn)
{
	T* ret = nullptr;
	jvxDataFormat tp = JVX_DATAFORMAT_NONE;
#if __cplusplus >= 201703L
	if constexpr(std::is_same<T, jvxData>::value)
#else
	if (std::is_same<T, jvxData>::value)
#endif
	{
		tp = JVX_DATAFORMAT_DATA;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxInt8>::value)
#else
	else if (std::is_same<T, jvxInt8>::value)
#endif
	{
		tp = JVX_DATAFORMAT_8BIT;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxInt16>::value)
#else
	else if (std::is_same<T, jvxInt16>::value)
#endif
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxInt32>::value)
#else
	else if (std::is_same<T, jvxInt32>::value)
#endif
	{
		tp = JVX_DATAFORMAT_32BIT_LE;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxInt64>::value)
#else
	else if (std::is_same<T, jvxInt64>::value)
#endif
	{
		tp = JVX_DATAFORMAT_64BIT_LE;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxUInt8>::value)
#else
	else if (std::is_same<T, jvxUInt8>::value)
#endif
	{
		tp = JVX_DATAFORMAT_U8BIT;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxUInt16>::value)
#else
	else if (std::is_same<T, jvxUInt16>::value)
#endif
	{
		tp = JVX_DATAFORMAT_U16BIT_LE;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxUInt32>::value)
#else
	else if (std::is_same<T, jvxUInt32>::value)
#endif
	{
		tp = JVX_DATAFORMAT_U32BIT_LE;
	}
#if __cplusplus >= 201703L
	else if constexpr(std::is_same<T, jvxUInt64>::value)
#else
	else if (std::is_same<T, jvxUInt64>::value)
#endif
	{
		tp = JVX_DATAFORMAT_U64BIT_LE;
	}

	if(tp == tpIn)
	{
		ret = reinterpret_cast<T*>(in);
	}
	return ret;
}

template <typename T> jvxDataFormat template_get_type_enum()
{
	jvxDataFormat form = JVX_DATAFORMAT_NONE;

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxData>::value)
#else
	if (std::is_same<T, jvxData>::value)
#endif
	{
		form = JVX_DATAFORMAT_DATA;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt64>::value)
#else
	if (std::is_same<T, jvxUInt64>::value)
#endif
	{
		form = JVX_DATAFORMAT_U64BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt32>::value)
#else
	if (std::is_same<T, jvxUInt32>::value)
#endif
	{
		form = JVX_DATAFORMAT_U32BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt16>::value)
#else
	if (std::is_same<T, jvxUInt16>::value)
#endif
	{
		form = JVX_DATAFORMAT_U16BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxUInt8>::value)
#else
	if (std::is_same<T, jvxUInt8>::value)
#endif
	{
		form = JVX_DATAFORMAT_U8BIT;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt64>::value)
#else
	if (std::is_same<T, jvxInt64>::value)
#endif
	{
		form = JVX_DATAFORMAT_64BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt32>::value)
#else
	if (std::is_same<T, jvxInt32>::value)
#endif
	{
		form = JVX_DATAFORMAT_32BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt16>::value)
#else
	if (std::is_same<T, jvxInt16>::value)
#endif
	{
		form = JVX_DATAFORMAT_16BIT_LE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxInt8>::value)
#else
	if (std::is_same<T, jvxInt8>::value)
#endif
	{
		form = JVX_DATAFORMAT_8BIT;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxByte>::value)
#else
	if (std::is_same<T, jvxByte>::value)
#endif
	{
		form = JVX_DATAFORMAT_BYTE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxSize>::value)
#else
	if (std::is_same<T, jvxSize>::value)
#endif
	{
		form = JVX_DATAFORMAT_SIZE;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxApiString>::value)
#else
	if (std::is_same<T, jvxApiString>::value)
#endif
	{
		form = JVX_DATAFORMAT_STRING;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxSelectionList>::value)
#else
	if (std::is_same<T, jvxSelectionList>::value)
#endif
	{
		form = JVX_DATAFORMAT_SELECTION_LIST;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxApiStringList>::value)
#else
	if (std::is_same<T, jvxApiStringList>::value)
#endif
	{
		form = JVX_DATAFORMAT_STRING_LIST;
	}

#if __cplusplus >= 201703L
	if constexpr (std::is_same<T, jvxValueInRange>::value)
#else
	if (std::is_same<T, jvxValueInRange>::value)
#endif
	{
		form = JVX_DATAFORMAT_VALUE_IN_RANGE;
	}

	return form;
};

// Some template helper functions
template <class T> T** jvx_process_icon_extract_input_buffers(
	jvxLinkDataDescriptor* theData,
	jvxSize idx_stage)
{
	jvxDataFormat form = template_get_type_enum<T>();
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
	{
		idx_stage_local = *theData->con_pipeline.idx_stage_ptr;
	}
	
	// Check type if it is not a jvxHandle type
	if (!std::is_same<T, jvxHandle>::value)
	{
		assert(theData->con_params.format == form);
	}
	T** bufsIn = (T**)theData->con_data.buffers[idx_stage_local];
	return bufsIn;
};

template <class T> T** jvx_process_icon_extract_output_buffers(
	jvxLinkDataDescriptor* theData)
{
	jvxDataFormat form = template_get_type_enum<T>();

	jvxSize idx_stage_local = *theData->con_pipeline.idx_stage_ptr;
	T** bufsOut = (T**)theData->con_data.buffers[idx_stage_local];
	if (!std::is_same<T, jvxHandle>::value)
	{
		assert(theData->con_params.format == form);
	}
	return bufsOut;
}

#endif
