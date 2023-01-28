#ifndef __CJVXAUDIOFFTEQUALIZER_H__
#define __CJVXAUDIOFFTEQUALIZER_H__

#include "jvx.h"

extern "C"
{
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_firfft.h"
};

class CjvxAudioFftEqualizer
{
protected:
	
	jvxSize numChannels;
	jvxSize firEqSize;
	jvxSize firEqFftSize;
	jvxSize delayFir;
	jvxSize delay_max;
	jvxState stat;
	jvxBool enableEq;
	struct
	{
		jvxData** fir_eq;
		jvxData* gainFac;
		jvxSize* delayChan;
		
		jvxBool delayEng;
		jvxBool gainEng;
		jvxBool firEng;
	} proc_data;

	struct
	{
		jvxSize bsize;
		jvx_firfft* theFftFilt;
		jvx_circbuffer** delayline;
		jvxSize* current_delay;
		jvxData* current_gain;
		jvxSize delay_max;
	} proc_variables;

public:
	CjvxAudioFftEqualizer();
	~CjvxAudioFftEqualizer();

	jvxErrorType clear_config();
	jvxErrorType allocate_config(IjvxConfigProcessor* theConfigHdl, jvxConfigData* theLocalSection, std::string& errorDescr, jvxSize delay_max);

	jvxErrorType prepare_eq(jvxSize bsize);
	jvxErrorType process_eq_iplace(jvxData** bufs, jvxSize numChannels, jvxSize bsize);
	jvxErrorType postprocess_eq();

	jvxErrorType number_channels(jvxSize* num);
	jvxErrorType fft_order(jvxSize* order);
	jvxErrorType current_delay(jvxSize idChannel, jvxSize* delay);
	jvxErrorType current_gain(jvxSize idChannel, jvxData* gain);
	jvxErrorType current_fir_eq(jvxSize idChannel, std::string& fir_eq_str);
};

#endif