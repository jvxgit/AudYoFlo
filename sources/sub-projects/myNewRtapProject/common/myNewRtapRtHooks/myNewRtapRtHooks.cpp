#include "jvx.h"
#include "myNewRtapAudioNode.h"
#include "myNewRtapRtHooks.h"

#define JVX_ALGO_DESCRIPTION "My new RTAP Project"
#define JVX_ALGO_INIT_FUNC myNewRtapAudioNode_init
#define JVX_ALGO_TERMINATE_FUNC myNewRtapAudioNode_terminate

#include "jvxApplications/jvx_init_link_objects_tpl.h"

extern "C"
{
	void jvx_command_line_specify(IjvxCommandLine* cmdLine)
	{
		jvxErrorType resL;
		if (cmdLine)
		{
			resL = cmdLine->register_option("--my-spec-config", "-c", "Specify an application specific command line option", "", true, JVX_DATAFORMAT_STRING);
		}
	}

	void jvx_command_line_read(IjvxCommandLine* cmdLine)
	{
		jvxApiString opt_text;
		jvxErrorType resL;
		if (cmdLine)
		{
			resL = cmdLine->content_entry_option("--my-spec-config", 0, &opt_text, JVX_DATAFORMAT_STRING);
		}
	}
}

// ===============================================================================
const jvxModuleOnStart componentsOnLoad_algorithms[] =
{
	{"myNewRtapAudioNode"},
	NULL
};

const jvxModuleOnStart componentsOnLoad_audiotechnologies[] =
{
	{"jvxAuTGenericWrapper"},
	nullptr
};

jvxErrorType configureHost_features_local(configureHost_features* theFeaturesF)
{

	theFeaturesF->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_NODE] = true;
	theFeaturesF->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_NODE] = componentsOnLoad_algorithms;

	theFeaturesF->flag_blockModuleEdit[JVX_COMPONENT_AUDIO_TECHNOLOGY] = true;
	theFeaturesF->lst_ModulesOnStart[JVX_COMPONENT_AUDIO_TECHNOLOGY] = componentsOnLoad_audiotechnologies;

	return JVX_NO_ERROR;
}