#ifndef COMMON_CONFIG_HOST_FEATURES_H
#define COMMON_CONFIG_HOST_FEATURES_H

#include "jvx.h"

typedef enum
{
	JVX_HOST_SEQUENCER_DEFAULT_UNSPECIFIED
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "extensions/EpjvxHostFeatureTypes.h"
#endif
	, JVX_HOST_SEQUENCER_DEFAULT_CUSTOM
	, JVX_HOST_SEQUENCER_DEFAULT_NONE
	, JVX_HOST_SEQUENCER_DEFAULT_PRODUCT_OFFSET = 256
} jvxHostSequencerDefaultType;

typedef enum
{
	JVX_HOST_IMPLEMENTATION_FACTORY_HOST,
	JVX_HOST_IMPLEMENTATION_QT_FACTORY_HOST,
	JVX_HOST_IMPLEMENTATION_HOST,
	JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST,
	JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST,
	JVX_HOST_IMPLEMENTATION_LIB_HOST
} jvxHostFeatureType;

class configureFactoryHost_features
{
public:
	jvxHostFeatureType hostFeatureTp;
	jvxLoadModuleFilterCallback cb_loadfilter;
	jvxHandle* cb_loadfilter_priv;
	jvxBool allowOverlayOnly_config;
	IjvxFactoryHost* fHost = nullptr;
	struct
	{
		// Automation can be used as follows:
		// 1) <RECOMMENDED> Provide a module for an object of type IjvxAutomation. This object will be requested to 
		// receive pointers for if_report_automate and if_autoconnect. The name of the automation modules must be set
		// in <mod_selection> 
		// 2) Provide the pointers to if_report_automate and if_autoconnect directly. This is not recommended but used in some
		// older projects
		const char* mod_selection = nullptr;
		IjvxReportSystem* if_report_automate = nullptr;
		IjvxAutoDataConnect* if_autoconnect = nullptr;
	} automation;

	configureFactoryHost_features()
	{
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_FACTORY_HOST;
		cb_loadfilter = NULL;
		cb_loadfilter_priv = NULL;
		allowOverlayOnly_config = false;
		automation.if_autoconnect = nullptr;
		automation.if_report_automate = nullptr;

	};
	virtual void request_specialization(jvxHandle** onRet, jvxHostFeatureType tp)
	{
		if (onRet)
		{
			if (tp == hostFeatureTp)
			{
				*onRet = reinterpret_cast<jvxHandle*>(this);
			}
		}
	};
};

class configureHost_features: public configureFactoryHost_features
{
public:

	jvxSize numSlotsComponents[JVX_COMPONENT_ALL_LIMIT];
	jvxSize timeout_period_seq_ms;
	jvxSize granularity_state_report;

	jvxBool flag_blockModuleEdit[JVX_COMPONENT_ALL_LIMIT];
	const char** lst_ModulesOnStart[JVX_COMPONENT_ALL_LIMIT];
	jvxBool verbose_ModulesOnStart = false;

	jvxHandle* prv_features = nullptr;
	IjvxReport* if_report = nullptr;

	IjvxHost* hHost = nullptr;

	configureHost_features()
	{
		jvxSize i;
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_HOST;

		for (i = 0; i < JVX_MAIN_COMPONENT_LIMIT; i++)
		{
			numSlotsComponents[i] = 0;
			lst_ModulesOnStart[i] = NULL;
			flag_blockModuleEdit[i] = false;
		}
		for (i = JVX_MAIN_COMPONENT_LIMIT; i < JVX_COMPONENT_ALL_LIMIT; i++)
		{
			numSlotsComponents[i] = 0;
			lst_ModulesOnStart[i] = NULL;
			flag_blockModuleEdit[i] = true;
		}
		timeout_period_seq_ms = 200;
		granularity_state_report = 10; // every 10 steps = 2 seconds 
		if_report = nullptr;
		prv_features = nullptr;
		verbose_ModulesOnStart = false;
	};

	virtual void request_specialization(jvxHandle** onRet, jvxHostFeatureType tp)override
	{
		if (onRet)
		{
			if (tp == hostFeatureTp)
			{
				*onRet = reinterpret_cast<jvxHandle*>(this);
			}
		}
	};	
};

class configureLibHost_features : public configureHost_features
{
public:
	jvxBool host_sync = false;

	configureLibHost_features()
	{
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_LIB_HOST;
		host_sync = false;
	}

	virtual void request_specialization(jvxHandle** onRet, jvxHostFeatureType tp)override
	{
		if (onRet)
		{
			if (tp == hostFeatureTp)
			{
				*onRet = reinterpret_cast<jvxHandle*>(this);
			}
		}
	};
};

#endif