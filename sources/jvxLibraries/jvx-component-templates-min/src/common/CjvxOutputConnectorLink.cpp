#include "common/CjvxOutputConnectorLink.h"

CjvxOutputConnectorLink::CjvxOutputConnectorLink()
{

}

jvxErrorType 
CjvxOutputConnectorLink::_check_common_ocon(IjvxDataConnectionCommon*, IjvxConnectionMaster*)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxOutputConnectorLink::_transfer_backward_backward(jvxLinkDataTransferType, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return JVX_NO_ERROR;
}

