#include "CayfConnectFileOutput.h"
//  #include "ayfAuNMixer.h"
#include "jvxSpNMixChainEnterLeave.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectFileOutput"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectFileOutputContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectFileOutputContainer::CayfConnectFileOutputContainer()
{
}

CayfComponentLib*
CayfConnectFileOutputContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectFileOutput* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectFileOutput(
		"Audio Device Object - LeaveChain", false,
		"audio_leavechain", 0, "ayfConnectLeaveChain",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/ayfConnectLeaveChain", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectFileOutputContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectFileOutput::CayfConnectFileOutput(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
	CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL, parent), passthroughMode(passthroughModeArg)
{
	/*
	modName = "CayfWeaverConnectStarter";
	regToken = modName;
	if (regTokenArg)
	{
		regToken = regTokenArg;
	}
	*/
	mainNodeName = "jvxSpNMixChainEnterLeave";
};

jvxErrorType
CayfConnectFileOutput::allocate_main_node()
{
	jvxSpNMixChainEnterLeave_init(&priObj);
	this->mainObj = priObj;
	jvxSpNMixChainEnterLeave_init(&secObj);
	this->subsequentComponents.push_back(secObj);
	return JVX_NO_ERROR;

}

jvxErrorType
CayfConnectFileOutput::deallocate_main_node()
{
	jvxSpNMixChainEnterLeave_terminate(priObj);
	this->mainObj = nullptr;
	jvxSpNMixChainEnterLeave_terminate(secObj);
	subsequentComponents.clear();
	return JVX_NO_ERROR;
}

jvxErrorType
CayfConnectFileOutput::on_main_node_selected(IjvxNode* node)
{
	IjvxProperties* props = nullptr;
	IjvxManipulate* manIf = nullptr;
	jvxApiString astr;
	jvxVariant val;
	jPAD ident;
	jPD trans;
	jvxSelectionList sel;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;

	// Get the number of output channels towards file output
	jvxSize numChannelsToFileOutput = parent->audio_parameter_on_start().numInChans;
	jvxSize numChannelsFromDevice = 0;

	// Add the sub components
	props = reqInterfaceObj<IjvxProperties>(node);
	manIf = reqInterfaceObj<IjvxManipulate>(node);

	assert(props);
	assert(manIf);
	
	if(node == priObj)
	{
		astr = "MixChain - File Output - mix-in";
		val.assign(&astr);
		resL = manIf->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &val);
	
		ident.reset("/number_channels_side");
		resL = props->set_property(callGate, jPRIO<jvxSize>(numChannelsFromDevice), ident);
		assert(resL == JVX_NO_ERROR);

		jvx_bitZSet(sel.bitFieldSelected(), 0); // <- Input
		ident.reset("/operation_mode");
		resL = props->set_property(callGate, jPROSL(sel), ident, jPD(true));
		assert(resL == JVX_NO_ERROR);
	}

	if (node == secObj)
	{
		astr = "MixChain - File Output - mix-out";
		val.assign(&astr);
		resL = manIf->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &val);

		ident.reset("/number_channels_side");
		resL = props->set_property(callGate, jPRIO<jvxSize>(numChannelsToFileOutput), ident);
		assert(resL == JVX_NO_ERROR);

		jvx_bitZSet(sel.bitFieldSelected(), 1); // <- Output
		ident.reset("/operation_mode");
		resL = props->set_property(callGate, jPROSL(sel), ident, jPD(true));
		assert(resL == JVX_NO_ERROR);
	}

	return JVX_NO_ERROR;
}
