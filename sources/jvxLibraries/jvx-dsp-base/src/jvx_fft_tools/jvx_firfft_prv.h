#ifndef __JVX_FIRFFT_PRV_H__
#define __JVX_FIRFFT_PRV_H__

typedef struct
{
	struct
	{
		jvxFFT* corefft;
		jvxIFFT* coreifft;
		jvxDataCplx* spec;
		jvxData* out;
		jvxData* in;
		jvxFFTGlobal* fftGlob;
		jvxSize phase;
		jvxSize outoffset;
		jvxData normFactor;
	} ram;

	jvx_firfft_prmInit init_cpy;
	jvx_firfft_prmDerived derived_cpy;
} jvx_firfft_prv;

void jvx_firfft_resetDerived(jvx_firfft* init);

#endif
