#ifndef __IJVXBINARYFASTDATACONTROL_H__
#define __IJVXBINARYFASTDATACONTROL_H__

JVX_INTERFACE IjvxBinaryFastDataControl_report
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxBinaryFastDataControl_report(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION request_release_input(jvxSize idx, jvxHandle* context) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_release_output(jvxSize idx, jvxHandle* context) = 0;
};

JVX_INTERFACE IjvxBinaryFastDataControl
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxBinaryFastDataControl(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_report(IjvxBinaryFastDataControl_report* rep, jvxHandle* context) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION number_inputs(jvxSize* num) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION description_input(jvxSize idx, jvxApiString* str) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_access_input(jvxSize idx, jvxBinaryData_header** input) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION release_access_input(jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_use_input(jvxSize idx, jvxBitField* whatChanged = NULL, jvxSize numBitFields = 0) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION number_outputs(jvxSize* num) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION description_output(jvxSize idx, jvxApiString* str) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_access_output(jvxSize idx, jvxBinaryData_header** output) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION release_access_output(jvxSize idx) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_prepare_output(jvxSize idx, jvxBitField* whatChanged = NULL, jvxSize numBitFields = 0) = 0;


	virtual jvxErrorType JVX_CALLINGCONVENTION lock_access() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION unlock_access() = 0;

};

#endif
