#ifndef __CJVXREMOTECALL_H__
#define __CJVXREMOTECALL_H__

#include "jvx.h"
#include <list>
#include "common/CjvxObject.h"
extern "C"
{
#include "jvx_remote_call.h"
}

#define JVX_INTEGRATE_CRC_REMOTE_CALL

class CjvxRemoteCall: public IjvxRemoteCall, public IjvxConnection_report, public CjvxObject
{
	typedef enum
	{
		JVX_INPUT_STATE_HEADER = 0,
		JVX_INPUT_STATE_FIELD = 1
	} jvxPacketInputStateMachine;

	typedef enum
	{
		JVX_REMOTE_CALL_ADDRESS_UNKNOWN = 0,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_TCP = 1,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_TCP = 2,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_UDP = 3,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_UDP = 4,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_SERVER_PCAP = 5,
		JVX_REMOTE_CALL_ADDRESS_SOCKET_CLIENT_PCAP = 6,
		JVX_REMOTE_CALL_ADDRESS_RS232 = 7
	} jvxRemoteCallAddress;

	typedef struct
	{
		jvxSize bytesPrepend;
		jvxSize fldMinSize;
		jvxSize fldMaxSize;
	} jvxFrameConstraintType;

	typedef enum
	{
		JVX_REMOTE_CALL_CHANNEL_STATUS_AVAILABLE = 0,
		JVX_REMOTE_CALL_CHANNEL_STATUS_ACCEPT_INCOMING = 1,
		JVX_REMOTE_CALL_CHANNEL_STATUS_WAIT_OUTGOING = 2
	} jvxRcChannelStatus;
private:

	IjvxConnection* myConnection;
	jvxSize portId;
	IjvxRemoteCall_callback* reportManager;
	jvxSize globalCount;
	jvxRCOperationMode myMode;
	jvx_remote_call hdlRemoteCall;
	jvxFrameConstraintType myFrameConstraint;
	std::string myConnectionDescriptor;
	jvxRemoteCallAddress whatToAddress;
	jvxConnectionThreadingModel myThreadingModel;
	JVX_MUTEX_HANDLE safeAccess;
	jvxRcChannelStatus myChannelStatus;

	struct
	{
		struct
		{
			jvxSize idChannelIn;
		} tcp;
	} derived;

	struct
	{
		jvxRCOneParameter* inputParameters;
		jvxSize numInParams;
		jvxRCOneParameter* outputParameters;
		jvxSize numOutParams;
		std::string nameAction;
		jvxSize processingId;
		int counterParamIn;	
		int counterParamOut;
		JVX_MUTEX_HANDLE safeAccess_incoming;
		jvxSize processCounter;
		jvxBool isLocked;
	} incomingRequest;

	struct
	{
		JVX_NOTIFY_HANDLE hdlOperationComplete;
		jvxErrorType fCall_result;
		jvxSize processId;

	} outgoingRequest;

	struct
	{
		JVX_MUTEX_HANDLE safeAccessError;
		jvxBool isErrorPending;
		jvxErrorType errCode;
		std::string errDescription;
	} pendingError;

	struct
	{
		JVX_MUTEX_HANDLE safeAccessEvents;
		std::list<jvxRCEventType> events;
	} pendingEvents;

	struct
	{
		jvxByte* fld;
		jvxSize sz;
		jvxSize readOffset;
		jvxSize evaluateOffset;
	} ram;

	struct
	{
		jvxPacketInputStateMachine theState;
		jvxSize packetSize;
		jvxBool sc_startScanned;
	} stateMachine;

	struct
	{
		jvxByte startChar;
		jvxByte stopChar;
		jvxBool sc_active;
	} rs232Specific;

	struct
	{
		jvxBool completed;
	} singleThread;

	the_jvx_printf locptf;
public:

	CjvxRemoteCall(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxRemoteCall();

	virtual jvxErrorType JVX_CALLINGCONVENTION supportsOperationMode(jvxRCOperationMode mode, jvxBool* doesSupport)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, IjvxConnection* theConnection,
							      jvxRCOperationMode mode, jvxHandle* configStruct, IjvxRemoteCall_callback* backRef)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxHandle* connectionPrivate, jvxConnectionPrivateTypeEnum whatsthis, jvxSize idPort,
							 jvxSize preAllocateBytes)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connection_state(jvxRCConnectionState* st) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION allocateParameterList(jvxRCOneParameter** lstParams, jvxSize numberParams)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION deallocateParameterList(jvxRCOneParameter* lstParams)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION remoteRequest(jvxRCOneParameter* inputParameters, jvxSize numInputParameters,
		jvxRCOneParameter* outputParameters, jvxSize numOutputParameters,
		const char* actionDescriptor = "", jvxSize actionId = JVX_SIZE_UNSELECTED, jvxSize timeout_ms = JVX_INFINITE_MS,
		jvxErrorType* errCode_otherside = NULL)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;	




	
	// =============================================================================
	// Async interface
	// =============================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingError(char* buf, jvxSize lField, jvxErrorType* tp);

	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingEvent(jvxRCEventType* tp);

	virtual jvxErrorType JVX_CALLINGCONVENTION lockPendingAction();

	virtual jvxErrorType JVX_CALLINGCONVENTION isLockedPendingAction(jvxBool* yes_itslocked);

	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingAction(jvxBool* yes_thereis);

	virtual jvxErrorType JVX_CALLINGCONVENTION parameterDescriptorsPendingRequest(jvxSize* numInputParams,
										      jvxSize* numOutputParams, char* fldActionDescription,
										      jvxSize fldLength, jvxSize* processId);

	virtual jvxErrorType JVX_CALLINGCONVENTION parametersPendingRequest(jvxRCOneParameter* inParams,jvxSize numInConts,
									    jvxRCOneParameter* outParams, jvxSize numOutConts = 0);

	virtual jvxErrorType JVX_CALLINGCONVENTION reportCompletePendingAction(jvxErrorType success);

	virtual jvxErrorType JVX_CALLINGCONVENTION unlockPendingAction();


#include "codeFragments/simplify/jvxObjects_simplify.h"


	// ==================================================================
	// C style function callbacks for state machine
	// ==================================================================
	static jvxDspBaseErrorType jvx_rc_accept_init_request(jvxHandle* hdl, const char* description, jvxSize numInParams,
							      jvxSize numOutParams, jvxSize processingId);
	static jvxDspBaseErrorType jvx_rc_unlock_receive_buffer(jvxHandle* hdl, jvxSize unique_id);
	static jvxDspBaseErrorType jvx_rc_allocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose, jvxHandle** ptrReturn, 
		jvxSize* offset, jvxSize numElements, jvxSize* szBytesAllocated, jvxDataFormat form);
	static jvxDspBaseErrorType jvx_rc_deallocate_memory(jvxHandle* hdl, jvx_rc_allocation_purpose, jvxHandle* ptrReturn, jvxSize szFldAllocated);
	static jvxDspBaseErrorType jvx_rc_send_buffer(jvxHandle* hdl, jvxByte* resp, jvxSize numBytes);
	static jvxDspBaseErrorType jvx_rc_process_request(jvxHandle* hdl, const char* description, jvxRCOneParameter* paramsIn, 
		jvxSize numParamsIn, jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateResponse);
	static jvxDspBaseErrorType jvx_rc_lock(jvxHandle* hdl);
	static jvxDspBaseErrorType jvx_rc_unlock(jvxHandle* hdl);
	static jvxDspBaseErrorType jvx_rc_report_local_error(jvxHandle* hdl, const char* text, jvxDspBaseErrorType errCode, jvxSize errId);
	static jvxDspBaseErrorType jvx_rc_report_transfer_terminated(jvxHandle* hdl, jvxSize processing_id,
								     jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description);

	jvxDspBaseErrorType ic_rc_accept_init_request(const char* description, jvxSize numInParams, jvxSize numOutParams, jvxSize processingId);
	jvxDspBaseErrorType ic_rc_unlock_receive_buffer( jvxSize unique_id);
	jvxDspBaseErrorType ic_rc_allocate_memory(jvx_rc_allocation_purpose, jvxHandle** ptrReturn, jvxSize* offset, jvxSize numElements, 
		jvxSize* szBytesAllocated, jvxDataFormat form);
	jvxDspBaseErrorType ic_rc_deallocate_memory(jvx_rc_allocation_purpose, jvxHandle* ptrReturn, jvxSize szFldAllocated);
	jvxDspBaseErrorType ic_rc_send_buffer(jvxByte* resp, jvxSize numBytes);
	jvxDspBaseErrorType ic_rc_process_request(const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, jvxRCOneParameter* paramsOut, 
		jvxSize numParamsOut, jvxSize processingId, jvxCBool* immediateresponse);
	jvxDspBaseErrorType ic_rc_lock();
	jvxDspBaseErrorType ic_rc_unlock();
	jvxDspBaseErrorType ic_rc_report_local_error(const char* text, jvxDspBaseErrorType errCode, jvxSize errId);
	jvxDspBaseErrorType ic_rc_report_transfer_terminated( jvxSize processing_id, jvxSize processing_counter, jvxDspBaseErrorType resulTransfer, const char* err_description);

	// ==================================================================
	// C++ callbacks for IjvxConnection callbacks
	// ==================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(jvxByte**, jvxSize*, jvxSize* offset, jvxSize id_port,
									   jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(jvxByte*, jvxSize, jvxSize offset, jvxSize id_port, jvxHandle* addInfo,
									jvxConnectionPrivateTypeEnum whatsthis)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo,
								jvxConnectionPrivateTypeEnum whatsthis)override;
};

#endif

/* End of File */
