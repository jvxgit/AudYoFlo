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