#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::property_request_handle(jvxComponentIdentification cpId,
	IjvxObject** obj_ptr,
	IjvxProperties** prop_ptr)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	IjvxObject* objPtr = nullptr;
	IjvxProperties* propPtr = nullptr;
	if (involvedHost.hHost)
	{
		involvedHost.hHost->request_object_selected_component(cpId, &objPtr);
		if (objPtr)
		{
			propPtr = reqInterfaceObj<IjvxProperties>(objPtr);
			if (propPtr)
			{
				*obj_ptr = objPtr;
				*prop_ptr = propPtr;

				auto elm = checkedOutPropReferences.find(cpId);
				if (elm != checkedOutPropReferences.end())
				{
					assert(objPtr == elm->second.ptrObj);
					assert(propPtr == elm->second.ptrProp);
					elm->second.refCnt++;
				}
				else
				{
					oneCheckoutPropReference newRef;
					newRef.refCnt++;
					newRef.ptrObj = objPtr;
					newRef.ptrProp = propPtr;
					checkedOutPropReferences[cpId] = newRef;
				}
				res = JVX_NO_ERROR;				
			}
		}
	}
	return res;
}

jvxErrorType 
jvxLibHost::property_return_handle(jvxComponentIdentification cpId,
	IjvxObject* obj_ptr,
	IjvxProperties* prop_ptr)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	if (involvedHost.hHost)
	{
		// Here, we return a property reference. Typically the property reference 
		// has been removed from list in a previous unselect
		if (obj_ptr)
		{
			auto elm = checkedOutPropReferences.find(cpId);
			if (elm != checkedOutPropReferences.end())
			{
				assert(obj_ptr == elm->second.ptrObj);
				assert(prop_ptr == elm->second.ptrProp);
				elm->second.refCnt--;
				if (elm->second.refCnt == 0)
				{
					checkedOutPropReferences.erase(elm);
				}
			}
			res = JVX_NO_ERROR;
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::verify_property_handle(
	jvxComponentIdentification cpId,
	IjvxObject* obj_ptr,
	IjvxProperties* prop_ptr)
{
	auto elm = checkedOutPropReferences.find(cpId);
	if (elm != checkedOutPropReferences.end())
	{
		assert(obj_ptr == elm->second.ptrObj);
		assert(prop_ptr == elm->second.ptrProp);
		return JVX_NO_ERROR;		
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
jvxLibHost::set_property(
	jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
	IjvxObject* objPtr, IjvxProperties* propPtr,	
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans,
	AyfBackendReportPropertySet propSetType)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (propPtr && objPtr)
	{
		jvxErrorType res = verify_property_handle(cpId, objPtr, propPtr);
		if (res == JVX_NO_ERROR)
		{
			switch (propSetType)
			{
			case AYF_BACKEND_REPORT_SET_PROPERTY_ACTIVE:
				callGate.on_set.report_set = true;
				break;

			case AYF_BACKEND_REPORT_SET_PROPERTY_INACTIVE:
				callGate.on_set.report_set = false;
				break;
			}
			res = propPtr->set_property(callGate, rawPtr, ident, trans);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::start_property_set_collect(jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
	IjvxObject* objPtr, IjvxProperties* propPtr)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (propPtr && objPtr)
	{
		jvxErrorType res = verify_property_handle(cpId, objPtr, propPtr);
		if (res == JVX_NO_ERROR)
		{
			res = propPtr->start_property_report_collect(callGate);
		}
	}
	return res;
}

jvxErrorType
jvxLibHost::stop_property_set_collect(jvxCallManagerProperties& callGate, jvxComponentIdentification cpId,
	IjvxObject* objPtr, IjvxProperties* propPtr)
{
	jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
	if (propPtr && objPtr)
	{
		jvxErrorType res = verify_property_handle(cpId, objPtr, propPtr);
		if (res == JVX_NO_ERROR)
		{
			res = propPtr->stop_property_report_collect(callGate);
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(jvxLibHost, cb_set_options) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::report_properties_modified(const char* props_set)
{
	//
	// 
	// 
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::set_request_command_handler(CjvxHandleRequestCommands_callbacks* refFwdReqCommands)
{
	reqHandle.initialize_fwd_link(refFwdReqCommands);
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxLibHost::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename , jvxInt32 lineno )
{
	std::vector<std::string> warns;
	genLibHost::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe, &warns);
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	genLibHost::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	return JVX_NO_ERROR;
}