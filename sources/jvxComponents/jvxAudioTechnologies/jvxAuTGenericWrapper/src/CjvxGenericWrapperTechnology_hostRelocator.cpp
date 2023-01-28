#include "CjvxGenericWrapperTechnology_hostRelocator.h"
#include "CjvxGenericWrapperTechnology.h"

CjvxGenericWrapperTechnology_hostRelocator::CjvxGenericWrapperTechnology_hostRelocator()
{
	connectedTech = NULL;
	theHostRef = NULL;
	theReport = NULL;
}
	
CjvxGenericWrapperTechnology_hostRelocator::~CjvxGenericWrapperTechnology_hostRelocator()
{
}

void 
CjvxGenericWrapperTechnology_hostRelocator::init(CjvxGenericWrapperTechnology* ref, IjvxHiddenInterface* hostRef)
{
	connectedTech = ref;
	theHostRef = hostRef;
	theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT,
		reinterpret_cast<jvxHandle**>(&theReport));
}

void
CjvxGenericWrapperTechnology_hostRelocator::terminate()
{
	if (theReport)
	{
		theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT,
			reinterpret_cast<jvxHandle*>(theReport));
	}
	theReport = NULL;
}

// =============================================================================

jvxErrorType 
CjvxGenericWrapperTechnology_hostRelocator::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		res = theReport->report_simple_text_message(txt, prio);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
	jvxPropertyCallPurpose callpurpose )
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		res = theReport->report_internals_have_changed(thisismytype, thisisme,
			cat, propId, onlyContent, offs, num, tpTo,
			callpurpose);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
	jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
	jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, 
	jvxPropertyCallPurpose callpurpose)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		res = theReport->report_take_over_property( thisismytype, thisisme,
			 fld,  numElements,  format,  offsetStart,  onlyContent,
			 cat,  propId,  tpTo,  callpurpose );
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		if (jvx_bitTest(request, JVX_REPORT_REQUEST_REACTIVATE_SHIFT))
		{
			res = theReport->report_command_request(request, &connectedTech->_common_set.theComponentType, sizeof(jvxComponentIdentification));
		}
		else
		{
			res = theReport->report_command_request(request, caseSpecificData, szSpecificData);
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		jvxReportCommandRequest req = request.request();
		switch (req)
		{
			// Messages from the sub system are not forwarded
			// case XYZ:
			// res = theReport->request_command(request);
			// break;
			default:

				// Skip reports from subsystem
				break;
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::report_os_specific(jvxSize commandId, void* context)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theReport)
	{
		res = theReport->report_os_specific(commandId, context);
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::interface_sub_report(IjvxSubReport** subReport)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::request_hidden_interface(jvxInterfaceType tp, jvxHandle** data)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theHostRef)
	{
		switch (tp)
		{
		case JVX_INTERFACE_REPORT:
			if (data)
			{
				*data = static_cast<IjvxReport*>(this);
			}
			res = JVX_NO_ERROR;
			break;
		default:
			res = theHostRef->request_hidden_interface(tp, data);
			break;
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::return_hidden_interface(jvxInterfaceType tp, jvxHandle* data)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theHostRef)
	{
		switch (tp)
		{
		case JVX_INTERFACE_REPORT:
			assert(data == static_cast<IjvxReport*>(this));
			res = JVX_NO_ERROR;
			break;
		default:
			res = theHostRef->return_hidden_interface(tp, data);
			break;
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperTechnology_hostRelocator::object_hidden_interface(IjvxObject** objRef)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	if (theHostRef)
	{
		res = theHostRef->object_hidden_interface(objRef);
	}
	return res;
}

