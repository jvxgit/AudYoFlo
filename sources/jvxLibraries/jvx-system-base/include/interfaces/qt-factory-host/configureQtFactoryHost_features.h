#ifndef CONFIGUREQTFACTORYHOST_FEATURES_H__
#define CONFIGUREQTFACTORYHOST_FEATURES_H__

#include "interfaces/all-hosts/configHostFeatures_common.h"
#include "interfaces/qt/configureQtui_features.h"

class configureQtFactoryHost_features: public configureFactoryHost_features, public configureQtui_features
{
public:
	jvxBool mainWidgetInScrollArea;

	configureQtFactoryHost_features()
	{
		mainWidgetInScrollArea = false;
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_QT_FACTORY_HOST;
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