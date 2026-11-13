#include "jvx.h"

#include "jvxAuNAnalyze.h"
#include "jvxTDataLogger.h"

#define JVX_ALGO_DESCRIPTION "Analyze Audio"
#define JVX_ALGO_INIT_FUNC jvxAuNAnalyze_init
#define JVX_ALGO_TERMINATE_FUNC jvxAuNAnalyze_terminate

extern "C"
{
	jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id)
	{
		switch (tp)
		{
		case JVX_COMPONENT_AUDIO_NODE:
			switch (id)
			{
			case 0:
				adescr->assign(JVX_ALGO_DESCRIPTION);
				*funcInit = JVX_ALGO_INIT_FUNC;
				*funcTerm = JVX_ALGO_TERMINATE_FUNC;
				return(JVX_NO_ERROR);

			default:
				break;
			}
			break;
		case JVX_COMPONENT_DATALOGGER:
			switch (id)
			{
			case 0:

				adescr->assign("Data Logger");
				*funcInit = jvxTDataLogger_init;
				*funcTerm = jvxTDataLogger_terminate;
				return(JVX_NO_ERROR);
			default:
				break;
			}
			break;
		default:
			break;
		}
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
}

// =============================================================
// Configure ui extensions
// =============================================================
#include "CjvxQtComponentWidget_socket_master_dev.h"
#include "CjvxQtComponentWidget_socket_slave_dev.h"

#include "CjvxQtComponentWidget_socket_tech.h"

// =============================================================
// Configure ui extensions
// =============================================================
jvxErrorType
IjvxQtComponentWidget_init(IjvxQtComponentWidget** onReturn, jvxBitField featureClass, const jvxComponentIdentification& cpId)
{
	CjvxQtComponentWidget_socket_master_dev* newElmMDev = NULL;
	CjvxQtComponentWidget_socket_slave_dev* newElmSDev = NULL;
	CjvxQtComponentWidget_socket_tech* newElmTech = NULL;
	switch (cpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:
		if (JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER))
		{
			newElmMDev = new CjvxQtComponentWidget_socket_master_dev("VERBOSE=yes,RTUPDATE=yes");
			if (onReturn)
				*onReturn = static_cast<IjvxQtComponentWidget*>(newElmMDev);
			return JVX_NO_ERROR;
		}
		else if (JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE))
		{
			newElmSDev = new CjvxQtComponentWidget_socket_slave_dev("VERBOSE=yes,RTUPDATE=yes");
			if (onReturn)
				*onReturn = static_cast<IjvxQtComponentWidget*>(newElmSDev);
			return JVX_NO_ERROR;
		}
		break;

	case JVX_COMPONENT_AUDIO_TECHNOLOGY:

		/* Shared UI */
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			newElmTech = new CjvxQtComponentWidget_socket_tech("VERBOSE=yes,RTUPDATE=yes");

		}

		if (onReturn)
			*onReturn = static_cast<IjvxQtComponentWidget*>(newElmTech);
		return JVX_NO_ERROR;

		break;
	}
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate(IjvxQtComponentWidget* returnMe, jvxBitField featureClass,
	const jvxComponentIdentification& cpId)
{
	switch (cpId.tp)
	{
	case JVX_COMPONENT_AUDIO_DEVICE:
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			delete(returnMe);
		}
		return JVX_NO_ERROR;
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		if ((JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_MASTER) ||
			JVX_EVALUATE_BITFIELD(featureClass & JVX_FEATURE_CLASS_COMPONENT_SOCKET_SLAVE)))
		{
			delete(returnMe);
		}
		return JVX_NO_ERROR;
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

// =======================================================================


#include "myCentralWidget.h"

// =============================================================
// Main widget entry functions
// =============================================================
jvxErrorType
mainWindow_UiExtension_host_init(mainWindow_UiExtension_host** onReturn, QWidget* parent)
{
#ifdef JVX_PROJECT_NAMESPACE
	JVX_PROJECT_NAMESPACE::myCentralWidget* elm = new JVX_PROJECT_NAMESPACE::myCentralWidget(parent);
#else
	myCentralWidget* elm = new myCentralWidget(parent);
#endif
	if (onReturn)
	{
		*onReturn = static_cast<mainWindow_UiExtension_host*>(elm);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mainWindow_UiExtension_host_terminate(mainWindow_UiExtension_host* elm)
{
	delete(elm);
	return JVX_NO_ERROR;
}

// =============================================================
// Specify some runtime host configurations
// =============================================================
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
#include <QApplication>
#include <QScreen>

void
myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv)
{
	*doNotLoad = true;
	switch (tp)
	{
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		*doNotLoad = false;
	}
}

const jvxModuleOnStart componentsOnLoad_algorithms[] =
{
	{"jvxAuNAnalyze"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_audiotechnologies[] =
{
	{"jvxAuTGenericWrapper"},
	nullptr
};

extern "C"
{
	jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesA = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesA), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesA) 
	{
		theFeaturesA->cb_loadfilter = myComponentFilterCallback;
		theFeaturesA->cb_loadfilter_priv = NULL;

		theFeaturesA->timeout_viewer_maincentral_ms = 50;		

#ifdef JVX_PROJECT_NAMESPACE
		JVX_PROJECT_NAMESPACE::myCentralWidget* theWidget = dynamic_cast<JVX_PROJECT_NAMESPACE::myCentralWidget*>(theFeaturesA->myMainWidget);
#else
		myCentralWidget* theWidget = dynamic_cast<myCentralWidget*>(theFeaturesA->myMainWidget);
#endif

		//theWidget->preferredSize(theFeaturesA->minHeightWindow, theFeaturesA->minWidthWindow);
		
		theFeaturesA->config_ui.minWidthWindow = JVX_MAX(theFeaturesA->config_ui.minWidthWindow, 256);
		theFeaturesA->config_ui.minHeightWindow = JVX_MAX(theFeaturesA->config_ui.minHeightWindow, 256);
		theFeaturesA->config_ui.tweakUi = JVX_QT_MAINWIDGET_EXPAND_FULLSIZE;	

		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
		theFeaturesA->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
		theFeaturesA->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

	}
	/*
		rec = lstS.at(0)->availableGeometry();
		theFeatures->minWidthWindow = rec.width()/2;
		theFeatures->minHeightWindow = 2*rec.height()/3;
		//theFeatures->maxWidthWindow = rec.width()/2;
		//theFeatures->maxHeightWindow = 2*rec.height()/3;
	}*/
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}


