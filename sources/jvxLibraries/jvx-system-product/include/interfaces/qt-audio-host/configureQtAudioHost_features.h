#ifndef _CONFIGUREAUDIOHOST_FEATURES_H__
#define _CONFIGUREAUDIOHOST_FEATURES_H__

#include "interfaces/all-hosts/configHostFeatures_common.h"
#include "interfaces/qt/mainWindow_UiExtension.h"
#include "interfaces/qt/configureQtui_features.h"

class configureQtAudioHost_features: public configureHost_features, public configureQtui_features
{
public:

	jvxBool hideHostStartButton;
	jvxBool disableSaveFunction;
	//jvxBool activateDefaultAlgorithm;
	//const char* nameDefaultAlgorithm;
	mainWindow_UiExtension_host* myMainWidget;
	jvxBool includesVideo;

	configureQtAudioHost_features()
	{
		hostFeatureTp = JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST;

		includesVideo = false;
		hideHostStartButton = false;
		disableSaveFunction = false;
		myMainWidget = NULL;
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