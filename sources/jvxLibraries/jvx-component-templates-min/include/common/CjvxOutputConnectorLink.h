#ifndef __CJVXOUTPUTCONNECTORLINK_H__
#define __CJVXOUTPUTCONNECTORLINK_H__

// #define JVX_VERBOSE_CJVXINPUTCONNECTOR_H
#include "common/CjvxOutputConnectorCore.h"
#include "common/CjvxInputOutputConnectorCore.h"

class CjvxOutputConnectorLink :
	public CjvxOutputConnectorCore,
	public CjvxInputOutputConnectorCore
{
public:
	CjvxOutputConnectorLink();

	jvxErrorType _check_common_ocon(IjvxDataConnectionCommon*, IjvxConnectionMaster*)override;
	jvxErrorType _transfer_backward_backward(jvxLinkDataTransferType, jvxHandle*, IjvxCallProt*)override;
};

#endif
