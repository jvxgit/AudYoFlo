#include "typedefs/config-host/configHostFeatures_common.h"

configureFactoryHost_features::configureFactoryHost_features()
{
	hostFeatureTp = JVX_HOST_IMPLEMENTATION_FACTORY_HOST;
	cb_loadfilter = NULL;
	cb_loadfilter_priv = NULL;
	allowOverlayOnly_config = false;
}

void 
configureFactoryHost_features::request_specification(jvxHandle** onRet, jvxHostFeatureType tp)
{
	if (onRet)
	{
		if (tp == hostFeatureTp)
		{
			*onRet = reinterpret_cast<jvxHandle*>(this);
		}
	}
}

configureHost_features::configureHost_features()
{
	jvxSize i;
	defSeqInit = JVX_HOST_SEQUENCER_DEFAULT_AUDIO_VIDEO;
	hostFeatureTp = JVX_HOST_IMPLEMENTATION_HOST;
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		numSlotsComponents[i] = 0;
	}
	init_defaultSteps_run = NULL;
	init_numSteps_run = 0;
	init_defaultSteps_leave = NULL;
	init_numSteps_leave = 0;
	timeout_period_seq_ms = 200;

}

void
configureHost_features::request_specification(jvxHandle** onRet, jvxHostFeatureType tp)
{
	if (onRet)
	{
		if (tp == hostFeatureTp)
		{
			*onRet = reinterpret_cast<jvxHandle*>(this);
		}
	}
}