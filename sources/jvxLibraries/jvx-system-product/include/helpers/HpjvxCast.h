#ifndef __HPJVXCAST_H__
#define __HPJVXCAST_H__

// Additional component types
#define JVX_COMPONENT_CAST_PRODUCT \
	case JVX_COMPONENT_AUDIO_CODEC: \
		if (std::is_same<T, IjvxAudioCodec>::value) \
		{ \
			result = true; \
		} \
		break; \
	case JVX_COMPONENT_RT_AUDIO_FILE_READER: \
		if (std::is_same<T, IjvxRtAudioFileReader>::value) \
		{ \
			result = true; \
		} \
		break; \
	case JVX_COMPONENT_RT_AUDIO_FILE_WRITER: \
		if (std::is_same<T, IjvxRtAudioFileReader>::value) \
		{ \
			result = true; \
		} \
		break; \
	case JVX_COMPONENT_RESAMPLER: \
		if (std::is_same<T, IjvxResampler>::value) \
		{ \
			result = true; \
		} \
		break; \
	case  JVX_COMPONENT_DATACONVERTER: \
		if (std::is_same<T, IjvxDataConverter>::value) \
		{ \
			result = true; \
		} \
		break; \
	case JVX_COMPONENT_AUDIO_DECODER: \
	case JVX_COMPONENT_AUDIO_ENCODER: \
		if (std::is_same<T, IjvxSimpleNode>::value) \
		{ \
			result = true; \
		} \
		break; \

#define JVX_COMPONENT_CASE_DEVICE 
#define JVX_COMPONENT_CASE_TECHNOLOGY 
#define JVX_COMPONENT_CASE_NODE 
	
#define JVX_PROPERTY_EXTENDER_CAST_CASES \
	else if (std::is_same < T, IjvxPropertyExtenderChainControl>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
		jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_CHAIN_CONTROL); \
	} \
	else if (std::is_same < T, IjvxPropertyExtenderStreamAudioPlugin>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
		jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_PROPERTY_STREAM_AUDIO_PLUGIN); \
	} \
	else if (std::is_same < T, IjvxDirectMixerControl>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
		jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_DIRECT_MIXER_CONTROL); \
	} \
	else if (std::is_same < T, IjvxPropertyExtenderHrtfDispenser>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
		jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER); \
	} \
	else if (std::is_same < T, IjvxPropertyExtenderHeadTrackerProvider>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
			jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HEADTRACKER_PROVIDER); \
	} \
	else if (std::is_same < T, IjvxPropertyExtenderNftfProvider>::value) \
	{ \
		pExt->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&ptrRet), \
			jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_NFTF_PROVIDER); \
	}

#endif
