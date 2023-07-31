#include "jvx.h"

#include "jvxHosts/CjvxConnectionMinHost.h"

// ====================================================================
// Member functions: Allocate/deallocate object
// ====================================================================

/**
 * Constructor: Only those commands required on this class derivation level.
 */
CjvxConnectionMinHost::CjvxConnectionMinHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxDefaultInterfaceFactory<IjvxMinHost>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.tp = JVX_COMPONENT_MIN_HOST;
	_common_set.theComponentType.slotid = 0;
	_common_set.theComponentType.slotsubid = 0;

	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxMinHost*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	this->_set_unique_descriptor("min host");
}

/**
 * Destructor: Only terminate call
 */
CjvxConnectionMinHost::~CjvxConnectionMinHost()
{
	this->terminate();
}

// ====================================================================
// Member functions: State transitions
// ====================================================================

/*
 * Initialize the current component
 */
jvxErrorType
CjvxConnectionMinHost::initialize(IjvxHiddenInterface* theOtherhost)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theOtherhost == NULL)
	{
		theOtherhost = static_cast<IjvxHiddenInterface*>(this);
	}
	else
	{
		// Does this happen?? A host in a host would not make sense..
		assert(0);
	}

	res = _initialize(theOtherhost);
	return(res);
}

jvxErrorType
CjvxConnectionMinHost::select(IjvxObject* theOwner)
{
	jvxErrorType res = _select(theOwner);
	return(res);
}


/*
 * Activate host component
 */
jvxErrorType
CjvxConnectionMinHost::activate()
{
	jvxErrorType res = JVX_NO_ERROR;

	_set_system_refs(_common_set_min.theHostRef, nullptr);

	res = _activate_no_text_log(); // We need to wait with text log start until components have been loaded
	return(res);
}


/*
 * Deactivate host component
 */
jvxErrorType
CjvxConnectionMinHost::deactivate()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		res = _deactivate_no_text_log();
		assert(res == JVX_NO_ERROR);

		_set_system_refs(nullptr, nullptr);
	}

	return(res);
}

jvxErrorType
CjvxConnectionMinHost::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_SELECTED)
	{
		res = _unselect();
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/*
 * Terminate host component
 */
jvxErrorType
CjvxConnectionMinHost::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _terminate();
	return(res);
}

jvxErrorType 
CjvxConnectionMinHost::owner(IjvxObject** dependsOn)
{
	return _owner(dependsOn);
}

// ====================================================================

jvxErrorType
CjvxConnectionMinHost::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxConnectionMinHost::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_INTERFACE_DATA_CONNECTIONS:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this));
		}
		break;

	case JVX_INTERFACE_UNIQUE_ID:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this));
		}
		break;

	case JVX_INTERFACE_REPORT_SYSTEM:
		if (hdl)
		{
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxReportSystem*>(this));
		}
		break;
	default:

		res = _request_hidden_interface(tp, hdl);
	}
	return(res);
}

jvxErrorType
CjvxConnectionMinHost::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_INTERFACE_DATA_CONNECTIONS:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_UNIQUE_ID:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxUniqueId*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_REPORT_SYSTEM:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxReportSystem*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = _return_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType
CjvxConnectionMinHost::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;

	// We support only the request to report a test request if we are in a test run
	if (request.request() == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN)
	{
		const CjvxReportCommandRequest_uid* req_spec = castCommandRequest<CjvxReportCommandRequest_uid>(request);
		if (req_spec)
		{
			jvxSize num;
			jvxSize uId = JVX_SIZE_UNSELECTED;
			req_spec->uid(&uId);
			res = this->add_process_test(uId, &num, request.immediate());
			if (res == JVX_ERROR_POSTPONE)
			{
				// What to do here? how can be trigger back the request??
			}
		}
	}
	return res;
}
