#include "CjvxAuCPcm.h"
#include "CjvxAuCPcmDecoder.h"
#include "CjvxAuCPcmEncoder.h"

CjvxAuCPcm::CjvxAuCPcm(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

jvxErrorType
CjvxAuCPcm::request_encoder(IjvxAudioEncoder** encoder_on_return)
{
	if (encoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAuCPcmEncoder* newEncoder = NULL;
			newEncoder = new CjvxAuCPcmEncoder("PCM Audio Encoder", false, "pcm_audio_encoder", 0, "sub_module", JVX_COMPONENT_ACCESS_SUB_COMPONENT,
				JVX_COMPONENT_AUDIO_ENCODER, "audio_encoder", NULL);
			newEncoder->set_parent(this);
			uniqueId++;
			_common_set_codec.requestedEncoder.push_back(newEncoder);
			*encoder_on_return = newEncoder;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxAuCPcm::request_decoder(IjvxAudioDecoder** decoder_on_return)
{
	if (decoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAuCPcmDecoder* newDecoder = NULL;
			newDecoder = new CjvxAuCPcmDecoder(("PCM Audio Decoder (" + jvx_int2String(uniqueId) + ")").c_str(), false,
				"pcm_audio_decoder", 0, "sub_module", JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DECODER, "audio_decoder",
				NULL);
			newDecoder->set_parent(this);
			uniqueId++;
			_common_set_codec.requestedDecoder.push_back(newDecoder);
			*decoder_on_return = newDecoder;
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_WRONG_STATE);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

void
CjvxAuCPcm::computeRateAndOutBuffersize(CjvxAudioCodec_genpcg* myCfgStrict)
{
	jvxSize fldOutMax = myCfgStrict->general.buffersize.value * myCfgStrict->general.num_audio_channels.value * sizeof(jvxInt16);
	fldOutMax += sizeof(CjvxAudioCodec::pcm_fld_header);

	myCfgStrict->general.max_number_bytes.value = JVX_SIZE_INT32(fldOutMax);

	jvxSize selId = jvx_bitfieldSelection2Id(myCfgStrict->general.mode.value.selection(), myCfgStrict->general.mode.value.entries.size());
	switch (selId)
	{
	case 0:
		fldOutMax = myCfgStrict->general.buffersize.value * myCfgStrict->general.num_audio_channels.value * sizeof(jvxInt16);
		break;
	case 1:
		fldOutMax = myCfgStrict->general.buffersize.value * myCfgStrict->general.num_audio_channels.value * sizeof(jvxInt8);
		break;
	}
	fldOutMax += sizeof(CjvxAudioCodec::pcm_fld_header);
	myCfgStrict->general.bit_rate.value = JVX_DATA2INT32((fldOutMax * myCfgStrict->general.samplerate.value) / (jvxData)myCfgStrict->general.buffersize.value);
}
