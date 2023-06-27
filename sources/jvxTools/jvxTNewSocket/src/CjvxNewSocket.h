#ifndef __CJVXNEWSOCKET_H__
#define __CJVXNEWSOCKET_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "CjvxSockets.h"

class CjvxNewSocket: public IjvxSocketsClient_report, public IjvxSocketsConnection_report, 
	public IjvxConnection, public CjvxObject
{
private:
	CjvxSocketsClientFactory sFac;

	CjvxSocketsClientInterface* theIfaceStart = nullptr;
	CjvxSocketsClientInterfaceTcpUdp* theIfaceStartUdp = nullptr;

	CjvxSocketsConnection_transfer sendOne;

	jvxByte* fldRead = nullptr;
	jvxSize fldReadSz = 0;
	
	jvxSize myId = JVX_SIZE_UNSELECTED;
	IjvxConnection_report* theReport = nullptr;

	IjvxSocketsConnection* theConnectionHandle = nullptr;
	JVX_MUTEX_HANDLE safeAccessHandles;

public:
	CjvxNewSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxNewSocket();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv,
							      jvxConnectionPrivateTypeEnum whatsthis) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION availablePorts(jvxApiStringList* allPorts)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION threading_model(jvxConnectionThreadingModel* thread_model)override
	{
		if (thread_model)
			*thread_model = JVX_CONNECT_THREAD_MULTI_THREAD;
		return JVX_NO_ERROR;
	};

	// =============================================================
	// Port specific commands
	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION state_port(jvxSize idPort, jvxState* theState)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_port(jvxSize idPort, jvxHandle* cfg,
							      jvxConnectionPrivateTypeEnum whatsthis,
							      IjvxConnection_report* theReport)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend,
									  jvxSize* fldMinSize, jvxSize* fldMaxSize)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld,
								    jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION readMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld,
								    jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION scanMessage_port(jvxSize idPort, jvxSize* requiredSize) override
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_port(jvxSize idPort)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION control_port(jvxSize idPort, jvxSize operation_id,
								jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis)override;

	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

	// ============================================================================================
	// Interface IjvxSocketsClient_report
	jvxErrorType report_client_connect(IjvxSocketsConnection* connection) override;
	jvxErrorType report_client_disconnect(IjvxSocketsConnection* connection) override;
	jvxErrorType report_client_error(jvxSocketsErrorType err, const char* description) override;

	// ============================================================================================
	// Interface IjvxSocketsConection_report
	jvxErrorType provide_data_and_length(char** fld_use, 
		jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo) override;
	jvxErrorType report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset,
		IjvxSocketsAdditionalInfoPacket* additionalInfo) override;
	jvxErrorType report_connection_error(jvxSocketsErrorType errCode, const char* errDescription) override;

	// ============================================================================================
	void deallocate_fld();
	void allocate_fld(jvxSize fldSz);
};

#endif
