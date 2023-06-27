#include "jvxGenericSocketTechnologies/CjvxGenericSocketTechnology.h"

CjvxGenericSocketTechnology::CjvxGenericSocketTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxGenericConnectionTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL) 
{
	connectionIdenitificationToken = "jvxTNewSocket";
}

CjvxGenericSocketTechnology::~CjvxGenericSocketTechnology()
{
}

jvxErrorType 
CjvxGenericSocketTechnology::activate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::activate();
	if (res == JVX_NO_ERROR)
	{

		CjvxGenericSocketTechnology_pcg::init_all();
		CjvxGenericSocketTechnology_pcg::allocate_all();
		CjvxGenericSocketTechnology_pcg::register_all(this);
	}
	return res;
}

jvxErrorType 
CjvxGenericSocketTechnology::deactivate()
{
	jvxErrorType res = CjvxGenericConnectionTechnology::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxGenericSocketTechnology_pcg::unregister_all(this);
		CjvxGenericSocketTechnology_pcg::deallocate_all();		
		CjvxGenericConnectionTechnology::deactivate();
	}
	return res;
}

jvxErrorType
CjvxGenericSocketTechnology::initializeConnectionCoreModule(IjvxConnection* conn)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = conn->initialize(_common_set_min.theHostRef,
		(jvxHandle*)txtConfigure.c_str(),
		JVX_CONNECT_PRIVATE_ARG_TYPE_INPUT_STRING);	
	return res;
}
