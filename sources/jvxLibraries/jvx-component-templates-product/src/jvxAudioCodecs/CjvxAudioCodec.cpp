#include "jvx.h"
#include "jvxAudioCodecs/CjvxAudioCodec.h"
#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "jvxAudioCodecs/CjvxAudioDecoder.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAudioCodec::CjvxAudioCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_CODEC);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxAudioCodec*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	uniqueId = 0;
}

CjvxAudioCodec::~CjvxAudioCodec()
{
}

jvxErrorType 
CjvxAudioCodec::accept_for_decoding(const char* format_token, jvxBool* compr, jvxApiString* astr)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxApiString astrFam;
	jvxApiString astrTp;
	if (jvx_codec_configtoken_2_fam_tp(format_token, astrFam, astrTp) == JVX_NO_ERROR)
	{
		if(astrFam.std_str() == myFamily)
		{
			if (jvx_compareStringsWildcard(myTypeWc, astrTp.std_str()))
			{
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxAudioCodec::provide_for_encoding(const char* format_token_preferred, jvxBool* compr, jvxApiString* astr) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioCodec::supports_format_group(jvxDataFormatGroup query_format)
{
	if (query_format == JVX_DATAFORMAT_GROUP_AUDIO_PCM_INTERLEAVED)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxAudioCodec::request_encoder(IjvxAudioEncoder** encoder_on_return)
{
	if (encoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAudioEncoder* newEncoder = NULL;
			newEncoder = new CjvxAudioEncoder("PCM Audio Encoder", false, "pcm_audio_encoder", 0, "sub_module", JVX_COMPONENT_ACCESS_SUB_COMPONENT, 
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
CjvxAudioCodec::return_encoder(IjvxAudioEncoder* encoder_on_return)
{
	std::vector<IjvxAudioEncoder*>::iterator elm = _common_set_codec.requestedEncoder.begin();
	for (; elm != _common_set_codec.requestedEncoder.end(); elm++)
	{
		if (*elm == encoder_on_return)
		{
			break;
		}
	}
	if (elm == _common_set_codec.requestedEncoder.end())
	{
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
	_common_set_codec.requestedEncoder.erase(elm);
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxAudioCodec::request_decoder(IjvxAudioDecoder** decoder_on_return)
{
	if (decoder_on_return)
	{
		if (_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			CjvxAudioDecoder* newDecoder = NULL;
			newDecoder = new CjvxAudioDecoder(("PCM Audio Decoder (" + jvx_int2String(uniqueId) + ")").c_str(), false, 
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

jvxErrorType 
CjvxAudioCodec::return_decoder(IjvxAudioDecoder* decoder_on_return)
{
	std::vector<IjvxAudioDecoder*>::iterator elm = _common_set_codec.requestedDecoder.begin();
	for (; elm != _common_set_codec.requestedDecoder.end(); elm++)
	{
		if (*elm == decoder_on_return)
		{
			break;
		}
	}
	if (elm == _common_set_codec.requestedDecoder.end())
	{
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
	_common_set_codec.requestedDecoder.erase(elm);
	return(JVX_NO_ERROR);
}

void
CjvxAudioCodec::computeRateAndOutBuffersize(CjvxAudioCodec_genpcg* myCfgStrict)
{
	jvxSize fldOutMax = myCfgStrict->general.buffersize.value *  myCfgStrict->general.num_audio_channels.value * sizeof(jvxInt16);
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

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
