#ifndef __CJVXINPUTCONNECTORLINK_H__
#define __CJVXINPUTCONNECTORLINK_H__

// #define JVX_VERBOSE_CJVXINPUTCONNECTOR_H
#include "common/CjvxInputConnectorCore.h"
#include "common/CjvxInputOutputConnectorCore.h"

class CjvxInputConnectorLink :
	public CjvxInputConnectorCore,
	public CjvxInputOutputConnectorCore
{
public:
	CjvxInputConnectorLink();

	jvxErrorType _connect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType _disconnect_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType _prepare_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType _postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType _start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	void _start_connect_common(jvxSize uId) override;
	jvxErrorType _stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	void _stop_connect_common(jvxSize* uId)override;
	jvxErrorType _check_common_icon(IjvxDataConnectionCommon*, IjvxConnectionMaster*)override;
	jvxErrorType _test_connect_forward(IjvxCallProt*)override;
	jvxErrorType _transfer_forward_forward(jvxLinkDataTransferType, jvxHandle*, IjvxCallProt*)override;
	jvxErrorType _process_start_forward(void) override;
	jvxErrorType _process_stop_forward(jvxSize, jvxBool, jvxSize, callback_process_stop_in_lock, jvxHandle*)override;
	jvxErrorType _process_buffers_forward(jvxSize, jvxSize)override;
};

#endif
