#include "jvx_dsp_base.h"
#include "jvx_resampler/jvx_fixed_resampler.h"
#include <assert.h>
#include "jvx_resampler/jvx_fixed_resampler_config.h"
#include "jvx_math/jvx_math_extensions.h"
#include "jvx_windows/jvx_windows.h"
#include "jvx_circbuffer/jvx_circbuffer.h"

#define JVX_RUN_DIRECT_IMPLEMENTATION

//#define JVX_RESAMPLER_OLD_IMPLEMENTATION

// Configuration of the IIR resampler filter: Stopband attenuation
typedef enum
{
	RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_40DB = 0,
	RESAMPLER_IIR_CONFIG_STOPBAND_ATTENUATION_60DB = 1
} jvxAdaptiveResampler_config_stopband;

#include "jvx_tables_resampler.h"

// Struct for fir
typedef struct
{
	struct
	{
		int id;
	} ident;

	jvx_fixed_resampler_prmInit init;

	struct
	{
		jvxSize oversamplingFactor;
		jvxSize downsamplingFactor;
		jvxSize buffersizeUpsampled;
		jvxSize lFilterUpsampled;
		jvxSize midPosition;
		jvxData alpha;
		jvxData delta_f;
		jvxData group_delay;
		jvxSize lStates;
	} constants;

	struct
	{
		jvxData* impResp;
#ifdef JVX_RESAMPLER_OLD_IMPLEMENTATION
		jvxData* states;
#endif
	} ram;

#ifndef JVX_RESAMPLER_OLD_IMPLEMENTATION
	struct
	{
		jvx_circbuffer* states;
		jvxData* impResp;
		jvxSize impResp_length;
		jvxSize impResp_vlength;
		jvxData* impResp_ptr;
		jvxSize inFrameOffset;
	} lowmemram;
#endif

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	jvx_fixed_resampler dirInst;
#endif

} frs_vFIR;

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// Fixed resampler, VARIANT FIR
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

jvxDspBaseErrorType jvx_fixed_resampler_initConfig(jvx_fixed_resampler* H)
{
	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	H->prmInit.buffersizeIn = 128;
	H->prmInit.buffersizeOut = 256;
	H->prmInit.format = JVX_DATAFORMAT_DATA;
	H->prmInit.lFilter = 32;
	H->prmInit.socketVal = 0.0;
	H->prmInit.stopbandAtten_dB = 60;

	H->prmDerived.delay = 0;

	H->prv = NULL;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fixed_resampler_prepare_direct(jvx_fixed_resampler* H,
	jvxSize lFilter,
	jvxSize bsizein,
	jvxSize bsizeout,
	jvxDataFormat form,
	jvxSize osampling,
	jvxSize downsampling,
	jvx_circbuffer* state,
	jvxData* impResp)
{
	frs_vFIR* h = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(h, frs_vFIR);
	memset(h, 0, sizeof(frs_vFIR));

	H->prv = h;

	h->init.buffersizeIn = bsizein;
	h->init.buffersizeOut = bsizeout;

	h->constants.oversamplingFactor = osampling;
	h->constants.downsamplingFactor = downsampling;

	h->init.lFilter = lFilter;
	h->init.format = form;
#ifdef JVX_RESAMPLER_OLD_IMPLEMENTATION	
	h->ram.impResp = impResp;
	assert(0);
	h->ram.states = state;
#else
	h->constants.lFilterUpsampled = (h->init.lFilter - 1) * h->constants.oversamplingFactor + 1;
	h->lowmemram.impResp = impResp;
	h->lowmemram.impResp_vlength = (h->constants.lFilterUpsampled - 1) / 2 + 1;
	h->lowmemram.impResp_length = (h->lowmemram.impResp_vlength + (h->constants.oversamplingFactor - 1));
	h->lowmemram.inFrameOffset = 0;
	h->lowmemram.impResp_ptr = h->lowmemram.impResp + (h->constants.oversamplingFactor - 1);

	h->lowmemram.states = state;
#endif

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	h->dirInst.prv = NULL;
#endif
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_fixed_resampler_postprocess_direct(jvx_fixed_resampler* H)
{
	JVX_DSP_SAFE_DELETE_OBJECT(H->prv);
	H->prv = NULL;
	return JVX_DSP_NO_ERROR;
}

// Iam not sure why but this function - even if not used - pulls in too much code on DSPs
#ifndef JVX_DSP_BASE_COMPILE_MINIMAL_SIZE

/**
 * Initialize fixed resampler, variant FIR.
 *///=========================================================================================================
jvxDspBaseErrorType
jvx_fixed_resampler_prepare(jvx_fixed_resampler* H)
{
	jvxSize i, tmp;

	frs_vFIR* handle;

	jvxData fgOmega;
	jvxData Omega_g;
	jvxData m_pi_omega_g;

	// Do not accept NULL pointer for handle
	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	if (H->prv)
		return JVX_DSP_ERROR_WRONG_STATE;

	// ====================================================================
	// Start config
	// ====================================================================
	// Configuration for lowpass design, FIR filter with Kaiser window

	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(handle, frs_vFIR);

	handle->ident.id = 1;
	handle->init = H->prmInit;

	// Some common parameters
	tmp = (jvxSize)jvx_gcd((jvxInt32)handle->init.buffersizeIn, (jvxInt32)handle->init.buffersizeOut);
	handle->constants.downsamplingFactor = handle->init.buffersizeIn / tmp;
	handle->constants.oversamplingFactor = handle->init.buffersizeOut / tmp;
	handle->constants.buffersizeUpsampled = handle->init.buffersizeIn * handle->constants.oversamplingFactor;

	// If we want to have a clear algorithmic delay only allow for odd
	// filter length
	if (handle->init.lFilter % 2 == 0)
	{
		handle->init.lFilter = handle->init.lFilter + 1;
	}

	// ====================================================================
	// Low pass design Low pass design Low pass design Low pass design
	// ====================================================================
	// Setup some parameters
	handle->constants.lFilterUpsampled = (handle->init.lFilter - 1) * handle->constants.oversamplingFactor + 1;
	handle->constants.midPosition = (handle->init.lFilter - 1) / 2 * handle->constants.oversamplingFactor;
	handle->constants.alpha = 0;

	// From the stopband attenuation, derive the factor alpha for Kaiser
	// window
	if (handle->init.stopbandAtten_dB > 21)
	{
		if (handle->init.stopbandAtten_dB > 50)
		{
			handle->constants.alpha = 0.1102 * (handle->init.stopbandAtten_dB - 8.7);
		}
		else
		{
			handle->constants.alpha = 0.5842 * pow((handle->init.stopbandAtten_dB - 21.0), 0.4) + 0.07886*(handle->init.stopbandAtten_dB - 21);
		}
	}

	// Allocate impulse response followed by some sapece for zeros
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.impResp, jvxData, (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));
	memset(handle->ram.impResp, 0, sizeof(jvxData) * (handle->constants.lFilterUpsampled + handle->constants.oversamplingFactor));

	jvx_compute_window(handle->ram.impResp, (jvxInt32)handle->constants.lFilterUpsampled, handle->constants.alpha, 0,
		JVX_WINDOW_KAISER, NULL);

	// Next, derive the desired frequency response parameters: Consider
	// a specific distance to the maximum allowed frequency
	handle->constants.delta_f = 0;
	if (handle->init.stopbandAtten_dB > 21.0)
	{
		handle->constants.delta_f = ((handle->init.stopbandAtten_dB - 7.95)) / (14.36 * (handle->constants.lFilterUpsampled - 1));
	}
	else
	{
		handle->constants.delta_f = (0.9222) / (handle->constants.lFilterUpsampled - 1);
	}

	fgOmega = JVX_MIN(1.0 / (jvxData)handle->constants.oversamplingFactor, 1.0 / (jvxData)handle->constants.downsamplingFactor);
	Omega_g = (fgOmega - handle->constants.delta_f);
	m_pi_omega_g = M_PI * Omega_g;

	// Compute the time domain impulse response based on the sin(x)/x
	for (i = 0; i < handle->constants.lFilterUpsampled; i++)
	{
		handle->ram.impResp[i] *= jvx_si(m_pi_omega_g * ((jvxData)i - handle->constants.midPosition))* Omega_g;
	}

	// ====================================================================
	// Allocate the relevant buffers for interpolation filter
	// ====================================================================

#ifdef JVX_RESAMPLER_OLD_IMPLEMENTATION
	// State buffer for convolution
	handle->constants.lStates = (handle->init.lFilter - 1) + handle->init.buffersizeIn;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->ram.states, jvxData, handle->constants.lStates);
	memset(handle->ram.states, 0, sizeof(jvxData) * handle->constants.lStates);
#else
	jvx_circbuffer_allocate(&handle->lowmemram.states, handle->init.lFilter - 1, 1, 1);
	handle->lowmemram.impResp_vlength = (handle->constants.lFilterUpsampled - 1) / 2 + 1;
	handle->lowmemram.impResp_length = (handle->lowmemram.impResp_vlength + (handle->constants.oversamplingFactor - 1));
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(handle->lowmemram.impResp, jvxData, handle->lowmemram.impResp_length);
	memset(handle->lowmemram.impResp, 0, sizeof(jvxData)* handle->lowmemram.impResp_length);
	handle->lowmemram.inFrameOffset = 0;
	handle->lowmemram.impResp_ptr = handle->lowmemram.impResp + (handle->constants.oversamplingFactor - 1);
	for (i = 0; i < handle->lowmemram.impResp_vlength; i++)
	{
		handle->lowmemram.impResp_ptr[i] = handle->ram.impResp[i];
	}
#endif
	// ====================================================================
	// Compute the delay of the resampler
	// ====================================================================

	//% Compute the group delay
	//%[handle.fir.groupDelay, x] = grpdelay(handle.fir.impResp,1); <-
	//version computed from impulse response
	handle->constants.group_delay = (handle->constants.midPosition) / (jvxData)handle->constants.oversamplingFactor;

	H->prmDerived.delay = handle->constants.group_delay;

	// Return reference to handle
	H->prv = handle;

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
#ifdef JVX_RESAMPLER_OLD_IMPLEMENTATION
	assert(0);
#else
	jvx_fixed_resampler_prepare_direct(&handle->dirInst,
		handle->init.lFilter,
		handle->init.buffersizeIn,
		handle->init.buffersizeOut,
		handle->init.format,
		handle->constants.oversamplingFactor,
		handle->constants.downsamplingFactor,
		handle->lowmemram.states,
		handle->lowmemram.impResp);
#endif
#endif
	return(JVX_DSP_NO_ERROR);
}

/**
 * Terminate resampler FIR.
 *///=========================================================================================================

jvxDspBaseErrorType
jvx_fixed_resampler_postprocess(jvx_fixed_resampler* H)
{
	// ====================================================================
	// Start config
	// ====================================================================
	// Configuration for lowpass design, FIR filter with Kaiser window

	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	if (!H->prv)
		return JVX_DSP_ERROR_WRONG_STATE;

	frs_vFIR* handle = NULL;
	handle = (frs_vFIR*)H->prv;

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	if (handle->dirInst.prv)
		jvx_fixed_resampler_postprocess_direct(&handle->dirInst);
#endif
	JVX_DSP_SAFE_DELETE_FIELD(handle->ram.impResp);

#ifdef JVX_RESAMPLER_OLD_IMPLEMENTATION	
	JVX_DSP_SAFE_DELETE_FIELD(handle->ram.states);
#else
	JVX_DSP_SAFE_DELETE_FIELD(handle->lowmemram.impResp);
	jvx_circbuffer_deallocate(handle->lowmemram.states);
#endif
	JVX_DSP_SAFE_DELETE_OBJECT(handle);
	H->prv = NULL;
	return(JVX_DSP_NO_ERROR);
}
#endif

#if JVX_RESAMPLER_OLD_IMPLEMENTATION
jvxDspBaseErrorType
jvx_fixed_resampler_process(jvx_fixed_resampler* H, void* in, void* out)
{
	int i, j;

	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	if (!H->prv)
		return JVX_DSP_ERROR_WRONG_STATE;

	frs_vFIR* handle = (frs_vFIR*)H->prv;

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	if (handle->dirInst.prv)
	{
		jvx_fixed_resampler_process(&handle->dirInst, in, out);
	}
	else
	{
#endif

		jvxData* inDbl = (jvxData*)in;
		jvxInt64* inInt64 = (jvxInt64*)in;
		jvxInt32* inInt32 = (jvxInt32*)in;
		jvxInt16* inInt16 = (jvxInt16*)in;
		jvxInt8* inInt8 = (jvxInt8*)in;

		jvxData* outDbl = (jvxData*)out;
		jvxInt64* outInt64 = (jvxInt64*)out;
		jvxInt32* outInt32 = (jvxInt32*)out;
		jvxInt16* outInt16 = (jvxInt16*)out;
		jvxInt8* outInt8 = (jvxInt8*)out;

		int ll1;
		int ll2;
		jvxData* myPtrFwd;
		jvxData* myPtrBwd;

		//int buffersizeUpsampled_local = handle->constants.buffersizeUpsampled;
		jvxSize buffersize_local = handle->init.buffersizeIn;

		jvxData accuOut = 0;

		// This is the memove to copy new buffers into state buffer. A circular
		// buffer would be more complex on an architecture without circular
		// addressing support
		//memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
		//	sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

		jvxData* ptrStatesIn = handle->ram.states + (handle->init.lFilter - 1); //(handle->constants.lStates-handle->constants.buffersizeInMax);

		jvxSize inFrameOffset;
		jvxSize idxIn_offset;
		jvxSize tmp;
		jvxSize idx_inBuf = 0;

		// Input filter for various datatypes
		switch (handle->init.format)
		{
		case JVX_DATAFORMAT_DATA:
			for (i = 0; i < buffersize_local; i++)
			{
				*ptrStatesIn++ = JVX_DATA_2_DATA(*inDbl);
				inDbl++;
			}
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			for (i = 0; i < buffersize_local; i++)
			{
				*ptrStatesIn++ = JVX_INT32_2_DATA(*inInt32);
				inInt32++;
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			for (i = 0; i < buffersize_local; i++)
			{
				*ptrStatesIn++ = JVX_INT16_2_DATA(*inInt16);
				inInt16++;
			}
			break;
		case JVX_DATAFORMAT_8BIT:
			for (i = 0; i < buffersize_local; i++)
			{
				*ptrStatesIn++ = JVX_INT8_2_DATA(*inInt8);
				inInt8++;
			}
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			for (i = 0; i < buffersize_local; i++)
			{
				*ptrStatesIn++ = JVX_INT64_2_DATA(*inInt64);
				inInt64++;
			}
			break;
		default:
			assert(0);
		}

		// Step I: Produce oversampled output:
		inFrameOffset = 0;
		idxIn_offset = handle->init.lFilter - 1;

		// Input filter for various datatypes
		switch (handle->init.format)
		{
		case JVX_DATAFORMAT_DATA:

			for (i = 0; i < handle->init.buffersizeOut; i++)
			{

				jvxData accu = 0.0;
				jvxSize cntS = idxIn_offset;
				jvxSize cntC = inFrameOffset;

				for (j = 0; j < handle->init.lFilter; j++)
				{
					jvxData op1 = handle->ram.states[cntS];
					jvxData op2 = handle->ram.impResp[cntC];
					accu = accu + op1 * op2;
					cntC += handle->constants.oversamplingFactor;
					cntS--;
				}

				// Consider the power manipulation due to oversampling, produce temp output
				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;

				// Write output to buffer out
				*outDbl++ = JVX_DATA_2_DATA(accuOut);

				// Index update
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				idxIn_offset += tmp;
			}

			break;
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_24BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{

				jvxData accu = 0.0;
				jvxSize cntS = idxIn_offset;
				jvxSize cntC = inFrameOffset;

				for (j = 0; j < handle->init.lFilter; j++)
				{
					jvxData op1 = handle->ram.states[cntS];
					jvxData op2 = handle->ram.impResp[cntC];
					accu = accu + op1 * op2;
					cntC += handle->constants.oversamplingFactor;
					cntS--;
				}

				// Consider the power manipulation due to oversampling, produce temp output
				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;

				// Write output to buffer out
				*outInt32++ = JVX_DATA2INT32(accuOut);

				// Index update
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				idxIn_offset += tmp;
			}
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{

				jvxData accu = 0.0;
				jvxSize cntS = idxIn_offset;
				jvxSize cntC = inFrameOffset;

				for (j = 0; j < handle->init.lFilter; j++)
				{
					jvxData op1 = handle->ram.states[cntS];
					jvxData op2 = handle->ram.impResp[cntC];
					accu = accu + op1 * op2;
					cntC += handle->constants.oversamplingFactor;
					cntS--;
				}

				// Consider the power manipulation due to oversampling, produce temp output
				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;

				// Write output to buffer out
				*outInt16++ = JVX_DATA2INT16(accuOut);

				// Index update
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				idxIn_offset += tmp;
			}
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{

				jvxData accu = 0.0;
				jvxSize cntS = idxIn_offset;
				jvxSize cntC = inFrameOffset;

				for (j = 0; j < handle->init.lFilter; j++)
				{
					jvxData op1 = handle->ram.states[cntS];
					jvxData op2 = handle->ram.impResp[cntC];
					accu = accu + op1 * op2;
					cntC += handle->constants.oversamplingFactor;
					cntS--;
				}

				// Consider the power manipulation due to oversampling, produce temp output
				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;

				// Write output to buffer out
				*outInt64++ = JVX_DATA2INT64(accuOut);

				// Index update
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				idxIn_offset += tmp;
			}
			break;
		case JVX_DATAFORMAT_8BIT:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{

				jvxData accu = 0.0;
				jvxSize cntS = idxIn_offset;
				jvxSize cntC = inFrameOffset;

				for (j = 0; j < handle->init.lFilter; j++)
				{
					jvxData op1 = handle->ram.states[cntS];
					jvxData op2 = handle->ram.impResp[cntC];
					accu = accu + op1 * op2;
					cntC += handle->constants.oversamplingFactor;
					cntS--;
				}

				// Consider the power manipulation due to oversampling, produce temp output
				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;

				// Write output to buffer out
				*outInt8++ = JVX_DATA2INT8(accuOut);

				// Index update
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				idxIn_offset += tmp;
			}
			break;
		default:
			assert(0);
		}


		// Shift the states in buffers for next frame
		memmove(handle->ram.states, handle->ram.states + buffersize_local,
			sizeof(jvxData) * (handle->init.lFilter - 1));
#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	}
#endif

	return(JVX_DSP_NO_ERROR);
}






#else
jvxDspBaseErrorType
jvx_fixed_resampler_process(jvx_fixed_resampler* H, void* in, void* out)
{
	int i, j;

	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	if (!H->prv)
		return JVX_DSP_ERROR_WRONG_STATE;

	frs_vFIR* handle = (frs_vFIR*)H->prv;

#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	if (handle->dirInst.prv)
	{
		jvx_fixed_resampler_process(&handle->dirInst, in, out);
	}
	else
	{
#endif
		jvxData* inDbl = (jvxData*)in;
		jvxInt64* inInt64 = (jvxInt64*)in;
		jvxInt32* inInt32 = (jvxInt32*)in;
		jvxInt16* inInt16 = (jvxInt16*)in;
		jvxInt8* inInt8 = (jvxInt8*)in;

		jvxData* outDbl = (jvxData*)out;
		jvxInt64* outInt64 = (jvxInt64*)out;
		jvxInt32* outInt32 = (jvxInt32*)out;
		jvxInt16* outInt16 = (jvxInt16*)out;
		jvxInt8* outInt8 = (jvxInt8*)out;

		jvxSize ll1;
		jvxSize ll2;
		jvxData* myPtrFwd;
		jvxData* myPtrBwd;

		jvxData accuOut = 0;

		// This is the memove to copy new buffers into state buffer. A circular
		// buffer would be more complex on an architecture without circular
		// addressing support
		//memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
		//	sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

		jvxSize inFrameOffset = 0;
		jvxSize tmp = 0;
		jvxData accu = 0.0;
		jvxData inData = 0;
		jvxData* fwd = NULL;
		jvxData* bwd = NULL;
		jvxData* stfws = NULL;
		jvxSize cntBwd = 0;
		jvxSize cntFwd = 0;

		inFrameOffset = handle->lowmemram.inFrameOffset;

		switch (handle->init.format)
		{
		case JVX_DATAFORMAT_DATA:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{
				accu = 0.0;
				inData = *inDbl;
				fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
				bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
				cntBwd = handle->lowmemram.states->idxRead;
				cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

				accu = *fwd * inData;

				accu += *bwd * stfws[cntBwd];
				cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

				ll2 = handle->init.lFilter / 2;
				ll1 = handle->lowmemram.states->length - cntBwd;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrBwd = stfws + cntBwd;
				for (j = 0; j < ll1; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				myPtrBwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				fwd += handle->constants.oversamplingFactor;

				ll2 = handle->init.lFilter / 2 - 1;
				ll1 = cntFwd + 1;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrFwd = stfws + cntFwd;
				for (j = 0; j < ll1; j++)
				{
					accu += *fwd * *myPtrFwd;
					myPtrFwd--;
					fwd += handle->constants.oversamplingFactor;
				}
				myPtrFwd = stfws + handle->lowmemram.states->length;
				for (j = 0; j < ll2; j++)
				{
					myPtrFwd--;
					accu += *fwd * *myPtrFwd;// stfws[cntFwd];
					fwd += handle->constants.oversamplingFactor;
				}

				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
				*outDbl++ = JVX_DATA_2_DATA(accuOut);
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

				ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
				ll1 = JVX_MIN(ll1, tmp);
				ll2 = tmp - ll1;

				myPtrFwd = stfws + handle->lowmemram.states->idxRead;

				for (j = 0; j < ll1; j++)
				{
					*myPtrFwd++ = *inDbl++;
				}
				myPtrFwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					*myPtrFwd++ = *inDbl++;
				}
				handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
			}
			handle->lowmemram.inFrameOffset = inFrameOffset;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{
				accu = 0.0;
				inData = JVX_INT16_2_DATA(*inInt16);
				fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
				bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
				cntBwd = handle->lowmemram.states->idxRead;
				cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

				accu = *fwd * inData;

				accu += *bwd * stfws[cntBwd];
				cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

				ll2 = handle->init.lFilter / 2;
				ll1 = handle->lowmemram.states->length - cntBwd;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrBwd = stfws + cntBwd;
				for (j = 0; j < ll1; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				myPtrBwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				fwd += handle->constants.oversamplingFactor;

				ll2 = handle->init.lFilter / 2 - 1;
				ll1 = cntFwd + 1;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrFwd = stfws + cntFwd;
				for (j = 0; j < ll1; j++)
				{
					accu += *fwd * *myPtrFwd;
					myPtrFwd--;
					fwd += handle->constants.oversamplingFactor;
				}
				myPtrFwd = stfws + handle->lowmemram.states->length;
				for (j = 0; j < ll2; j++)
				{
					myPtrFwd--;
					accu += *fwd * *myPtrFwd;// stfws[cntFwd];
					fwd += handle->constants.oversamplingFactor;
				}

				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
				*outInt16++ = JVX_DATA_2_INT16(accuOut);
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

				ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
				ll1 = JVX_MIN(ll1, tmp);
				ll2 = tmp - ll1;

				myPtrFwd = stfws + handle->lowmemram.states->idxRead;

				for (j = 0; j < ll1; j++)
				{
					*myPtrFwd++ = JVX_INT16_2_DATA(*inInt16);
					inInt16++;
				}
				myPtrFwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					*myPtrFwd++ = JVX_INT16_2_DATA(*inInt16);
					inInt16++;
				}
				handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
			}
			handle->lowmemram.inFrameOffset = inFrameOffset;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{
				accu = 0.0;
				inData = JVX_INT32_2_DATA(*inInt32);
				fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
				bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
				cntBwd = handle->lowmemram.states->idxRead;
				cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

				accu = *fwd * inData;

				accu += *bwd * stfws[cntBwd];
				cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

				ll2 = handle->init.lFilter / 2;
				ll1 = handle->lowmemram.states->length - cntBwd;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrBwd = stfws + cntBwd;
				for (j = 0; j < ll1; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				myPtrBwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				fwd += handle->constants.oversamplingFactor;

				ll2 = handle->init.lFilter / 2 - 1;
				ll1 = cntFwd + 1;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrFwd = stfws + cntFwd;
				for (j = 0; j < ll1; j++)
				{
					accu += *fwd * *myPtrFwd;
					myPtrFwd--;
					fwd += handle->constants.oversamplingFactor;
				}
				myPtrFwd = stfws + handle->lowmemram.states->length;
				for (j = 0; j < ll2; j++)
				{
					myPtrFwd--;
					accu += *fwd * *myPtrFwd;// stfws[cntFwd];
					fwd += handle->constants.oversamplingFactor;
				}

				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
				*outInt32++ = JVX_DATA_2_INT32(accuOut);
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

				ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
				ll1 = JVX_MIN(ll1, tmp);
				ll2 = tmp - ll1;

				myPtrFwd = stfws + handle->lowmemram.states->idxRead;

				for (j = 0; j < ll1; j++)
				{
					*myPtrFwd++ = JVX_INT32_2_DATA(*inInt32);
					inInt32++;
				}
				myPtrFwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					*myPtrFwd++ = JVX_INT32_2_DATA(*inInt32);
					inInt32++;
				}
				handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
			}
			handle->lowmemram.inFrameOffset = inFrameOffset;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{
				accu = 0.0;
				inData = JVX_INT64_2_DATA(*inInt64);
				fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
				bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
				cntBwd = handle->lowmemram.states->idxRead;
				cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

				accu = *fwd * inData;

				accu += *bwd * stfws[cntBwd];
				cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

				ll2 = handle->init.lFilter / 2;
				ll1 = handle->lowmemram.states->length - cntBwd;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrBwd = stfws + cntBwd;
				for (j = 0; j < ll1; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				myPtrBwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				fwd += handle->constants.oversamplingFactor;

				ll2 = handle->init.lFilter / 2 - 1;
				ll1 = cntFwd + 1;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrFwd = stfws + cntFwd;
				for (j = 0; j < ll1; j++)
				{
					accu += *fwd * *myPtrFwd;
					myPtrFwd--;
					fwd += handle->constants.oversamplingFactor;
				}
				myPtrFwd = stfws + handle->lowmemram.states->length;
				for (j = 0; j < ll2; j++)
				{
					myPtrFwd--;
					accu += *fwd * *myPtrFwd;// stfws[cntFwd];
					fwd += handle->constants.oversamplingFactor;
				}

				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
				*outInt64++ = JVX_DATA_2_INT64(accuOut);
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

				ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
				ll1 = JVX_MIN(ll1, tmp);
				ll2 = tmp - ll1;

				myPtrFwd = stfws + handle->lowmemram.states->idxRead;

				for (j = 0; j < ll1; j++)
				{
					*myPtrFwd++ = JVX_INT64_2_DATA(*inInt64);
					inInt64++;
				}
				myPtrFwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					*myPtrFwd++ = JVX_INT64_2_DATA(*inInt64);
					inInt64++;
				}
				handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
			}
			handle->lowmemram.inFrameOffset = inFrameOffset;
			break;

		case JVX_DATAFORMAT_8BIT:
			for (i = 0; i < handle->init.buffersizeOut; i++)
			{
				accu = 0.0;
				inData = JVX_INT8_2_DATA(*inInt8);
				fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
				bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
				cntBwd = handle->lowmemram.states->idxRead;
				cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

				accu = *fwd * inData;

				accu += *bwd * stfws[cntBwd];
				cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

				ll2 = handle->init.lFilter / 2;
				ll1 = handle->lowmemram.states->length - cntBwd;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrBwd = stfws + cntBwd;
				for (j = 0; j < ll1; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				myPtrBwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					bwd += handle->constants.oversamplingFactor;
					accu += *bwd * *myPtrBwd;
					myPtrBwd++;
				}
				fwd += handle->constants.oversamplingFactor;

				ll2 = handle->init.lFilter / 2 - 1;
				ll1 = cntFwd + 1;
				ll1 = JVX_MIN(ll1, ll2);
				ll2 = ll2 - ll1;
				myPtrFwd = stfws + cntFwd;
				for (j = 0; j < ll1; j++)
				{
					accu += *fwd * *myPtrFwd;
					myPtrFwd--;
					fwd += handle->constants.oversamplingFactor;
				}
				myPtrFwd = stfws + handle->lowmemram.states->length;
				for (j = 0; j < ll2; j++)
				{
					myPtrFwd--;
					accu += *fwd * *myPtrFwd;// stfws[cntFwd];
					fwd += handle->constants.oversamplingFactor;
				}

				accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
				*outInt8++ = JVX_DATA_2_INT8(accuOut);
				tmp = inFrameOffset + handle->constants.downsamplingFactor;
				inFrameOffset = tmp % handle->constants.oversamplingFactor;
				tmp = tmp / handle->constants.oversamplingFactor;
				stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

				ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
				ll1 = JVX_MIN(ll1, tmp);
				ll2 = tmp - ll1;

				myPtrFwd = stfws + handle->lowmemram.states->idxRead;

				for (j = 0; j < ll1; j++)
				{
					*myPtrFwd++ = JVX_INT8_2_DATA(*inInt8);
					inInt8++;
				}
				myPtrFwd = stfws;
				for (j = 0; j < ll2; j++)
				{
					*myPtrFwd++ = JVX_INT8_2_DATA(*inInt8);
					inInt8++;
				}
				handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
			}
			handle->lowmemram.inFrameOffset = inFrameOffset;
			break;
		default:
			assert(0);
			break;
		}
#ifdef JVX_RUN_DIRECT_IMPLEMENTATION
	}
#endif
	return(JVX_DSP_NO_ERROR);
}

#if defined(_TMS320C6740)
// Specific conversion routine in resampler code to avoid to do this multiple times
#define JVX_INT16_DATA_NORM(valueIn16, valueOutData) \
	valueOutData = (jvxData)valueIn16; \
    valueOutData = ldexpf(valueOutData, -15)
	
#else
#define JVX_INT16_DATA_NORM(valueIn16, valueOutData) \
	valueOutData = JVX_INT32_2_DATA(valueIn16)

#endif

jvxDspBaseErrorType
jvx_fixed_resampler_process_convert(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out)
{
	int i, j;

	if (!H)
	{
		return(JVX_DSP_ERROR_INVALID_ARGUMENT);
	}

	if (!H->prv)
		return JVX_DSP_ERROR_WRONG_STATE;

	frs_vFIR* handle = (frs_vFIR*)H->prv;

	jvxInt16* inInt16 = in;
	jvxData* outDbl = out;

	jvxSize ll1;
	jvxSize ll2;
	jvxData* myPtrFwd;
	jvxData* myPtrBwd;

	jvxData accuOut = 0;

	// This is the memove to copy new buffers into state buffer. A circular
	// buffer would be more complex on an architecture without circular
	// addressing support
	//memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
	//	sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

	jvxSize inFrameOffset = 0;
	jvxSize tmp = 0;
	jvxData accu = 0.0;
	jvxData inData = 0;
	jvxData* fwd = NULL;
	jvxData* bwd = NULL;
	jvxData* stfws = NULL;
	jvxSize cntBwd = 0;
	jvxSize cntFwd = 0;
	jvxData tmpID;
	jvxInt16 tmpI16;
	inFrameOffset = handle->lowmemram.inFrameOffset;

	for (i = 0; i < handle->init.buffersizeOut; i++)
	{
		accu = 0.0;
		//inData = *inDbl;
		tmpI16 = *inInt16;
		JVX_INT16_DATA_NORM(tmpI16, inData);

		fwd = handle->lowmemram.impResp_ptr + inFrameOffset;
		bwd = handle->lowmemram.impResp_ptr - inFrameOffset;
		stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
		cntBwd = handle->lowmemram.states->idxRead;
		cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

		accu = *fwd * inData;

		accu += *bwd * stfws[cntBwd];
		cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

		ll2 = handle->init.lFilter / 2;
		ll1 = handle->lowmemram.states->length - cntBwd;
		ll1 = JVX_MIN(ll1, ll2);
		ll2 = ll2 - ll1;
		myPtrBwd = stfws + cntBwd;
		for (j = 0; j < ll1; j++)
		{
			bwd += handle->constants.oversamplingFactor;
			accu += *bwd * *myPtrBwd;
			myPtrBwd++;
		}
		myPtrBwd = stfws;
		for (j = 0; j < ll2; j++)
		{
			bwd += handle->constants.oversamplingFactor;
			accu += *bwd * *myPtrBwd;
			myPtrBwd++;
		}
		fwd += handle->constants.oversamplingFactor;

		ll2 = handle->init.lFilter / 2 - 1;
		ll1 = cntFwd + 1;
		ll1 = JVX_MIN(ll1, ll2);
		ll2 = ll2 - ll1;
		myPtrFwd = stfws + cntFwd;
		for (j = 0; j < ll1; j++)
		{
			accu += *fwd * *myPtrFwd;
			myPtrFwd--;
			fwd += handle->constants.oversamplingFactor;
		}
		myPtrFwd = stfws + handle->lowmemram.states->length;
		for (j = 0; j < ll2; j++)
		{
			myPtrFwd--;
			accu += *fwd * *myPtrFwd;// stfws[cntFwd];
			fwd += handle->constants.oversamplingFactor;
		}

		accuOut = accu * (jvxData)handle->constants.oversamplingFactor;
		*outDbl++ = JVX_DATA_2_DATA(accuOut);
		tmp = inFrameOffset + handle->constants.downsamplingFactor;
		inFrameOffset = tmp % handle->constants.oversamplingFactor;
		tmp = tmp / handle->constants.oversamplingFactor;
		stfws = (jvxData*)handle->lowmemram.states->ram.field[0];
		
		ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
		ll1 = JVX_MIN(ll1, tmp);
		ll2 = tmp - ll1;

		myPtrFwd = stfws + handle->lowmemram.states->idxRead;

		for (j = 0; j < ll1; j++)
		{
			//*myPtrFwd++ = *inDbl++;
		    tmpI16 = *inInt16++;
			JVX_INT16_DATA_NORM(tmpI16, tmpID);
			*myPtrFwd++ = tmpID;
		}
		myPtrFwd = stfws;
		for (j = 0; j < ll2; j++)
		{
			//*myPtrFwd++ = *inDbl++;
		    tmpI16 = *inInt16++;
			JVX_INT16_DATA_NORM(tmpI16, tmpID);
			*myPtrFwd++ = tmpID;
		}
		handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + tmp) % handle->lowmemram.states->length;
	}
	handle->lowmemram.inFrameOffset = inFrameOffset;

	return(JVX_DSP_NO_ERROR);
}


jvxDspBaseErrorType
jvx_fixed_resampler_process_convert_downonly(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out)
{
    int i, j;

    if (!H)
    {
        return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

    if (!H->prv)
        return JVX_DSP_ERROR_WRONG_STATE;

    frs_vFIR* handle = (frs_vFIR*)H->prv;

    jvxInt16* inInt16 = in;
    jvxData* outDbl = out;

    jvxSize ll1;
    jvxSize ll2;
    jvxData* myPtrFwd;
    jvxData* myPtrBwd;

    jvxData accuOut = 0;

    // This is the memove to copy new buffers into state buffer. A circular
    // buffer would be more complex on an architecture without circular
    // addressing support
    //memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
    //  sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

    jvxData accu = 0.0;
    jvxData inData = 0;
    jvxData* fwd = NULL;
    jvxData* bwd = NULL;
    jvxData* stfws = NULL;
    jvxSize cntBwd = 0;
    jvxSize cntFwd = 0;
    jvxData tmpID;
    jvxInt16 tmpI16;
    stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

    for (i = 0; i < handle->init.buffersizeOut; i++)
    {
        accu = 0.0;
        //inData = *inDbl;
        tmpI16 = *inInt16;
        JVX_INT16_DATA_NORM(tmpI16, inData);

        fwd = handle->lowmemram.impResp_ptr;
        bwd = handle->lowmemram.impResp_ptr;
        cntBwd = handle->lowmemram.states->idxRead;
        cntFwd = (cntBwd + handle->lowmemram.states->length - 1) % handle->lowmemram.states->length;

        accu = *fwd * inData;

        accu += *bwd * stfws[cntBwd];
        cntBwd = (cntBwd + 1) % handle->lowmemram.states->length;

        ll2 = handle->init.lFilter / 2;
        ll1 = handle->lowmemram.states->length - cntBwd;
        ll1 = JVX_MIN(ll1, ll2);
        ll2 = ll2 - ll1;
        myPtrBwd = stfws + cntBwd;
        for (j = 0; j < ll1; j++)
        {
            accu += *++bwd * *myPtrBwd++;
        }
        myPtrBwd = stfws;
        for (j = 0; j < ll2; j++)
        {
            accu += *++bwd * *myPtrBwd++;
        }
        fwd += handle->constants.oversamplingFactor;

        ll2 = handle->init.lFilter / 2 - 1;
        ll1 = cntFwd + 1;
        ll1 = JVX_MIN(ll1, ll2);
        ll2 = ll2 - ll1;
        myPtrFwd = stfws + cntFwd;
        for (j = 0; j < ll1; j++)
        {
            accu += *fwd++ * *myPtrFwd--;
        }
        myPtrFwd = stfws + handle->lowmemram.states->length;
        for (j = 0; j < ll2; j++)
        {
            accu += *fwd++ * *--myPtrFwd;// stfws[cntFwd];
        }

        *outDbl++ = accu;
        ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
        ll1 = JVX_MIN(ll1, handle->constants.downsamplingFactor);
        ll2 = handle->constants.downsamplingFactor - ll1;

        myPtrFwd = stfws + handle->lowmemram.states->idxRead;

        for (j = 0; j < ll1; j++)
        {
            //*myPtrFwd++ = *inDbl++;
            tmpI16 = *inInt16++;
            JVX_INT16_DATA_NORM(tmpI16, tmpID);
            *myPtrFwd++ = tmpID;
        }
        myPtrFwd = stfws;
        for (j = 0; j < ll2; j++)
        {
            //*myPtrFwd++ = *inDbl++;
            tmpI16 = *inInt16++;
            JVX_INT16_DATA_NORM(tmpI16, tmpID);
            *myPtrFwd++ = tmpID;
        }
        handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + handle->constants.downsamplingFactor) % handle->lowmemram.states->length;
    }

    return(JVX_DSP_NO_ERROR);
}

jvxDspBaseErrorType
jvx_fixed_resampler_process_convert_downonly_modand(jvx_fixed_resampler* H, jvxInt16* in, jvxData* out)
{
    int i, j;

    if (!H)
    {
        return(JVX_DSP_ERROR_INVALID_ARGUMENT);
    }

    if (!H->prv)
        return JVX_DSP_ERROR_WRONG_STATE;

    frs_vFIR* handle = (frs_vFIR*)H->prv;

    jvxInt16* inInt16 = in;
    jvxData* outDbl = out;

    jvxSize ll1;
    jvxSize ll2;
    jvxData* myPtrFwd;
    jvxData* myPtrBwd;

    jvxData accuOut = 0;

    // This is the memove to copy new buffers into state buffer. A circular
    // buffer would be more complex on an architecture without circular
    // addressing support
    //memmove(handle->ram.states, handle->ram.states + handle->constants.buffersizeIn,
    //  sizeof(jvxData) * (handle->constants.lStates-handle->constants.buffersizeIn));

    jvxData accu = 0.0;
    jvxData inData = 0;
    jvxData* fwd = NULL;
    jvxData* bwd = NULL;
    jvxData* stfws = NULL;
    jvxSize cntBwd = 0;
    jvxSize cntFwd = 0;
    jvxData tmpID;
    jvxInt16 tmpI16;
    jvxSize lm1 = handle->lowmemram.states->length - 1;
    stfws = (jvxData*)handle->lowmemram.states->ram.field[0];

    for (i = 0; i < handle->init.buffersizeOut; i++)
    {
        accu = 0.0;
        //inData = *inDbl;
        tmpI16 = *inInt16;
        JVX_INT16_DATA_NORM(tmpI16, inData);

        fwd = handle->lowmemram.impResp_ptr;
        bwd = handle->lowmemram.impResp_ptr;
        cntBwd = handle->lowmemram.states->idxRead;
        cntFwd = (cntBwd + lm1);
        cntFwd &= lm1;

        accu = *fwd * inData;

        accu += *bwd * stfws[cntBwd];
        cntBwd = (cntBwd + 1);
        cntBwd &= lm1;

        ll2 = handle->init.lFilter / 2;
        ll1 = handle->lowmemram.states->length - cntBwd;
        ll1 = JVX_MIN(ll1, ll2);
        ll2 = ll2 - ll1;
        myPtrBwd = stfws + cntBwd;
        for (j = 0; j < ll1; j++)
        {
            accu += *++bwd * *myPtrBwd++;
        }
        myPtrBwd = stfws;
        for (j = 0; j < ll2; j++)
        {
            accu += *++bwd * *myPtrBwd++;
        }
        fwd ++;

        ll2 = handle->init.lFilter / 2 - 1;
        ll1 = cntFwd + 1;
        ll1 = JVX_MIN(ll1, ll2);
        ll2 = ll2 - ll1;
        myPtrFwd = stfws + cntFwd;
        for (j = 0; j < ll1; j++)
        {
            accu += *fwd++ * *myPtrFwd--;
        }
        myPtrFwd = stfws + handle->lowmemram.states->length;
        for (j = 0; j < ll2; j++)
        {
            accu += *fwd++ * *--myPtrFwd;// stfws[cntFwd];
        }

        *outDbl++ = accu;
        ll1 = handle->lowmemram.states->length - handle->lowmemram.states->idxRead;
        ll1 = JVX_MIN(ll1, handle->constants.downsamplingFactor);
        ll2 = handle->constants.downsamplingFactor - ll1;

        myPtrFwd = stfws + handle->lowmemram.states->idxRead;

        for (j = 0; j < ll1; j++)
        {
            //*myPtrFwd++ = *inDbl++;
            tmpI16 = *inInt16++;
            JVX_INT16_DATA_NORM(tmpI16, tmpID);
            *myPtrFwd++ = tmpID;
        }
        myPtrFwd = stfws;
        for (j = 0; j < ll2; j++)
        {
            //*myPtrFwd++ = *inDbl++;
            tmpI16 = *inInt16++;
            JVX_INT16_DATA_NORM(tmpI16, tmpID);
            *myPtrFwd++ = tmpID;
        }
        handle->lowmemram.states->idxRead = (handle->lowmemram.states->idxRead + handle->constants.downsamplingFactor);
        handle->lowmemram.states->idxRead &= lm1;
    }

    return(JVX_DSP_NO_ERROR);
}

#endif

