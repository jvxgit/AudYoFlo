#ifndef _CONFIGUREDEVELOPHOST_FEATURES_H__
#define _CONFIGUREDEVELOPHOST_FEATURES_H__

#include "interfaces/all-hosts/configHostFeatures_common.h"
#include "interfaces/qt/configureQtui_features.h"

class configureQtDevelopHost_features: public configureHost_features, public configureQtui_features
{
public:
	jvxBool showExpertUi_seq; // Default: true
	jvxBool showExpertUi_props; // Default: true
	jvxBool showExpertUi_plots; // Default: true
	jvxBool showExpertUi_messages; // Default: true
	jvxBool showExpertUi_menuelements; // Default: true

	configureQtDevelopHost_features()
	{
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST;
		showExpertUi_seq = true;
		showExpertUi_props = true;
		showExpertUi_plots = true;
		showExpertUi_messages = true;
		showExpertUi_menuelements = true;
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
	
#endif