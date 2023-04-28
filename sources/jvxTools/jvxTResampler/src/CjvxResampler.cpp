#include "CjvxResampler.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstring>

#include "jvx_math/jvx_math_extensions.h"
#include "jvx_windows/jvx_windows.h"

#define QFORMAT_32BIT 26


/**
* Konstruktor: Initialize most common values
*///==================================================
CjvxResampler::CjvxResampler(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_RESAMPLER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxResampler*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	filterLengthUpsampled = 0;
	vector_size_input = 0;
	vector_size_output = 0;
	filter = NULL;
	statesBuffer = NULL;
	initialized = false;
	shift = -1;

	jvx_fixed_resampler_initConfig(&resLib);
}

/**
* Destructor: Deallocate memory if necessary
*///==================================================
CjvxResampler::~CjvxResampler()
{
	if(initialized == true)
	{
		free(filter);
		filter = NULL;
		free(statesBuffer);
		statesBuffer = NULL;
		initialized = false;
	}
}

jvxErrorType
CjvxResampler::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = CjvxObject::_initialize(hostRef);
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_select();
	}
	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_activate();
	}

	return res;
}

/**
 * Initialize function: Design filter and prepare all buffers!
 * The last two parameters are set by default:
 * stopBandAtten = 60, socket = 0.0.
 * Suggestion is: If using filters with low number of taps: Use less stopBandAtten.
 * Socket is in accordance to Hess - Digitale Filter.
 *///==================================================
jvxErrorType
CjvxResampler::prepare(jvxSize size_input,
	jvxSize size_output,
	jvxData in_freq,
	jvxData out_freq,
	jvxSize filter_length,
	jvxBool adaptOutputFreqIfNecessary,
	jvxDataFormat format,
	jvxData stopBandAtten,
	jvxData socket)
{
	jvxErrorType res = CjvxObject::_prepare();

#ifdef JVX_RESAMPLER_DSP_LIB
	jvxDspBaseErrorType resD = JVX_DSP_NO_ERROR;
	jvxData delayRes = 0;
#endif

	if (res == JVX_NO_ERROR)
	{
		res = CjvxObject::_start();
	}

	if (res == JVX_NO_ERROR)
	{

#ifdef JVX_RESAMPLER_DSP_LIB
		resLib.prmInit.lFilter = filter_length;
		resLib.prmInit.buffersizeIn = size_input;
		resLib.prmInit.buffersizeOut = size_output;
		resLib.prmInit.format = format;
		resLib.prmInit.stopbandAtten_dB = stopBandAtten;
		resLib.prmInit.socketVal = socket;
		resD = jvx_fixed_resampler_prepare(&resLib);
		if (resD != JVX_DSP_NO_ERROR)
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
#else

		this->formatData = format;
		switch(formatData)
		{
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_16BIT_LE:
			szElement = jvxDataFormat_size[formatData];
			break;
		default:
			res = JVX_ERROR_INVALID_SETTING;
		}

		int i;
		// check vector sizes

		vector_size_input = size_input;
		vector_size_output = size_output;

		// get some parameters
		stopband_attenuation = stopBandAtten;
		socketKaiser = socket;

		jvxData inFreqReplace = out_freq *((jvxData)vector_size_input/(jvxData)vector_size_output);
		jvxData outFreqReplace = in_freq *((jvxData)vector_size_output/(jvxData)vector_size_input);

		// Update the sampling frequencies according to buffersizes
		if(adaptOutputFreqIfNecessary)
		{
			out_freq = outFreqReplace;
		}
		else
		{
			in_freq = inFreqReplace;
		}
		// ... design parameters
		inSamplerate = in_freq;
		outSamplerate = out_freq;

		//   jvxData insmplfreq = in_freq;
		//  jvxData outsmplfreq = out_freq;

		// ... compute numerator and denominator of the fraction
		unsigned long g2 = jvx_gcd ( ( long ) ( vector_size_input - vector_size_output ),
			( long ) vector_size_output );
		z = ( long ) ( vector_size_input - vector_size_output ) / ( long ) g2;
		n = ( long ) vector_size_output / ( long ) g2;

		// Print a warning if setup fails!
		if((inSamplerate/vector_size_input) != (outSamplerate/vector_size_output))
		{
			std::cout << "Warning: Buffersizes and samplerates do not match:" << std::endl;
			std::cout << "Input: rate:" << inSamplerate << ", size:" << vector_size_input << std::endl;
			std::cout << "Output: rate:" << outSamplerate << ", size:" << vector_size_output << std::endl;
		}

		// Frequenz nach Hochtastung
		jvxData f0 = n * inSamplerate;

		// ... design filter coefficients

		// Determine size of oversampled filter
		filterLengthOriginal = filter_length;
		filterLengthUpsampled = n * ( filter_length - 1 ) + 1;

		// Determine middle position
		shift = ((jvxData)filterLengthUpsampled-1.0) / 2.0;

		//============================================================
		// DETERMINE THE WINDOWED LOWPASS PROTOTYPE
		jvxData delta_f;
		if ( stopband_attenuation > 21.0 )
			delta_f = ( f0 * ( stopband_attenuation - 7.95 ) )
			/ ( 14.36 * ( filterLengthUpsampled - 1 ) );
		else
			delta_f = ( 0.9222 * f0 ) / ( filterLengthUpsampled - 1 );

		// Determine min frequency for lowpass
		jvxData f_min = ( inSamplerate < outSamplerate ? inSamplerate : outSamplerate );

		// Store the frequency for lowpass
		fminLowpass = f_min -delta_f;

		// Find a distance to minimum freq
		jvxData Omega_g = ( f_min - delta_f ) / f0;

		jvxData alpha;
		if ( stopband_attenuation > 50.0 )
			alpha = 0.1102 * ( stopband_attenuation - 8.7 );
		else if ( stopband_attenuation <= 21.0 )
			alpha = 0.0;
		else
			alpha = 0.5842 * pow ( stopband_attenuation - 21.0, 0.4 )
			+ 0.07886 * ( stopband_attenuation - 21.0 );

		// Compute the filter coefficients
		jvxData* filterCompute = (jvxData*)calloc ( filterLengthUpsampled + 2 * n, sizeof(jvxData) );
		jvxData* filterCompute_ptr = (jvxData*)filterCompute;
		filterCompute_ptr += n;

		jvx_compute_window(filterCompute_ptr, JVX_SIZE_INT32(filterLengthUpsampled), alpha, socketKaiser, JVX_WINDOW_KAISER, NULL);

		jvxData m_pi_omega_g = M_PI * Omega_g;
		for ( i = 0; i < filterLengthUpsampled; i++ )
		{
			filterCompute_ptr[ i ] *= jvx_si ( m_pi_omega_g * (( jvxData )i - shift ) )
				* Omega_g;
		}

		// .. number of coefficients - 1 (no over-sampling)
		hsize = ( int ) ( ( jvxData ) ( filterLengthUpsampled - 1 ) / ( jvxData ) n + 0.5 );

		// ... correction factor (due to down-sampling)
		for ( i = 0; i < filterLengthUpsampled; i++ )
			filterCompute_ptr[ i ] *= ( jvxData ) n;


		switch(formatData)
		{
		case JVX_DATAFORMAT_DATA:
		case JVX_DATAFORMAT_32BIT_LE:
			filter = calloc ( filterLengthUpsampled + 2 * n, szElement );
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			// Little irregularity here: Store the filter coefficients in 32 bit Q
			filter = calloc ( filterLengthUpsampled + 2 * n, sizeof(jvxInt32) );
			break;
		default:
			assert(0);
		}

		jvxData* filter_ptr_dbl = (jvxData*)filter;
		jvxInt32* filter_ptr_int32 = (jvxInt32*)filter;
		jvxInt32* filter_ptr_int16 = (jvxInt32*)filter;

		switch(formatData)
		{
		case JVX_DATAFORMAT_DATA:
			// Data operation

			for(i = 0; i < filterLengthUpsampled + 2 * n; i++)
				filter_ptr_dbl[i] = filterCompute[i];
			filter_ptr_dbl += n;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			for(i = 0; i < filterLengthUpsampled + 2 * n; i++)
				JVX_DATA2INT32Q(filterCompute[i], QFORMAT_32BIT, filter_ptr_int32+i);
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			for(i = 0; i < filterLengthUpsampled + 2 * n; i++)
				JVX_DATA2INT32Q(filterCompute[i], QFORMAT_32BIT, filter_ptr_int16+i);
			break;
		default:
			assert(0);
		}

		free(filterCompute);
		// Determine required vector-size for input and states
		state_size = vector_size_input + hsize;

		// allocate state buffer
		statesBuffer = calloc ( state_size, szElement );

		// initialize helpers
		in_N_Counter = 0;
#endif
	}
	return res;
}

#if 0
/**
* Second version of the initialize function: The parameter freqMin specifies the
* passband edge frequency which is usually determined automatically..
*///==================================================
jvxErrorType
CjvxResampler::prepare(jvxData freqMin,
		       jvxSize size_input,
		       jvxSize size_output,
		       jvxData in_freq,
		       jvxData out_freq,
		       jvxSize filter_length,
		       jvxData stopBandAtten,
		       jvxData socket )
{
	jvxErrorType res = CjvxObject::_prepare(NULL, 0);

	if(res == JVX_NO_ERROR)
	{
		res = CjvxObject::_start();
	}
	if(res == JVX_NO_ERROR)
	{
		int i;
		// check vector sizes

		vector_size_input = size_input;
		vector_size_output = size_output;

		// get some parameters
		stopband_attenuation = stopBandAtten;
		socketKaiser = socket;

		// ... design parameters
		inSamplerate = in_freq;
		outSamplerate = out_freq;

		jvxData insmplfreq = (jvxData) in_freq;
		jvxData outsmplfreq = (jvxData) out_freq;

		// ... compute numerator and denominator of the fraction

		unsigned long g = jvx_gcd ( ( long ) ( insmplfreq - outsmplfreq ),
			( long ) outsmplfreq );
		z = ( long ) ( insmplfreq - outsmplfreq ) / ( long ) g;
		n = ( long ) outsmplfreq / ( long ) g;

		// Print a warning if setup fails!
		if((insmplfreq/vector_size_input) != (outsmplfreq/vector_size_output))
		{
			std::cout << "Warning: Buffersizes and samplerates do not match:" << std::endl;
			std::cout << "Input: rate:" << insmplfreq << ", size:" << vector_size_input << std::endl;
			std::cout << "Output: rate:" << outsmplfreq << ", size:" << vector_size_output << std::endl;
		}

		// Frequenz nach Hochtastung
		jvxData f0 = n * insmplfreq;

		// ... design filter coefficients

		// Determine size of oversampled filter
		filterLengthOriginal = filter_length;
		filterLengthUpsampled = n * ( filter_length - 1 ) + 1;

		// Determine middle position
		shift = ((jvxData)filterLengthUpsampled-1.0) / 2;

		//============================================================
		// DETERMINE THE WINDOWED LOWPASS PROTOTYPE
		jvxData delta_f;
		if ( stopband_attenuation > 21.0 )
			delta_f = ( f0 * ( stopband_attenuation - 7.95 ) )
			/ ( 14.36 * ( filterLengthUpsampled - 1 ) );
		else
			delta_f = ( 0.9222 * f0 ) / ( filterLengthUpsampled - 1 );

		// Store the frequency for lowpass
		fminLowpass = freqMin;

		// Find a distance to minimum freq
		jvxData Omega_g = ( fminLowpass ) / f0;

		jvxData alpha;
		if ( stopband_attenuation > 50.0 )
			alpha = 0.1102 * ( stopband_attenuation - 8.7 );
		else if ( stopband_attenuation <= 21.0 )
			alpha = 0.0;
		else
			alpha = 0.5842 * pow ( stopband_attenuation - 21.0, 0.4 )
			+ 0.07886 * ( stopband_attenuation - 21.0 );

		// ... allocate memory

		filter = calloc ( filterLengthUpsampled + 2 * n, szElement );

		jvxData* filter_ptr_dbl = reinterpret_cast<jvxData*>(filter);
		filter_ptr_dbl += n;

		//============================================================
		// ... design filter with Kaiser window
		jvx_compute_window(filter_ptr_dbl, JVX_SIZE_INT32(filterLengthUpsampled), alpha, socketKaiser, JVX_WINDOW_KAISER, NULL);

		jvxData m_pi_omega_g = M_PI * Omega_g;
		for ( i = 0; i < filterLengthUpsampled; i++ )
			filter_ptr_dbl [ i ] *= jvx_si ( m_pi_omega_g * ( jvxData ) ( i - shift ) )
			* Omega_g;

		//============================================================

		// .. number of coefficients - 1 (no over-sampling)
		hsize = ( int ) ( ( jvxData ) ( filterLengthUpsampled - 1 ) / ( jvxData ) n + 0.5 );

		// ... correction factor (due to down-sampling)
		for ( i = 0; i < filterLengthUpsampled; i++ )
			filter_ptr_dbl [ i ] *= ( jvxData ) n;

		// Determine required vector-size for input and states
		state_size = vector_size_input + hsize;

		// allocate state buffer
		statesBuffer = calloc ( state_size, szElement);

		// initialize helpers
		in_N_Counter = 0;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxResampler::properties(jvxData* ratein,
			  jvxData* rateout,
			  jvxSize* bsize_in,
			  jvxSize* bsize_out,
			  jvxSize* filter_length,
			  jvxSize* filter_length_orig,
			  jvxData* nominator,
			  jvxData* denom,
			  jvxData* freqMin,
			  jvxInt32* oversample,
			  jvxData* delay_used)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState >= JVX_STATE_PREPARED)
	{
		if(freqMin)
		{
			*freqMin = fminLowpass;
		}

		if(ratein)
		{
			*ratein = inSamplerate;
		}

		if(rateout)
		{
			*rateout = outSamplerate;
		}

		if(nominator)
		{
			*nominator = (z+1);
		}
		if(denom)
		{
			*denom = n;
		}

		if(filter_length)
		{
			*filter_length = filterLengthUpsampled;
		}

		if(bsize_in)
		{
			*bsize_in = vector_size_input;
		}

		if(bsize_out)
		{
			*bsize_out = vector_size_output;
		}

		if(filter_length_orig)
		{
			*filter_length_orig = filterLengthOriginal;
		}

		if(delay_used)
		{
			jvxData latency = (jvxData)shift/(jvxData)n;
			*delay_used = latency;
		}

		if(oversample)
		{
			*oversample = (jvxInt32)n;
		}

	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}
#endif



/**
 * Main filter routine, NOT using cyclic buffers. This is the faster way of implementation
 * for the PC due to x86 lack of addressing units!
 *///==================================================
jvxErrorType
CjvxResampler::process(jvxHandle* input,
		       jvxHandle* output)
{
	int j,i;
	jvxInt32 N_Counter = (jvxInt32)hsize;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
#ifdef JVX_RESAMPLER_DSP_LIB
		jvxDspBaseErrorType resD = JVX_DSP_NO_ERROR;
		resD = jvx_fixed_resampler_process(&resLib, input, output, nullptr); // Full size resampling
		if (resD != JVX_DSP_NO_ERROR)
		{
			res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
		}
#else
		jvxData* output_dbl = reinterpret_cast<jvxData*>(output);
		jvxData* state_ptr_dbl;
		jvxData* statesBuf_dbl = reinterpret_cast<jvxData*>(statesBuffer);
		jvxData* filter_ptr_dbl = NULL;
		jvxData* filter_dbl = reinterpret_cast<jvxData*>(filter);

		jvxInt32* output_int32 = reinterpret_cast<jvxInt32*>(output);
		jvxInt32* state_ptr_int32;
		jvxInt32* statesBuf_int32 = reinterpret_cast<jvxInt32*>(statesBuffer);
		jvxInt32* filter_ptr_int32 = NULL;
		jvxInt32* filter_int32 = reinterpret_cast<jvxInt32*>(filter);

		jvxInt16* output_int16 = reinterpret_cast<jvxInt16*>(output);
		jvxInt16* state_ptr_int16;
		jvxInt16* statesBuf_int16 = reinterpret_cast<jvxInt16*>(statesBuffer);
		jvxInt32* filter_ptr_int16 = NULL;
		jvxInt32* filter_int16 = reinterpret_cast<jvxInt32*>(filter);

		switch(this->formatData)
		{
		case JVX_DATAFORMAT_DATA:
			memcpy(statesBuf_dbl, &statesBuf_dbl[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_dbl[(state_size - vector_size_input)], input, szElement*vector_size_input);

			for ( i = 0; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_dbl = filter_dbl + in_N_Counter + n;
				state_ptr_dbl = statesBuf_dbl + N_Counter;

				jvxData sum = 0.0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += *state_ptr_dbl-- * *filter_ptr_dbl;
					filter_ptr_dbl += n;
				}
				*output_dbl++ = sum;
			}
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			memcpy(statesBuf_int32, &statesBuf_int32[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_int32[(state_size - vector_size_input)], input, szElement*vector_size_input);

			for ( i = 0; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int32 = filter_int32 + in_N_Counter + n;
				state_ptr_int32 = statesBuf_int32 + N_Counter;

				long long sum = 0;

				for ( j = 0; j < hsize ; j ++ )
				{
					sum += (long long)(*state_ptr_int32) * (long long)(*filter_ptr_int32);
					state_ptr_int32--;
					filter_ptr_int32 += n;
				}
				*output_int32++ = (jvxInt32)(sum >> QFORMAT_32BIT);
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			memcpy(statesBuf_int16, &statesBuf_int16[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_int16[(state_size - vector_size_input)], input, szElement*vector_size_input);

			for ( i = 0; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int16 = filter_int16 + in_N_Counter + n;
				state_ptr_int16 = statesBuf_int16 + N_Counter;

				long long sum = 0;

				for ( j = 0; j < hsize ; j ++ )
				{
					// Make 32 bit out of 16 first
					sum += ((long long)(*state_ptr_int16) << 16) * (long long)(*filter_ptr_int16);
					state_ptr_int16--;
					filter_ptr_int16 += n;
				}
				*output_int16++ = (jvxInt16)(sum >> (QFORMAT_32BIT+16));
			}
			break;
		default:
			assert(0);
		}
#endif
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

#if 0
jvxErrorType
CjvxResampler::process(jvxHandle* inputPart1,
		       jvxSize input_szPart1,
		       jvxHandle* inputPart2,
		       jvxHandle* outputPart1,
		       jvxSize output_szPart1,
		       jvxHandle* outputPart2)
{
	int j,i;
	jvxInt32 N_Counter = (jvxInt32)hsize;
	jvxErrorType res = JVX_NO_ERROR;
	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		jvxData* outputPart1_dbl = reinterpret_cast<jvxData*>(outputPart1);
		jvxData* outputPart2_dbl = reinterpret_cast<jvxData*>(outputPart2);
		jvxData* state_ptr_dbl;
		jvxData* statesBuf_dbl = reinterpret_cast<jvxData*>(statesBuffer);
		jvxData* filter_ptr_dbl = NULL;
		jvxData* filter_dbl = reinterpret_cast<jvxData*>(filter);

		jvxInt32* outputPart1_int32 = reinterpret_cast<jvxInt32*>(outputPart1);
		jvxInt32* outputPart2_int32 = reinterpret_cast<jvxInt32*>(outputPart2);
		jvxInt32* state_ptr_int32;
		jvxInt32* statesBuf_int32 = reinterpret_cast<jvxInt32*>(statesBuffer);
		jvxInt32* filter_ptr_int32 = NULL;
		jvxInt32* filter_int32 = reinterpret_cast<jvxInt32*>(filter);

		jvxInt16* outputPart1_int16 = reinterpret_cast<jvxInt16*>(outputPart1);
		jvxInt16* outputPart2_int16 = reinterpret_cast<jvxInt16*>(outputPart2);
		jvxInt16* state_ptr_int16;
		jvxInt16* statesBuf_int16 = reinterpret_cast<jvxInt16*>(statesBuffer);
		jvxInt32* filter_ptr_int16 = NULL;
		jvxInt32* filter_int16 = reinterpret_cast<jvxInt32*>(filter);

		switch(this->formatData)
		{
		case JVX_DATAFORMAT_DATA:
			memcpy(statesBuf_dbl, &statesBuf_dbl[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_dbl[(state_size - vector_size_input)], inputPart1, szElement*input_szPart1);
			memcpy(&statesBuf_dbl[(state_size - vector_size_input+ input_szPart1)], inputPart2, szElement*(vector_size_input-input_szPart1));

			for ( i = 0; i < output_szPart1; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_dbl = filter_dbl + in_N_Counter + n;
				state_ptr_dbl = statesBuf_dbl + N_Counter;

				jvxData sum = 0.0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += *state_ptr_dbl-- * *filter_ptr_dbl;
					filter_ptr_dbl += n;
				}
				*outputPart1_dbl++ = sum;
			}
			for ( ; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_dbl = filter_dbl + in_N_Counter + n;
				state_ptr_dbl = statesBuf_dbl + N_Counter;

				jvxData sum = 0.0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += *state_ptr_dbl-- * *filter_ptr_dbl;
					filter_ptr_dbl += n;
				}
				*outputPart2_dbl++ = sum;
			}
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			memcpy(statesBuf_int32, &statesBuf_int32[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_int32[(state_size - vector_size_input)], inputPart1, szElement*input_szPart1);
			memcpy(&statesBuf_int32[(state_size - vector_size_input+ input_szPart1)], inputPart2, szElement*(vector_size_input-input_szPart1));

			for ( i = 0; i < output_szPart1; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int32 = filter_int32 + in_N_Counter + n;
				state_ptr_int32 = statesBuf_int32 + N_Counter;

				long long sum = 0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += (long long)(*state_ptr_int32) * (long long)(*filter_ptr_int32);
					state_ptr_int32--;
					filter_ptr_int32 += n;
				}
				*outputPart1_int32++ = (jvxInt32)(sum >> QFORMAT_32BIT);
			}
			for ( ; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int32 = filter_int32 + in_N_Counter + n;
				state_ptr_int32 = statesBuf_int32 + N_Counter;

				long long sum = 0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += (long long)(*state_ptr_int32) * (long long)(*filter_ptr_int32);
					state_ptr_int32--;
					filter_ptr_int32 += n;
				}
				*outputPart2_int32++ = (jvxInt32)(sum >> QFORMAT_32BIT);
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			memcpy(statesBuf_int16, &statesBuf_int16[vector_size_input], szElement*(state_size - vector_size_input));
			memcpy(&statesBuf_int16[(state_size - vector_size_input)], inputPart1, szElement*input_szPart1);
			memcpy(&statesBuf_int16[(state_size - vector_size_input+ input_szPart1)], inputPart2, szElement*(vector_size_input-input_szPart1));

			for ( i = 0; i < output_szPart1; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int16 = filter_int16 + in_N_Counter + n;
				state_ptr_int16 = statesBuf_int16 + N_Counter;

				long long sum = 0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += ((long long)(*state_ptr_int16) << 16) * (long long)(*filter_ptr_int16);
					state_ptr_int16--;
					filter_ptr_int16 += n;
				}

				*outputPart1_int16++ = (jvxInt16)(sum >> (QFORMAT_32BIT+16));
			}
			for ( ; i < vector_size_output; i ++, N_Counter++ )
			{
				in_N_Counter += z;

				N_Counter += in_N_Counter / n;

				in_N_Counter %= n;

				// calculate output sample
				filter_ptr_int16 = filter_int16 + in_N_Counter + n;
				state_ptr_int16 = statesBuf_int16 + N_Counter;

				long long sum = 0;
				for ( j = 0; j < hsize ; j ++ )
				{
					sum += ((long long)(*state_ptr_int16) << 16) * (long long)(*filter_ptr_int16);
					state_ptr_int16--;
					filter_ptr_int16 += n;
				}
				*outputPart2_int16++ = (jvxInt16)(sum >> (QFORMAT_32BIT+16));
			}
			break;
		default:
			assert(0);
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}
#endif

jvxErrorType
CjvxResampler::postprocess()
{
	jvxErrorType res = CjvxObject::_stop();

	if(res == JVX_NO_ERROR)
	{
		CjvxObject::_postprocess();
	}
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_RESAMPLER_DSP_LIB
		jvx_fixed_resampler_postprocess(&resLib);

#else
		filterLengthUpsampled = 0;
		vector_size_input = 0;
		vector_size_output = 0;
		if(filter != NULL)
			free(filter);
		filter = NULL;
		if(statesBuffer)
			free(statesBuffer);
		statesBuffer = NULL;
#endif

	}
	return res;
}


/**
 * Filter termination routine. This deallocates all buffers
 * and sets counting variables to zero!
 *///==================================================
jvxErrorType
CjvxResampler::terminate()
{

	if(_common_set_min.theState == JVX_STATE_PROCESSING)
	{
		this->postprocess();
	}

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxObject::_deactivate();
		CjvxObject::_unselect();
		CjvxObject::_terminate();
	}
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxResampler::getSignalDelay_precompute(jvxData* ret,
					 jvxSize vector_size_input,
					 jvxSize vector_size_output,
					 jvxSize filter_length)
{
	unsigned long g2 = jvx_gcd ( JVX_SIZE_INT32 ( vector_size_input - vector_size_output ),
				     JVX_SIZE_INT32( vector_size_output ));
	long zl = ( long ) ( vector_size_input - vector_size_output ) / ( long ) g2;
	long nl = ( long ) vector_size_output / ( long ) g2;
	long filterLengthUpsampled = nl * ( JVX_SIZE_INT(filter_length) - 1 ) + 1;

	jvxData shift = ((jvxData)filterLengthUpsampled-1.0) / 2.0;

	jvxData latency = (jvxData)shift/(jvxData)nl;
	if(ret)
	{
		*ret = latency;
	}
	return JVX_NO_ERROR;
}
