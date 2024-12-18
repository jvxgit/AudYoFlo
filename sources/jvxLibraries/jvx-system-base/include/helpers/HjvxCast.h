#ifndef __HJVXCAST_H__
#define __HJVXCAST_H__

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
	else if (std::is_same<T, IjvxPackage>::value)
	{
		tp = JVX_INTERFACE_PACKAGE;
	}
	else if (std::is_same<T, IjvxComponentHostExt>::value)
	{
		tp = JVX_INTERFACE_COMPONENT_HOST_EXT;
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
	else if (std::is_same<T, CjvxReportCommandRequest_ss_id>::value)
	{
		tp = jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS_ID;
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
const T* ccastPropDescriptor(const jvx::propertyDescriptor::IjvxPropertyDescriptor* in)
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

	// This is a dirty hack: discarding const in in pointer and in function argument. However, we 
	// only want ONE version of function "specialization". As we return the result as a const 
	// pointer, we can do this without violating any const condition. Note that an "interpret_cast"
	// can not be used as it does not allow const to non-const conversion.
	jvxErrorType res = const_cast<jvx::propertyDescriptor::IjvxPropertyDescriptor*>(in)->specialization(reinterpret_cast<jvxHandle**>(const_cast<T**>(&ret)), tp);
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

#endif
