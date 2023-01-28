#ifndef __CJVXAUDIOFIXEDFFTN1FILTER_H__
#define __CJVXAUDIOFIXEDFFTN1FILTER_H__

#include "jvx.h"

extern "C"
{
#include "jvx_fft_tools/jvx_fft_tools.h"
#include "jvx_fft_tools/jvx_fft_core.h"
#include "jvx_fft_tools/jvx_firfft.h"
};

class CjvxAudioFixedFftN1Filter
{
protected:

	jvxSize numChannelsIn = JVX_SIZE_UNSELECTED;
	jvxSize firSize = JVX_SIZE_UNSELECTED;
	jvxSize firFftSize = JVX_SIZE_UNSELECTED;
	jvxSize delayFir = JVX_SIZE_UNSELECTED;
	jvxState stat = JVX_STATE_INIT;
	struct
	{
		jvxData** fir_in = nullptr;
	} proc_data;

	struct
	{
		jvxSize bsize = 0;
		jvx_firfft* theFftFilt = nullptr;
		jvxData* out_buf = nullptr;
	} proc_variables;

public:
	CjvxAudioFixedFftN1Filter();
	~CjvxAudioFixedFftN1Filter();

	jvxErrorType clear_config();
	jvxErrorType allocate_config(
		IjvxConfigProcessor* theConfigHdl, 
		jvxConfigData* theLocalSection, 
		const std::string& token_scan,
		std::string& errorDescr);

	jvxErrorType prepare_N1Filter(jvxSize bsize);
	jvxErrorType process_N1Filter(jvxData** bufs_in, jvxData* buf_out, jvxSize numChannelsIn, jvxSize bsize);
	jvxErrorType postprocess_N1Filter();

	jvxErrorType number_channels(jvxSize* num);

	jvxErrorType access_fir_hrtf(jvxSize idChannel, jvxData**ptr_fir, jvxSize* len_fir);
	jvxErrorType current_fir_hrtf(jvxSize idChannel, std::string& fir_eq_str);
};

#endif
