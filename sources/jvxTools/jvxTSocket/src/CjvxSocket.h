#ifndef __CJVXSOCKET_H__
#define __CJVXSOCKET_H__

#include "jvx.h"
#include "common/CjvxObject.h"

#include "jvx_connect_client_st.h"
#include "jvx_connect_server_st.h"

class CjvxSocket: public IjvxConnection, public CjvxObject
{
private:

	typedef struct
	{
		jvx_connect_client_if* theIf;
		IjvxConnection_report* theReport;
		/*
		struct
		{
			struct
			{
				std::string connectionTarget; 
				int connectionPort;
			} udp;
		} preset;
		*/
	} oneClientIf;

	typedef struct
	{
		jvx_connect_server_if* theIf;
		IjvxConnection_report* theReport;
		/*
		struct
		{
			struct
			{
				std::string connectionTarget;
				int connectionPort;
			} udp;

			struct
			{
				int portId;
			} pcap;
		} derived;
		*/
	} oneServerIf;
	
	jvxSocketAddressType config;
	struct
	{
		struct
		{
			jvx_connect_client_family* fam;
			oneClientIf* ifaces;
			jvxSize numifaces;
			jvx_connect_client_callback_type callbacks;
		} client;

		struct
		{
			jvx_connect_server_family* fam;
			oneServerIf* ifaces;
			jvxSize numifaces;
			jvx_connect_server_callback_type callbacks;
		} server;

		jvx_connect_socket_type mySocketType;
	} runtime;


public:
	CjvxSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxSocket();

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


	// =================================================================
	// Client & Server Interfacing
	// =================================================================
	static jvxErrorType sccallback_report_startup_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType sccallback_report_shutdow_complete(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType sccallback_provide_data_and_length(jvxHandle* privateData, jvxSize channelId, char**, jvxSize*, jvxSize*, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType sccallback_report_data_and_read(jvxHandle* privateData, jvxSize channelId, char*, jvxSize, jvxSize, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	static jvxErrorType sccallback_report_error(jvxHandle* privateData, jvxInt32 errCode, const char* errDescription, jvxSize ifidx);

	jvxErrorType ic_sccallback_report_startup_complete(jvxSize ifidx);
	jvxErrorType ic_sccallback_report_shutdow_complete(jvxSize ifidx);
	jvxErrorType ic_sccallback_provide_data_and_length(jvxSize channelId, char**, jvxSize*, jvxSize*, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType ic_sccallback_report_data_and_read(jvxSize channelId, char*, jvxSize, jvxSize, jvxSize idIf, jvxHandle* additionalInfo, jvxConnectionPrivateTypeEnum whatsthis);
	jvxErrorType ic_sccallback_report_error(jvxInt32 errCode, const char* errDescription, jvxSize ifidx);

	// =================================================================
	// Client Interfacing
	// =================================================================

	static jvxErrorType ccallback_report_unsuccesful_outgoing(jvxHandle* privateData, jvx_connect_fail_reason reason, jvxSize ifidx);
	static jvxErrorType ccallback_report_connect_outgoing(jvxHandle* privateData, jvxSize ifidx);
	static jvxErrorType ccallback_report_disconnect_outgoing(jvxHandle* privateData, jvxSize ifidx);

	jvxErrorType ic_ccallback_report_unsuccesful_outgoing(jvx_connect_fail_reason reason, jvxSize ifidx);
	jvxErrorType ic_ccallback_report_connect_outgoing(jvxSize ifidx);
	jvxErrorType ic_ccallback_report_disconnect_outgoing(jvxSize ifidx);

	// =================================================================
	// Server Interfacing
	// =================================================================
	static jvxErrorType scallback_report_denied_incoming(jvxHandle* privateData, const char* description, jvxSize ifidx);
	static jvxErrorType scallback_report_connect_incoming(jvxHandle* privateData, jvxSize channelId, const char* description, jvxSize ifidx);
	static jvxErrorType scallback_report_disconnect_incoming(jvxHandle* privateData, jvxSize channelId, jvxSize ifidx);

	jvxErrorType ic_scallback_report_denied_incoming(const char* description, jvxSize ifidx);
	jvxErrorType ic_scallback_report_connect_incoming(jvxSize channelId, const char* description, jvxSize ifidx);
	jvxErrorType ic_scallback_report_disconnect_incoming(jvxSize channelId, jvxSize ifidx);
};

#endif
