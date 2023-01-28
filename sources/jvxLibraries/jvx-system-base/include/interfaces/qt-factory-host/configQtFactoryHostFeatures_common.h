#ifndef CONFIGUREQTFACTORYHOST_FEATURES_H__
#define CONFIGUREQTFACTORYHOST_FEATURES_H__

#include "interfaces/all-hosts/configHostFeatures_common.h"

class configureQtFactoryHost_features: public configureFactoryHost_features
{
public:
	jvxBool mainWidgetInScrollArea;
	configureQtFactoryHost_features()
	{
		mainWidgetInScrollArea = false;
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_QT_FACTORY_HOST;
	};

	virtual void request_specification(jvxHandle** onRet, jvxHostFeatureType tp)
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