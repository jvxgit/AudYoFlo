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
	jvxErrorType CjvxInputConnectorCore::_postprocess_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType CjvxInputConnectorCore::_start_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	void CjvxInputConnectorCore::_start_connect_common(jvxSize uId) override;
	jvxErrorType CjvxInputConnectorCore::_stop_connect_forward(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	void CjvxInputConnectorCore::_stop_connect_common(jvxSize* uId)override;
	jvxErrorType CjvxInputConnectorCore::_check_common_icon(IjvxDataConnectionCommon*, IjvxConnectionMaster*)override;
};

#endif
