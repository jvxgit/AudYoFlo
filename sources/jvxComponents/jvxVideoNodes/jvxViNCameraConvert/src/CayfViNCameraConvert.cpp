#include "CayfViNCameraConvert.h"

// class CayfViNCameraConvert

CayfViNCameraConvert::CayfViNCameraConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{

}

CayfViNCameraConvert::~CayfViNCameraConvert()
{
}

jvxErrorType 
CayfViNCameraConvert::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));
	return res;
}

jvxErrorType 
CayfViNCameraConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

