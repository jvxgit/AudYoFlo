// =============================================================
// Involve components from static lib
// =============================================================

#include "jvx.h"
#include "ayfAuNStarter.h"
#include "ayfATTypical.h"
#include "interfaces/qt-audio-host/configureQtAudioHost_features.h"

#define JVX_ALGO_DESCRIPTION "ayfAuNStarter"
#define JVX_ALGO_INIT_FUNC ayfAuNStarter_init
#define JVX_ALGO_TERMINATE_FUNC ayfAuNStarter_terminate

#define JVX_HOST_AUTOMATION_COMPONENT \
	case JVX_COMPONENT_SYSTEM_AUTOMATION: \
		switch (id) \
		{ \
		case 0: \
			adescr->assign("Typical Automation"); \
			*funcInit = ayfATTypical_init; \
			*funcTerm = ayfATTypical_terminate; \
			return(JVX_NO_ERROR); \
			break; \
		} \
		break;

/*

jvxErrorType jvx_access_link_objects_audio_tech(
	jvxInitObject_tp * funcInit,
	jvxTerminateObject_tp * funcTerm, jvxApiString * adescr,
	jvxSize id)
{
	jvxSize cnt = 0;

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
#endif

#if defined JVX_USE_ALSA
	if (id == cnt)
	{
		adescr->assign("Alsa Audio");
		*funcInit = jvxAuTAlsa_init;
		*funcTerm = jvxAuTAlsa_terminate;
		return(JVX_NO_ERROR);
	}
	cnt++;
#endif

#if defined JVX_USE_COREAUDIO

	if (id == cnt)
	{
		adescr->assign("Core Audio");
		*funcInit = jvxAuTCoreAudio_init;
		*funcTerm = jvxAuTCoreAudio_terminate;
		return(JVX_NO_ERROR);
	}
	cnt++;
#endif

#if defined JVX_USE_ANDROID

	if (id == cnt)
	{
		adescr->assign("Android Audio");
		*funcInit = jvxAuTAndroid_init;
		*funcTerm = jvxAuTAndroid_terminate;
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

#if defined JVX_USE_PIPEWIRE
	if (id == cnt)
	{
		adescr->assign("Pipewire Audio");
		*funcInit = jvxAuTPipewire_init;
		*funcTerm = jvxAuTPipewire_terminate;
		return(JVX_NO_ERROR);
	}
	cnt++;
#endif
	return(JVX_ERROR_ELEMENT_NOT_FOUND);
}


#define JVX_HOST_AUDIO_TECHNOLOGIES \
	case JVX_COMPONENT_AUDIO_TECHNOLOGY: \
		return jvx_access_link_objects_audio_tech( \
			funcInit, funcTerm, adescr, id); \
		break;
*/

#include "jvxApplications/jvx_init_link_objects_tpl.h"
#include "../common/ayfstarter-common.h"

// ==============================================================
const jvxModuleOnStart componentsOnLoad_algorithms[] = 
{
	{"ayfAuNStarter"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_audiotechnologies[] =
{
	{"jvxAuTGenericWrapper"},
	nullptr
};

const jvxModuleOnStart componentsOnLoad_automation[] =
{
	{"ayfATTypical",[](IjvxObject* obj) {
	IjvxProperties* props = reqInterfaceObj<IjvxProperties>(obj);
	if (props)
	{
		jvxCallManagerProperties callGate;
		jvxCBool flag = 1;
		jvxErrorType res = props->set_property(callGate, jPRFCBOOL(flag), jPAD("/audio/primary_audio_tech_lowlevel"));
		retInterfaceObj<IjvxProperties>(obj, props);
	}}},
	nullptr
};


