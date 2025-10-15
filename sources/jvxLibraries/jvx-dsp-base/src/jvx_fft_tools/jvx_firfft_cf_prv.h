#ifndef __JVX_FIRFFT_CF_PRV_H__
#define __JVX_FIRFFT_CF_PRV_H__

typedef struct
{
	// This struct contais all variables required for a "simple" firfft: one fft, one ifft
	jvx_firfft_prv firfft;	

	struct
	{
		// Secondary fft for weight update
		jvxFFT* corefft;

		// Input buffer to drive the secondary fft
		jvxData* in_weights;

		// We use 2 iffts to involve old and new weights
		jvxIFFT* coreifft[2];

		// Corresponding spectrum input buffers, should be the result from different weights
		jvxDataCplx* spec_ifft_in[2];

		// The two output buffers
		jvxData* ifft_out[2];

		// Cross fade increment value
		jvxData cf_inc;

		// Index to the OLD weights in case of transition
		jvxSize idx_cf_from;
	} ram_cf;

	jvxCBool allocatedViaAllocator;
} jvx_firfft_cf_prv;
		
#endif
