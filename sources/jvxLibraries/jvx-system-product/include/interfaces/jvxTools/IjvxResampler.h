#ifndef __IJVXRESAMPLER_H__
#define __IJVXRESAMPLER_H__

JVX_INTERFACE IjvxResampler: public IjvxObject
{
public:

	virtual JVX_CALLINGCONVENTION ~IjvxResampler(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION  initialize(IjvxHiddenInterface* hostRef) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxSize sizeInput, jvxSize sizeOutput, jvxData inFrequency, jvxData outFrequency, jvxSize filterLength, 
		jvxBool adaptOutputRateIfNec, jvxDataFormat format, jvxData stopBandAtten = 60.0, jvxData socket = 0.0 ) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxData freqMin, jvxSize size_input, jvxSize size_output, jvxData in_freq, jvxData out_freq, jvxSize filter_length, 
	//	jvxData stopBandAtten, jvxData socket ) = 0;
		
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION process(jvxHandle* input, jvxHandle* output) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION process(jvxHandle* inputPart1, jvxSize input_szPart1, jvxHandle* inputPart2, jvxHandle* outputPart1, jvxSize output_szPart1, jvxHandle* outputPart2) = 0;

	//virtual jvxErrorType JVX_CALLINGCONVENTION properties(jvxData* ratein, jvxData* rateout, jvxSize* bsize_in, jvxSize* bsize_out, jvxSize* filter_length,jvxSize* filter_length_orig,jvxData* nominator, jvxData* denom, jvxData* freqMin, jvxInt32* oversample, jvxData* delay_used) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION getSignalDelay_precompute(jvxData* ret, jvxSize sizeInput, jvxSize sizeOutput, jvxSize filter_length) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() = 0;

};

#endif
