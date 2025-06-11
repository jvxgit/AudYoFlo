#include "jvx.h"
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
#include "jvxSpNSpeakerEqualizer.h"

const jvxModuleOnStart componentsOnLoad_anodes_1dev[] =
{
	{"jvxAuNPlayChannelId"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_spnodes_1dev[] =
{
	{"jvxSpNMeasureIr"},
	{"jvxSpNSpeakerEqualizer"},
	NULL
};

// ========================================================================
const jvxModuleOnStart componentsOnLoad_anodes_2dev[] =
{
	{"jvxAuNMeasureIr2Dev"},
	{"jvxAuNPlayChannelId"},
	NULL
};


const jvxModuleOnStart componentsOnLoad_spnodes_2dev[] =
{
	{"jvxSpNAsyncIo_sm"},
	{"jvxSpNMeasureIr"},
	{"jvxSpNSpeakerEqualizer"},
	NULL
};

extern "C"
{
	jvxErrorType jvx_access_link_object_specialization(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm,
		jvxApiString* adescr)
	{
		adescr->assign("SpeakerEqualizer");
		*funcInit = jvxSpNSpeakerEqualizer_init;
		*funcTerm = jvxSpNSpeakerEqualizer_terminate;
		return(JVX_NO_ERROR);
	};

	jvxErrorType jvx_configure_factoryhost_features_specialization(configureQtAudioHost_features* cfgFeat, jvxBool is1dev)
	{
		if (is1dev)
		{
			cfgFeat->numSlotsComponents[JVX_COMPONENT_AUDIO_NODE] = 1;
			cfgFeat->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = 2;
			cfgFeat->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_anodes_1dev;
			cfgFeat->lst_ModulesOnStart[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = componentsOnLoad_spnodes_1dev;
		}
		else
		{
			cfgFeat->numSlotsComponents[JVX_COMPONENT_AUDIO_NODE] = 2;
			cfgFeat->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = 3;
			cfgFeat->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_anodes_2dev;
			cfgFeat->lst_ModulesOnStart[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = componentsOnLoad_spnodes_2dev;
		}
		return(JVX_NO_ERROR);
	};

	jvxErrorType jvx_default_connection_rules_add_specialization(jvxComponentIdentification* tpEnque, jvxBool is1dev)
	{
		if (is1dev)
		{
			*tpEnque = jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0);
		}
		else
		{
			*tpEnque = jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 2, 0);
		}
		return(JVX_NO_ERROR);
	};
}

