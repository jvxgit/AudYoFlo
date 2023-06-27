#ifndef __IJVXCONNECTION_H__
#define __IJVXCONNECTION_H__

JVX_INTERFACE IjvxConnection_report
{
public:
	virtual ~IjvxConnection_report(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(jvxByte**ptr, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, 
		jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, 
		jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, 
		jvxConnectionPrivateTypeEnum whatsthis) = 0;

};

JVX_INTERFACE IjvxConnection: public IjvxObject
{
public:
	virtual ~IjvxConnection(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef, jvxHandle* priv = NULL, jvxConnectionPrivateTypeEnum whatsthis = JVX_CONNECT_PRIVATE_ARG_TYPE_NONE) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION threading_model(jvxConnectionThreadingModel* thread_model) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION availablePorts(jvxApiStringList* allPorts) = 0;

	// =============================================================
	// Port specific commands
	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION state_port(jvxSize idPort, jvxState* theState) = 0;
 
	virtual jvxErrorType JVX_CALLINGCONVENTION start_port(jvxSize idPort, jvxHandle* cfgSpecific, jvxConnectionPrivateTypeEnum whatsthis, IjvxConnection_report* theReport) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_constraints_buffer(jvxSize idPort, jvxSize* bytesPrepend, jvxSize* fldMinSize, jvxSize* fldMaxSize) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION sendMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION scanMessage_port(jvxSize idPort, jvxSize* requiredSize) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION readMessage_port(jvxSize idPort, jvxByte* fld, jvxSize* szFld, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_port(jvxSize idPort) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION control_port(jvxSize idPort, jvxSize operation_id, jvxHandle* priv, jvxConnectionPrivateTypeEnum whatsthis) = 0;
	
	// =============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;
};

#endif