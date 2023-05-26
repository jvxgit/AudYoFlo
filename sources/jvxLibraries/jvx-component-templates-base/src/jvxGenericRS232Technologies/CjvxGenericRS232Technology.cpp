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
