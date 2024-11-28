#include "jvx.h"

// #include "jvxSpNMeasureIr.h"

#include "jvxAuNMeasureIr2Dev.h"
#include "jvxSpNAsyncIo_sm.h"
#include "jvxSpTDeploySignal.h"
#include "jvxSpNMeasureIr.h"
#include "jvxSpNSpeakerEqualizer.h"

#include "jvxAuTPortAudio.h"

#include "jvxAuTNetwork_master.h"
#include "jvxAuTNetwork_slave.h"

#ifdef JVX_OS_WINDOWS
#include "jvxAuTAsio.h"
#endif

#ifdef JVX_OS_LINUX
#include "jvxAuTAlsa.h"
#endif

#ifdef JVX_OS_MACOSX
#include "jvxAuTCoreAudio.h"
#endif

#ifdef JVX_USE_PORTAUDIO
#include "jvxAuTPortAudio.h"
#endif

#ifdef JVX_USE_WASAPI
#include "jvxAuTWindows.h"
#endif

// #include "jvxSpNHoaEnc.h"
//#include "jvxSpNHoaLocate.h"
#include "jvxAuNPlayChannelId.h"
#ifdef JVX_OS_WINDOWS
#include "jvxTMSWText2Speech.h"
#endif

#include "jvxTDataLogger.h"
#include "jvxTLocalTextLog.h"

/*
Configuration control flow:

ASYNC IO: (jvxSpNAsyncIo_sm - SECONDARY MASTER)
- Primary Chain:
	1) Output/input channels opposite to channels in primary chain
	2) All other parameters: Secondary chain controls primary chain
- Secondary chain:
	1) Input/output channels variable
	2) All other parameters variable: set constraints for primary chain

AUNTASK; (PRIMARY MASTER)
- Primary Chain:
	1) Input / output channels variable
	2) All other parameters: variable: set constraint for secondary chain
- Secondary Chain:
	1) Input / output channels variable
	2) All other parameters variable: Primary chain controls secondary chain
 */

extern "C"
{
	// open function - to be provided by the application
	extern jvxErrorType jvx_access_link_object_specialization(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm,
		jvxApiString* adescr);
};

extern "C"
{
jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, 
	jvxApiString* adescr, jvxComponentType tp, jvxSize id)
{
	jvxSize cnt = 0;
	switch (tp)
	{
	case JVX_COMPONENT_AUDIO_TECHNOLOGY:
		
#ifdef JVX_USE_PORTAUDIO
		if (id == cnt)
		{
			adescr->assign("PortAudio Audio");
			*funcInit = jvxAuTPortAudio_init;
			*funcTerm = jvxAuTPortAudio_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;
#endif
#if defined JVX_USE_WASAPI

		if (id == cnt)
		{
			adescr->assign("Windows Audio");
			*funcInit = jvxAuTWindows_init;
			*funcTerm = jvxAuTWindows_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;

#elif defined JVX_USE_ALSA
		if (id == cnt)
		{
			adescr->assign("Alsa Audio");
			*funcInit = jvxAuTAlsa_init;
			*funcTerm = jvxAuTAlsa_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;
#elif defined JVX_USE_COREAUDIO

		if (id == cnt)
		{
			adescr->assign("Core Audio");
			*funcInit = jvxAuTCoreAudio_init;
			*funcTerm = jvxAuTCoreAudio_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;
#endif

#if defined JVX_USE_ASIO
		if (id == cnt)
		{
			adescr->assign("Asio Audio");
			*funcInit = jvxAuTAsio_init;
			*funcTerm = jvxAuTAsio_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;
#endif

		if (id == cnt)
		{
			adescr->assign("Network Master Audio");
			*funcInit = jvxAuTNetwork_master_init;
			*funcTerm = jvxAuTNetwork_master_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;

		if (id == cnt)
		{
			adescr->assign("Network Slave Audio");
			*funcInit = jvxAuTNetwork_slave_init;
			*funcTerm = jvxAuTNetwork_slave_terminate;
			return(JVX_NO_ERROR);
		}
		cnt++;
		break;
	case JVX_COMPONENT_AUDIO_NODE:
		switch (id)
		{
		case 0:
			adescr->assign("Measure 2 Dev");
			*funcInit = jvxAuNMeasureIr2Dev_init;
			*funcTerm = jvxAuNMeasureIr2Dev_terminate;
			return(JVX_NO_ERROR);

		case 1:
			adescr->assign("Play Channel");
			*funcInit = jvxAuNPlayChannelId_init;
			*funcTerm = jvxAuNPlayChannelId_terminate;
			return(JVX_NO_ERROR);

		default:
			break;
		}
		break;

#if defined(JVX_OS_WINDOWS) && !define(JVX_WINDOWS_EXCLUDE_ATL_PROJECTS) 
	case JVX_COMPONENT_TEXT2SPEECH:
		switch (id)
		{
		case 0:
			adescr->assign("MSW Text 2 Speech");
			*funcInit = jvxTMswText2Speech_init;
			*funcTerm = jvxTMswText2Speech_terminate;
			return(JVX_NO_ERROR);

		default:
			break;
		}
		break;
#endif
	case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:
		switch (id)
		{
		case 0:
			adescr->assign("Async I/O Node");
			*funcInit = jvxSpNAsyncIo_sm_init;
			*funcTerm = jvxSpNAsyncIo_sm_terminate;
			return(JVX_NO_ERROR);

		case 1:

			adescr->assign("MeasureIr");
			*funcInit = jvxSpNMeasureIr_init;
			*funcTerm = jvxSpNMeasureIr_terminate;
			return(JVX_NO_ERROR);

		case 2:

			return jvx_access_link_object_specialization(funcInit, funcTerm, adescr);
		default:
			break;
		}
		break;

	case JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY:
		switch (id)
		{
		case 0:
			adescr->assign("Deploy Signal Technology");
			*funcInit = jvxSpTDeploySignal_init;
			*funcTerm = jvxSpTDeploySignal_terminate;
			return(JVX_NO_ERROR);

		default:
			break;
		}
		break;

#ifdef JVX_NTASK_WITH_DATALOGGER
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
#endif

	case JVX_COMPONENT_LOCAL_TEXT_LOG:
		switch (id)
		{
		case 0:

			adescr->assign("Local Text Log");
			*funcInit = jvxTLocalTextLog_init;
			*funcTerm = jvxTLocalTextLog_terminate;
			return(JVX_NO_ERROR);
			break;

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
const char* componentsOnLoad_audiotechnologies[] =
{
	"jvxAuTGenericWrapper",
	nullptr
};

const char* componentsOnLoad_algorithms[] =
{
	"jvxAuNMeasureIr2Dev",
	"jvxAuNPlayChannelId",
	NULL
};


const char* componentsOnLoad_spnodes[] =
{
	"jvxSpNAsyncIo_sm",
	"jvxSpNMeasureIr",
	"jvxSpNSpeakerEqualizer",
	NULL
};

const char* componentsOnLoad_sptechs[] =
{
	"jvxSpTDeploySignal",
	NULL
};

#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
extern "C"
{
	extern jvxErrorType jvx_configure_factoryhost_features_specialization(configureQtAudioHost_features* cfgFeat, jvxBool is1dev);

jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesA = NULL;
	features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesA), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);
	if (theFeaturesA)
	{
		theFeaturesA->numSlotsComponents[JVX_COMPONENT_AUDIO_TECHNOLOGY] = 2;
		theFeaturesA->numSlotsComponents[JVX_COMPONENT_AUDIO_DEVICE] = 2;
		theFeaturesA->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY] = 1;
		theFeaturesA->numSlotsComponents[JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE] = JVX_SIZE_UNSELECTED;

		// The mainCentral widget MUST be of size policy EXPANDING to fill out display completely!!!
		theFeaturesA->config_ui.minWidthWindow = JVX_MAX(theFeaturesA->config_ui.minWidthWindow, 1024);
		theFeaturesA->config_ui.minHeightWindow = JVX_MAX(theFeaturesA->config_ui.minHeightWindow, 800);
		theFeaturesA->config_ui.tweakUi = JVX_QT_MAINWIDGET_EXPAND_FULLSIZE;

		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
		theFeaturesA->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_SIGNAL_PROCESSING_NODE] = true;

		theFeaturesA->lst_ModulesOnStart[JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY] = componentsOnLoad_sptechs;
		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY] = true;
		theFeaturesA->flag_blockModuleEdit[JVX_COMPONENT_SIGNAL_PROCESSING_DEVICE] = true;

		jvx_configure_factoryhost_features_specialization(theFeaturesA, false);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}
