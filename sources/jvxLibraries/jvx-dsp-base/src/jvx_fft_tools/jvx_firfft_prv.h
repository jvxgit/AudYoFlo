#ifndef __JVX_FIRFFT_PRV_H__
#define __JVX_FIRFFT_PRV_H__

typedef enum
{
	JVX_FIRFFT_PRV_TYPE_NONE,
	JVX_FIRFFT_PRV_TYPE_CF,
	JVX_FIRFFT_PRV_TYPE_CF_NOUT
} jvx_firfft_prv_type;

typedef struct
{
	jvx_firfft_prv_type tp;
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
		jvxCBool normOut;
	} ram;

	jvx_firfft_prmInit init_cpy;
	jvx_firfft_prmDerived derived_cpy;
} jvx_firfft_prv;

void jvx_firfft_resetDerived(jvx_firfft* init);

#endif
