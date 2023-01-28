#include "jvx.h"
#include "common/CjvxObject.h"

#ifndef _CJVXRESAMPLER_H__
#define _CJVXRESAMPLER_H__

#define JVX_RESAMPLER_DSP_LIB

#include "jvx_resampler/jvx_fixed_resampler.h"

/**
 * Class to implement a rateconversion filter. The underlying
 * algorithm is described in "Crochiere and Rabiner - Multirate
 * Signal Processing", chapter 2.3.4 for example.
 * For the prototype lowpass, a kaiser window with an si lowpass
 * is used and determined depending on the conversion parameters.
 * The module automatically determines all relevant parameters,
 * thus the result may be long filters depending on the setup used.
 * There are numerous functions to determine the parameters for a setup,
 * that is M, L as up/downsampling parameters...
 * The complexity for a filtering is roughly as high as
 * <sizeOutputFrame> * <sizeImpulseLowpass> (MAC).
 * If you are using filters with only few tabs, do not use
 * stopband attenuation of 60 dB but less!
 *///==================================================
class CjvxResampler: public IjvxResampler, public CjvxObject
{
private:

public:

	//! Constructor to set the values to initial state (lengthes = 0 etc)
	CjvxResampler(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor, deallocate buffers if necessary
	~CjvxResampler();

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxSize sizeInput, jvxSize sizeOutput, jvxData inFrequency, jvxData outFrequency, jvxSize filterLength,
							   jvxBool adaptOutputRateIfNec, jvxDataFormat format, jvxData stopBandAtten = 60.0, jvxData socket = 0.0 )override;

	//virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxData freqMin, jvxSize size_input, jvxSize size_output, jvxData in_freq, jvxData out_freq, jvxSize filter_length,
	//						   jvxData stopBandAtten, jvxData socket );

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override ;

	virtual jvxErrorType JVX_CALLINGCONVENTION process(jvxHandle* input, jvxHandle* output) override;

	//virtual jvxErrorType JVX_CALLINGCONVENTION process(jvxHandle* inputPart1, jvxSize input_szPart1, jvxHandle* inputPart2, jvxHandle* outputPart1, jvxSize output_szPart1, jvxHandle* outputPart2);

	//virtual jvxErrorType JVX_CALLINGCONVENTION properties(jvxData* ratein, jvxData* rateout, jvxSize* bsize_in, jvxSize* bsize_out, jvxSize* filter_length,jvxSize* filter_length_orig,jvxData* nominator, jvxData* denom, jvxData* freqMin, jvxInt32* oversample, jvxData* delay_used) ;

	virtual jvxErrorType JVX_CALLINGCONVENTION getSignalDelay_precompute(jvxData* ret, jvxSize sizeInput, jvxSize sizeOutput, jvxSize filter_length) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

	//! Initialize the filter: Pass input and output properties, create buffers etc.. Return false if fails (reason: call initialize twice wo. terminate)
	bool initializeFreqMin(jvxData freqMin, int sizeInput, int sizeOutput, int inFrequency, int outFrequency,int filterLength, jvxData stopBandAtten = 60.0, jvxData socket = 0.0 );


private:

	//! Size for vector, input
	jvxSize vector_size_input;

	//! Size for vector, output
	jvxSize vector_size_output;

	jvxDataFormat formatData;

	jvxSize szElement;

	//! Value for the stopband attenuation, by default 60
	jvxData stopband_attenuation;

	//! Value for the socket for the kaiser window (as in "Hess - Digitale Filter")
	jvxData socketKaiser;

	//! Input samplingrate
	jvxData inSamplerate;

	//! Output samplingrate
	jvxData outSamplerate;

	//! Length of the impulse response, original samplerate
	jvxSize filterLengthOriginal;

	//! Length of the impulse response, upsampled samplerate
	jvxSize filterLengthUpsampled;

	//! Size for internal states, input vector plus filter length
	jvxSize state_size;

	//! Filter coefficients
	void* filter;

	//! Filter buffer (states)
	void* statesBuffer;

	//! Further variables, z: (F2-F1)/F1, n: oversampling, hsize: amount of filter coefficients to determine
	long z, n, hsize;

	jvxData shift;

	//! Counter: mod_N(Z*k)
	long in_N_Counter;

	//! Counter: int(Z*k/N)
	long N_Counter;

	//! Passband frequency
	jvxData fminLowpass;

	//! State variable for instance
	bool initialized;

#ifdef JVX_RESAMPLER_DSP_LIB
	jvx_fixed_resampler resLib;
#endif

};

#endif
