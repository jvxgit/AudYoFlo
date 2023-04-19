#include "common/CjvxInputConnectorLink.h"

CjvxInputConnectorLink::CjvxInputConnectorLink()
{

}

jvxErrorType 
CjvxInputConnectorLink::_connect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_prepare_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

void 
CjvxInputConnectorLink::_start_connect_common(jvxSize uId)
{

}

jvxErrorType 
CjvxInputConnectorLink::_stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

void 
CjvxInputConnectorLink::_stop_connect_common(jvxSize* uId)
{

}

jvxErrorType 
CjvxInputConnectorLink::_check_common_icon(IjvxDataConnectionCommon*, IjvxConnectionMaster*)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_test_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_transfer_forward_forward(jvxLinkDataTransferType, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_process_start_forward(void)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_process_stop_forward(jvxSize, jvxBool, jvxSize, callback_process_stop_in_lock, jvxHandle*)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputConnectorLink::_process_buffers_forward(jvxSize, jvxSize)
{
	return JVX_NO_ERROR;
}
