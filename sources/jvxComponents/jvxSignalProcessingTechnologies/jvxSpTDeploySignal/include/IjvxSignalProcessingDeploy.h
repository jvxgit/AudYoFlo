#ifndef _IJVXSIGNALPROCESSINGDEPLOY_H__
#define _IJVXSIGNALPROCESSINGDEPLOY_H__

#include "jvx.h"

class IjvxSignalProcessingDeploy_data
{
public:
	jvxByte** bufs_received_fld;
	jvxSize bufs_received_num;
	jvxByte** bufs_to_emit_fld;
	jvxSize bufs_to_emit_num;
	jvxSize bsize;
	jvxSize srate;
	jvxDataFormat form;
	jvxDataFormatGroup format_group;
	IjvxSignalProcessingDeploy_data()
	{
		bufs_received_fld = NULL;
		bufs_received_num = 0;
		bufs_to_emit_fld = NULL;
		bufs_to_emit_num = 0;
		bsize = 0;
		srate = 0;
		form = JVX_DATAFORMAT_NONE;
		format_group = JVX_DATAFORMAT_GROUP_NONE;
	};
};

JVX_INTERFACE IjvxSignalProcessingDeploy_interact
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSignalProcessingDeploy_interact() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION process_request_data_start(jvxLinkDataDescriptor* dataOut) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_request_data_stop(jvxLinkDataDescriptor* dataIn) = 0;
};

JVX_INTERFACE IjvxSignalProcessingDeploy
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSignalProcessingDeploy() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION setDataInteractReference(IjvxSignalProcessingDeploy_interact* interact) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION setOutputParameters(jvxLinkDataDescriptor_con_params* params) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION setInputParameters(jvxLinkDataDescriptor_con_params* params) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerTest() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION exchangeData(IjvxSignalProcessingDeploy_data* data, jvxLinkDataAttached* ptr = NULL) = 0;
	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerPrepare() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerStart() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerStop() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerPostprocess() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerProcess() = 0;
	*/
};

#endif