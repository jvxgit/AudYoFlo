#ifndef __IJVXREMOTECALL_H__
#define __IJVXREMOTECALL_H__

JVX_INTERFACE IjvxRemoteCall_callback
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxRemoteCall_callback() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION reportPendingError(const char* errorDescription, jvxErrorType tp) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reportConnectionEstablished(const char* conn_descriptor) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reportConnectionClosed() = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reportPendingAction(jvxRCOneParameter* input_params, jvxSize numInParams, jvxRCOneParameter* output_params, jvxSize numOutParams,
		const char* nmAction, jvxSize actionId, jvxCBool* immediateResponse) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reportActionRequest(const char* description, jvxSize numinparams, jvxSize numoutparams, jvxSize processId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reportLocalError(const char* description, jvxSize localErrorId) = 0;
	
};

JVX_INTERFACE IjvxRemoteCall: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxRemoteCall(){};

	//=============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION supportsOperationMode(jvxRCOperationMode mode, jvxBool* doesSupport) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, IjvxConnection* theConnection, jvxRCOperationMode mode, jvxHandle* configStruct, IjvxRemoteCall_callback* backRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION start(jvxHandle* connectionPrivate, jvxConnectionPrivateTypeEnum whatsthis, jvxSize idPort, jvxSize preAllocateBytes = 0) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION connection_state(jvxRCConnectionState* st) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION allocateParameterList(jvxRCOneParameter** lstParams, jvxSize numberParams) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION deallocateParameterList(jvxRCOneParameter* lstParams) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION remoteRequest(jvxRCOneParameter* inputParameters, jvxSize numInputParameters,
		jvxRCOneParameter* outputParameters, jvxSize numOutputParameters,
		const char* actionDescriptor = "", jvxSize actionId = JVX_SIZE_UNSELECTED, jvxSize timeout_ms = 10000,
		jvxErrorType* errCode_otherside = NULL) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
};

JVX_INTERFACE IjvxRemoteCall_async
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxRemoteCall_async(){};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingError(char* buf, jvxSize lField, jvxErrorType* tp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingEvent(jvxRCEventType* tp) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION lockPendingAction() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION isLockedPendingAction(jvxBool* yes_itslocked) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION requestPendingAction(jvxBool* yes_thereis) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION parameterDescriptorsPendingRequest(jvxSize* numInputParams, jvxSize* numOutputParams, char* fldActionDescription, jvxSize fldLength, jvxSize* processId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION parametersPendingRequest(jvxRCOneParameter* inParams,jvxSize numInConts, jvxRCOneParameter* outParams, jvxSize numOutConts = 0) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION reportCompletePendingAction(jvxErrorType success) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unlockPendingAction() = 0;
};

#endif
