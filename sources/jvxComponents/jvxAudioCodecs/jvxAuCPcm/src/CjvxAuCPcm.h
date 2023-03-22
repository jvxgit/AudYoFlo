#ifndef __CJVXAUCPCM_H__
#define __CJVXAUCPCM_H__

#include "jvxAudioCodecs/CjvxAudioCodec.h"

class CjvxAuCPcm : public CjvxAudioCodec
{
	friend class CjvxAuCPcmDecoder;
	friend class CjvxAuCPcmEncoder;


public:
	CjvxAuCPcm(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	jvxErrorType request_encoder(IjvxAudioEncoder** encoder_on_return);
	jvxErrorType request_decoder(IjvxAudioDecoder** decoder_on_return);
	virtual void computeRateAndOutBuffersize(CjvxAudioCodec_genpcg* myCfgStrict);

};

#endif