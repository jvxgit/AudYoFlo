#ifndef COMMON_CONFIG_HOST_FEATURES_H
#define COMMON_CONFIG_HOST_FEATURES_H

#include "jvx.h"

typedef enum
{
	JVX_HOST_SEQUENCER_DEFAULT_AUDIO,
	JVX_HOST_SEQUENCER_DEFAULT_AUDIO_VIDEO,
	JVX_HOST_SEQUENCER_DEFAULT_CUSTOM
} jvxHostSequencerDefaultType;

typedef enum
{
	JVX_HOST_IMPLEMENTATION_FACTORY_HOST,
	JVX_HOST_IMPLEMENTATION_QT_FACTORY_HOST,
	JVX_HOST_IMPLEMENTATION_HOST,
	JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST,
	JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST
} jvxHostFeatureType;

class configureFactoryHost_features
{
public:
	jvxHostFeatureType hostFeatureTp;
	jvxLoadModuleFilterCallback cb_loadfilter;
	jvxHandle* cb_loadfilter_priv;
	jvxBool allowOverlayOnly_config;

	configureFactoryHost_features();
	virtual void request_specification(jvxHandle** onRet, jvxHostFeatureType tp);
};

class configureHost_features: public configureFactoryHost_features
{
public:

	jvxHostSequencerDefaultType defSeqInit;
	jvxSize numSlotsComponents[JVX_COMPONENT_ALL_LIMIT];
	jvxOneSequencerStepDefinition* init_defaultSteps_run;
	jvxSize init_numSteps_run;
	jvxOneSequencerStepDefinition* init_defaultSteps_leave;
	jvxSize init_numSteps_leave; 
	jvxSize timeout_period_seq_ms;

	configureHost_features();
	virtual void request_specification(jvxHandle** onRet, jvxHostFeatureType tp)override;	
};

#endif