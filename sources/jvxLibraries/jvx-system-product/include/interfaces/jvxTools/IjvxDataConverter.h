#ifndef __IJVXDATACONVERTER_H__
#define __IJVXDATACONVERTER_H__

JVX_INTERFACE IjvxDataConverter: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxDataConverter(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxDataFormat inputFormat, jvxDataFormat outputFormat) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION process(const jvxHandle* fldIn, jvxHandle* fldOut, jvxSize numSamples) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif
