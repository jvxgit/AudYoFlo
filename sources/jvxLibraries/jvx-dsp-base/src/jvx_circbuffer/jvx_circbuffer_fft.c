#include "jvx_dsp_base.h"
#include "jvx_circbuffer/jvx_circbuffer_fftifft.h"

struct jvxFftIfftHandle
{
	jvxFFT** ffts;
	jvxIFFT** iffts;
	jvxSize num;
};

static struct jvxFftIfftHandle* allocateFftIfftExtension(jvxSize nChannels)
{
	struct jvxFftIfftHandle* lochdl = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(lochdl, struct jvxFftIfftHandle);
	lochdl->num = nChannels;
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(lochdl->ffts, jvxFFT*, lochdl->num);
	JVX_DSP_SAFE_ALLOCATE_FIELD_Z(lochdl->iffts, jvxIFFT*, lochdl->num);
	return lochdl;
}

static void deallocateFftIfftExtension(struct jvxFftIfftHandle** lochdl)
{
	if (lochdl && *lochdl)
	{
		JVX_DSP_SAFE_DELETE_FIELD((*lochdl)->ffts);
		JVX_DSP_SAFE_DELETE_FIELD((*lochdl)->iffts);
		JVX_DSP_SAFE_DELETE_OBJECT(*lochdl);
		*lochdl = NULL;
	}
}

jvxDspBaseErrorType
jvx_circbuffer_allocate_global_fft_ifft(jvxFFTGlobal** global_fft, jvxFFTSize fftType_max, jvxHandle* fftGlobalCfg)
{
	return jvx_create_fft_ifft_global(global_fft, fftType_max, fftGlobalCfg);
}

jvxDspBaseErrorType
jvx_circbuffer_destroy_global_fft_ifft(jvxFFTGlobal* global_fft)
{
	return jvx_destroy_fft_ifft_global(global_fft);
}

jvxDspBaseErrorType
jvx_circbuffer_allocate_fft_ifft(jvx_circbuffer_fft** hdlOnReturn, jvxFFTGlobal* hdl_global_fft,
				jvxFftTools_coreFftType fftType, jvxFFTSize fftSize, jvxCBool preserveInput, jvxSize nChannels)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxInt32 numDummy = 0;
	jvxData* buf = NULL;
	jvxSize N = 0;
	if(hdlOnReturn)
	{
		*hdlOnReturn = NULL;
		switch(fftType)
		{
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
			res =  JVX_DSP_NO_ERROR;
			break;
		default:
			res = JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		if(res == JVX_DSP_NO_ERROR)
		{
			jvxSize i;
			jvx_circbuffer_fft* hdl;
			JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(hdl, jvx_circbuffer_fft);
			hdl->circBuffer.channels = nChannels;
			hdl->circBuffer.format = JVX_DATAFORMAT_DATA;
			hdl->circBuffer.idxRead = 0;
			hdl->circBuffer.fHeight = 0;
			hdl->circBuffer.nUnits = 1;
			hdl->circBuffer.szElement = jvxDataFormat_size[JVX_DATAFORMAT_DATA];

			hdl->fftSize = fftSize;
			hdl->fftType = fftType;
			hdl->fftifft = allocateFftIfftExtension(hdl->circBuffer.channels);
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl->circBuffer.ram.field, jvxData*, hdl->circBuffer.channels);
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl->ram.field_cplx, jvxDataCplx*, hdl->circBuffer.channels);

			switch(hdl->fftType)
			{
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:

				for (i = 0; i < hdl->circBuffer.channels; i++)
				{
					if (preserveInput)
					{
						jvx_create_fft_real_2_complex(&hdl->fftifft->ffts[i], hdl_global_fft, hdl->fftSize,
							&buf, &hdl->ram.field_cplx[i], &N, JVX_FFT_IFFT_PRESERVE_INPUT,
							NULL, NULL, 0);
					}
					else
					{
						jvx_create_fft_real_2_complex(&hdl->fftifft->ffts[i], hdl_global_fft, hdl->fftSize,
							&buf, &hdl->ram.field_cplx[i], &N, JVX_FFT_IFFT_EFFICIENT,
							NULL, NULL, 0);
					}
					hdl->circBuffer.ram.field[i] = buf;
				}
				hdl->circBuffer.length = N;
				hdl->length_cplx = N/2 + 1;
				
				break;
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:

				for (i = 0; i < hdl->circBuffer.channels; i++)
				{
					if (preserveInput)
					{
						jvx_create_ifft_complex_2_real(&hdl->fftifft->iffts[i], hdl_global_fft, hdl->fftSize,
							&hdl->ram.field_cplx[i], &buf, &N,
							JVX_FFT_IFFT_PRESERVE_INPUT, NULL, NULL, 0);
					}
					else
					{
						jvx_create_ifft_complex_2_real(&hdl->fftifft->iffts[i], hdl_global_fft, hdl->fftSize,
							&hdl->ram.field_cplx[i], &buf, &N,
							JVX_FFT_IFFT_EFFICIENT, NULL, NULL, 0);
					}
					hdl->circBuffer.ram.field[i] = buf;
				}
				hdl->circBuffer.length = N;
				hdl->length_cplx = N/2 + 1;

				break;
			default:
			  assert(0);
			}
			*hdlOnReturn = hdl;
		}
		return res;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}


jvxDspBaseErrorType
jvx_circbuffer_allocate_fft_ifft_extbuf(jvx_circbuffer_fft** hdlOnReturn, jvxFFTGlobal* hdl_global_fft,
				jvxFftTools_coreFftType fftType, jvxFFTSize fftSize, jvxCBool preserveInput, void* extBuffer1, void* extBuffer2)
{
	jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;
	jvxInt32 numDummy = 0;
	jvxData* buf = NULL;
	jvxSize N = 0;
	if(hdlOnReturn)
	{
		*hdlOnReturn = NULL;
		switch(fftType)
		{
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
			res =  JVX_DSP_NO_ERROR;
			break;
		default:
			res = JVX_DSP_ERROR_INVALID_ARGUMENT;
		}

		if(res == JVX_DSP_NO_ERROR)
		{
			jvxSize i;
            jvx_circbuffer_fft* hdl;
            JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(hdl, jvx_circbuffer_fft);
			hdl->circBuffer.channels = 1;
			hdl->circBuffer.format = JVX_DATAFORMAT_DATA;
			hdl->circBuffer.idxRead = 0;
			hdl->circBuffer.fHeight = 0;
			hdl->circBuffer.nUnits = 1;
			hdl->circBuffer.szElement = jvxDataFormat_size[JVX_DATAFORMAT_DATA];

			hdl->fftSize = fftSize;
			hdl->fftType = fftType;
			hdl->fftifft = allocateFftIfftExtension(hdl->circBuffer.channels);
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl->circBuffer.ram.field, jvxData*, hdl->circBuffer.channels);
			JVX_DSP_SAFE_ALLOCATE_FIELD_Z(hdl->ram.field_cplx, jvxDataCplx*, hdl->circBuffer.channels);

			switch(hdl->fftType)
			{
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:

				for (i = 0; i < hdl->circBuffer.channels; i++)
				{
					if (preserveInput)
					{
						jvx_create_fft_real_2_complex(&hdl->fftifft->ffts[i], hdl_global_fft, hdl->fftSize,
							&buf, &hdl->ram.field_cplx[i], &N, JVX_FFT_IFFT_PRESERVE_INPUT,
							(jvxData*)extBuffer1, (jvxDataCplx*)extBuffer2, 0);
					}
					else
					{
						jvx_create_fft_real_2_complex(&hdl->fftifft->ffts[i], hdl_global_fft, hdl->fftSize,
							&buf, &hdl->ram.field_cplx[i], &N, JVX_FFT_IFFT_EFFICIENT,
							(jvxData*)extBuffer1, (jvxDataCplx*)extBuffer2, 0);
					}
					hdl->circBuffer.ram.field[i] = buf;
				}	
				hdl->circBuffer.length = N;
				hdl->length_cplx = N/2 + 1;
				break;
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:

				for (i = 0; i < hdl->circBuffer.channels; i++)
				{
					if (preserveInput)
					{
						jvx_create_ifft_complex_2_real(&hdl->fftifft->iffts[i], hdl_global_fft, hdl->fftSize,
							&hdl->ram.field_cplx[i], &buf, &N,
							JVX_FFT_IFFT_PRESERVE_INPUT, (jvxDataCplx*)extBuffer1, (jvxData*)extBuffer2, 0);
					}
					else
					{
						jvx_create_ifft_complex_2_real(&hdl->fftifft->iffts[i], hdl_global_fft, hdl->fftSize,
							&hdl->ram.field_cplx[i], &buf, &N,
							JVX_FFT_IFFT_EFFICIENT, (jvxDataCplx*)extBuffer1, (jvxData*)extBuffer2, 0);
					}
					hdl->circBuffer.ram.field[i] = buf;
				}
				hdl->circBuffer.length = N;
				hdl->length_cplx = N/2 + 1;

				break;
			default:
			  assert(0);
			}
			*hdlOnReturn = hdl;
		}
		return res;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_access_cplx_fft_ifft(jvx_circbuffer_fft* hdl, jvxDataCplx** outPtr, jvxSize* ll, jvxSize idx)
{
	if(hdl)
	{
		if (idx < hdl->circBuffer.channels)
		{
			if (outPtr)
			{
				*outPtr = hdl->ram.field_cplx[idx];
			}
			if (ll)
			{
				*ll = hdl->length_cplx;
			}
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_deallocate_fft_ifft(jvx_circbuffer_fft* hdl)
{
	if(hdl)
	{
		jvxSize i;
		for (i = 0; i < hdl->circBuffer.channels; i++)
		{
			switch (hdl->fftType)
			{
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
				jvx_destroy_fft(hdl->fftifft->ffts[i]);
				hdl->fftifft->ffts[i] = NULL;
				break;
			case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
				jvx_destroy_ifft(hdl->fftifft->iffts[i]);
				hdl->fftifft->iffts[i] = NULL;
				break;
			default:
				assert(0);
			}
			hdl->circBuffer.ram.field[i] = NULL;
		}
		
		JVX_DSP_SAFE_DELETE_FIELD(hdl->circBuffer.ram.field);
		hdl->circBuffer.ram.field = NULL;
		hdl->circBuffer.length = 0;

		JVX_DSP_SAFE_DELETE_FIELD(hdl->ram.field_cplx);
		hdl->ram.field_cplx = NULL;
		hdl->length_cplx = 0;

		deallocateFftIfftExtension(&hdl->fftifft);

		hdl->circBuffer.channels = 0;
		hdl->circBuffer.format = JVX_DATAFORMAT_NONE;
		hdl->circBuffer.idxRead = 0;
		hdl->circBuffer.fHeight = 0;
		hdl->circBuffer.szElement = 0;
		JVX_DSP_SAFE_DELETE_OBJECT(hdl);
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_deallocate_fft_ifft_extbuf(jvx_circbuffer_fft* hdl)
{
	return jvx_circbuffer_deallocate_fft_ifft(hdl);
}

jvxDspBaseErrorType
jvx_circbuffer_get_read_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize* phase)
{
	if(hdl)
	{
		*phase = hdl->circBuffer.idxRead;
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_set_read_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize phase)
{
	if(hdl)
	{
		if(phase < hdl->circBuffer.length)
		{
			hdl->circBuffer.idxRead = phase;
			return JVX_DSP_NO_ERROR;
		}
		return JVX_DSP_ERROR_INVALID_ARGUMENT;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_get_write_phase_fft_ifft(jvx_circbuffer_fft* hdl, jvxSize* phase)
{
	if(hdl)
	{
		return jvx_circbuffer_get_write_phase(&hdl->circBuffer, phase);		

		/*
		*phase = (hdl->circBuffer.idxRead + hdl->circBuffer.fHeight) % hdl->circBuffer.length;
		assert(*phase < hdl->circBuffer.length);
		*/
		// return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}

jvxDspBaseErrorType
jvx_circbuffer_process_fft_ifft(jvx_circbuffer_fft* hdl)
{
	if(hdl)
	{
		jvxSize i;
		switch(hdl->fftType)
		{
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX:
			for (i = 0; i < hdl->circBuffer.channels; i++)
			{
				jvx_execute_fft(hdl->fftifft->ffts[i]);
			}
			hdl->circBuffer.idxRead = (hdl->circBuffer.idxRead + hdl->circBuffer.fHeight)%hdl->circBuffer.length;
			hdl->circBuffer.fHeight = 0;
			break;
		case JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL:
			for (i = 0; i < hdl->circBuffer.channels; i++)
			{
				jvx_execute_ifft(hdl->fftifft->iffts[i]);
			}
			hdl->circBuffer.fHeight = hdl->circBuffer.length;
			break;
		default:
		  assert(0);
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_INVALID_ARGUMENT;
}
