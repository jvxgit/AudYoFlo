#include "CjvxAppFactoryHostCon.h"

CjvxAppFactoryHostCon::CjvxAppFactoryHostCon(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) : 
	CjvxAppFactoryHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

CjvxAppFactoryHostCon::~CjvxAppFactoryHostCon()
{
}

jvxErrorType
CjvxAppFactoryHostCon::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
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
	default:
		res = CjvxAppFactoryHost::request_hidden_interface(tp, hdl);
	}
	
	return(res);
}

jvxErrorType
CjvxAppFactoryHostCon::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_NO_ERROR;
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


	default:
		res = CjvxAppFactoryHost::return_hidden_interface(tp, hdl);
	}

	return res;
}
