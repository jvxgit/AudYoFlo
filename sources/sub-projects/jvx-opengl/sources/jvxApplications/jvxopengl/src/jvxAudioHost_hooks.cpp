#include "jvx.h"
#include "jvxAuNOpenGl.h"

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

#ifdef JVX_HOST_USE_REMOTE_CALL
#include "jvxTRemoteCall.h"
#endif

#ifdef JVX_HOST_USE_CONNECTION
#include "jvxTSocket.h"

#ifdef JVX_RS232_ACTIVE
#ifdef JVX_OS_WINDOWS
#include "jvxTRs232Win32.h"
#endif
#ifdef JVX_OS_Linux
#include "jvxTRs232Glnx.h"
#endif
#endif

#endif

#ifdef JVX_OS_WINDOWS
#include "jvxViTMfOpenGL.h"
#endif

extern "C"
{
	jvxErrorType jvx_access_link_objects(jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* adescr, jvxComponentType tp, jvxSize id)
	{
		std::string descr;
		jvxSize mL;

		switch (tp)
		{
#ifdef JVX_OS_WINDOWS
		case JVX_COMPONENT_VIDEO_TECHNOLOGY:
			switch (id)
			{
			case 0:
				adescr->assign("MS Video");
				*funcInit = jvxViTMfOpenGL_init;
				*funcTerm = jvxViTMfOpenGL_terminate;
				return(JVX_NO_ERROR);
				break;
			default:

				break;
			}
			break;
#endif

		case JVX_COMPONENT_AUDIO_TECHNOLOGY:
			switch (id)
			{
#ifdef JVX_USE_PORTAUDIO
			case 0:
				adescr->assign("PortAudio Audio");
				*funcInit = jvxAuTPortAudio_init;
				*funcTerm = jvxAuTPortAudio_terminate;
				return(JVX_NO_ERROR);

			case 1:
#else
			case 0:
#endif
				mL = JVX_MAXSTRING - 1;

#if defined JVX_OS_WINDOWS
				adescr->assign("Asio Audio");
				*funcInit = jvxAuTAsio_init;
				*funcTerm = jvxAuTAsio_terminate;
#elif defined JVX_OS_LINUX
				adescr->assign("Alsa Audio");
				*funcInit = jvxAuTAlsa_init;
				*funcTerm = jvxAuTAlsa_terminate;
#elif defined JVX_OS_MACOSX
				adescr->assign("Core Audio");
				*funcInit = jvxAuTCoreAudio_init;
				*funcTerm = jvxAuTCoreAudio_terminate;
#endif
				return(JVX_NO_ERROR);
			default:

				break;
			}
			break;

#ifdef JVX_ALGO_COMPONENT_TYPE
		case JVX_ALGO_COMPONENT_TYPE:
#else
		case JVX_COMPONENT_AUDIO_NODE:
#endif
			switch (id)
			{
			case 0:
				adescr->assign("Open GL Project");
				*funcInit = jvxAuNOpenGl_init;
				*funcTerm = jvxAuNOpenGl_terminate;
				return(JVX_NO_ERROR);

			default:
				break;
			}
			break;

#ifdef JVX_HOST_USE_REMOTE_CALL

		case JVX_COMPONENT_REMOTE_CALL:
			switch (id)
			{
			case 0:
				adescr->assign("Remote Call");
				*funcInit = jvxTRemoteCall_init;
				*funcTerm = jvxTRemoteCall_terminate;
				return(JVX_NO_ERROR);

			default:
				break;
			}
			break;

#endif

#ifdef JVX_HOST_USE_CONNECTION
		case JVX_COMPONENT_CONNECTION:
			switch (id)
			{
			case 0:
				adescr->assign("Socket");
				*funcInit = jvxTSocket_init;
				*funcTerm = jvxTSocket_terminate;
				return(JVX_NO_ERROR);

#ifdef JVX_OS_WINDOWS

			case 1:
				adescr->assign("Rs232");
				*funcInit = jvxTRs232Win32_init;
				*funcTerm = jvxTRs232Win32_terminate;
				return(JVX_NO_ERROR);

#endif
			default:
				break;
			}
			break;
#endif

		default:
			break;
		}
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
}

// =============================================================
// Configure ui extensions
// =============================================================
JVX_INTERFACE IjvxQtComponentWidget;
jvxErrorType
IjvxQtComponentWidget_init_device(IjvxQtComponentWidget** onReturn, jvxBitField featureClass)
{
	if (onReturn)
		*onReturn = NULL;
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
IjvxQtComponentWidget_terminate_device(IjvxQtComponentWidget* returnMe, jvxBitField featureClass)
{
	assert(returnMe == NULL);
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
const char* componentsOnLoad_algorithms[] =
{
	"jvxAuNOpenGl",
	NULL
};

const char* componentsOnLoad_audiotechnologies[] =
{
	"jvxAuTGenericWrapper",
	nullptr
};

#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"
#include "interfaces/qt-develop-host/configureQtDevelopHost_features.h"
extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	configureQtAudioHost_features* theFeaturesA = NULL;
	configureQtDevelopHost_features* theFeaturesD = NULL;

#ifdef JVX_PROJECT_NAMESPACE
	JVX_PROJECT_NAMESPACE::myCentralWidget* theWidget = NULL;
#else
	myCentralWidget* theWidget = NULL;
#endif

	switch (features->hostFeatureTp)
	{
	case JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST:
		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesA), JVX_HOST_IMPLEMENTATION_QT_AUDIO_HOST);

#ifdef JVX_PROJECT_NAMESPACE
		theWidget = dynamic_cast<JVX_PROJECT_NAMESPACE::myCentralWidget*>(theFeaturesA->myMainWidget);
#else
		theWidget = dynamic_cast<myCentralWidget*>(theFeatures->myMainWidget);
#endif
		theFeaturesA->config_ui.minWidthWindow = JVX_MAX(theFeaturesA->config_ui.minWidthWindow, 256);
		theFeaturesA->config_ui.minHeightWindow = JVX_MAX(theFeaturesA->config_ui.minHeightWindow, 256);
		theFeaturesA->config_ui.tweakUi = JVX_QT_MAINWIDGET_NO_EXPAND_CENTER;
		break;
	case  JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST:

		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesD), JVX_HOST_IMPLEMENTATION_QT_DEVELOP_HOST);

#ifdef JVX_NO_SEQUENCER
		theFeaturesD->showExpertUi_seq = false; // Default: true	
#endif

#ifdef JVX_NO_RTVIEWER
		theFeaturesD->showExpertUi_props = false; // Default: true
		theFeaturesD->showExpertUi_plots = false; // Default: true
#endif
#ifdef JVX_NO_MESSAGES
		theFeaturesD->showExpertUi_messages = false;
#endif

#ifdef JVX_ALLOW_ONLY_OVERLAYS
		theFeaturesD->allowOverlayOnly_config = true;
#endif
		theFeaturesD->config_ui.minWidthWindow = JVX_MAX(theFeaturesD->config_ui.minWidthWindow, 256);
		theFeaturesD->config_ui.minHeightWindow = JVX_MAX(theFeaturesD->config_ui.minHeightWindow, 256);
		theFeaturesD->config_ui.tweakUi = JVX_QT_MAINWIDGET_EXPAND_FULLSIZE; // JVX_QT_MAINWIDGET_SCROLL;
		
		theFeaturesD->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
		theFeaturesD->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

		theFeaturesD->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
		theFeaturesD->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

		break;
	default:
		assert(0);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}
}


