#include "jvxGenericRs232Technologies/CjvxGenericRs232Technology.h"

CjvxGenericRs232Technology::CjvxGenericRs232Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericConnectionTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL) 
{
	connectionIdenitificationToken = "jvxTRs232*";
}

CjvxGenericRs232Technology::~CjvxGenericRs232Technology()
{
}

jvxErrorType 
CjvxGenericRs232Technology::activate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericRs232Technology_pcg::init_all();
		CjvxGenericRs232Technology_pcg::allocate_all();
		CjvxGenericRs232Technology_pcg::register_all(this);
	}
	return res;
}

jvxErrorType 
CjvxGenericRs232Technology::deactivate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{

		CjvxGenericRs232Technology_pcg::unregister_all(this);
		CjvxGenericRs232Technology_pcg::deallocate_all();
		CjvxGenericConnectionTechnology::deactivate();
	}
	return res;
}

jvxErrorType
CjvxGenericRs232Technology::initializeConnectionCoreModule(IjvxConnection* conn)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (JVX_CHECK_SIZE_SELECTED(numPortsPolled))
	{
		res = conn->initialize(_common_set_min.theHostRef, &numPortsPolled, JVX_CONNECT_PRIVATE_ARG_TYPE_CONNECTION_NUM_PORT);
	}
	else
	{
		res = conn->initialize(_common_set_min.theHostRef, NULL, JVX_CONNECT_PRIVATE_ARG_TYPE_NONE);
	}
	return res;
}
