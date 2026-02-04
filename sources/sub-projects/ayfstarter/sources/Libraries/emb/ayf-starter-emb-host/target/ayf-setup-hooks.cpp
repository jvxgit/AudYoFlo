#include "interfaces/all-hosts/configHostFeatures_common.h"
#include "ayfATTypical.h"

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

extern "C"
{
	jvxErrorType jvx_configure_host_features(configureHost_features* features)
	{	 		
		configureHost_features* theFeaturesCH = nullptr;
		features->request_specialization(reinterpret_cast<jvxHandle**>(&theFeaturesCH), JVX_HOST_IMPLEMENTATION_HOST);
		if (theFeaturesCH)
		{
			theFeaturesCH->numSlotsComponents[JVX_COMPONENT_SYSTEM_AUTOMATION] = 1;
			theFeaturesCH->flag_blockModuleEdit[JVX_COMPONENT_SYSTEM_AUTOMATION] = true;
			theFeaturesCH->lst_ModulesOnStart[JVX_COMPONENT_SYSTEM_AUTOMATION] = componentsOnLoad_automation;

		}
		return(JVX_NO_ERROR);
	}

	jvxErrorType jvx_invalidate_host_features(configureHost_features* features)
	{
		return(JVX_NO_ERROR);
	}

	jvxErrorType jvx_access_link_objects(
		jvxInitObject_tp* funcInit,
		jvxTerminateObject_tp* funcTerm, jvxApiString* adescr,
		jvxComponentType tp, jvxSize id)
	{
		jvxSize cnt = 0;
		// std::cout << __FUNCTION__ << ", " << __FILE__ << "," << __LINE__ << ": Implementation in project." << std::endl;
		// 
		switch (tp)
		{

		case JVX_COMPONENT_SYSTEM_AUTOMATION:
			if (id == cnt)
			{
				adescr->assign("Typical Automation");
				*funcInit = ayfATTypical_init;
				*funcTerm = ayfATTypical_terminate;
				return(JVX_NO_ERROR);
			}
			cnt++;
			break;
		}
		// Default implementation does just nothing
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}
}

// ==========================================================================================================
// ==========================================================================================================

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_RUN 6
static jvxOneSequencerStepDefinition defaultSteps_2audio_run[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_RUN] =
{
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Prepare all connections", "PrepConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all prepared", "AfterAllPrep", 0, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Start all connections", "StartConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all started", "AfterAllStarted", 1, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER, JVX_COMPONENT_UNKNOWN, 0, 0, "Update Viewer", "UpdateViewer", 0, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_WAIT_AND_RUN_TASKS, JVX_COMPONENT_UNKNOWN, 0, 0, "Wait and run tasks", "WaitRun", 0, -1, "no label", "no label" }
};

#define JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_LEAVE 5
static jvxOneSequencerStepDefinition defaultSteps_2audio_leave[JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_LEAVE] =
{
	{ JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all stop", "BeforeAllStop", 16, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Stop all connections", "StopConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all postprocess", "BeforeAllPost", 17, 1000, "no label", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS, JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE, "Postprocess all connections", "PostProcessConnections", 0, 1000, "*", "no label" },
	{ JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER, JVX_COMPONENT_UNKNOWN, 0, 0, "Update Viewer", "UpdateViewer", 0, 1000, "no label", "no label" },
};
extern "C"
{
	jvxErrorType jvx_default_sequence_add(IjvxSequencer* theSeq)
	{
		// std::cout << __FUNCTION__ << ": Custom sequencer hook function, located in " << __FILE__ << std::endl;

		jvxErrorType res = JVX_NO_ERROR;
		//	featuresA->useDefaultStepsOnlyIfNoneConfig = false;
		//	featuresA->nm_default_seq = "jvxInspireSequencerSet";

		res = jvx_add_default_sequence_sequencer(theSeq,
			false,
			defaultSteps_2audio_run, JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_RUN,
			defaultSteps_2audio_leave, JVX_NUMBER_DEFAULT_STEPS_SEQUENCER_2AUDIO_LEAVE,
			"default sequencer spec 2 audio");

		return res;
	};
}