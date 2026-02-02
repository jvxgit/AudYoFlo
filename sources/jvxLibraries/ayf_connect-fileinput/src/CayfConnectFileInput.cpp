#include "CayfConnectFileInput.h"
//  #include "ayfAuNMixer.h"
#include "jvxSpNMixChainEnterLeave.h"

// ========================================================================================================

#define AYF_BINDING_MODULE_NAME "CayfConnectFileInput"
#define AYF_MODULE_CONTAINER_TYPE CayfConnectFileInputContainer
#include "ayf-component-entries-tpl.h"

// ========================================================================================================

CayfConnectFileInputContainer::CayfConnectFileInputContainer()
{
}

CayfComponentLib*
CayfConnectFileInputContainer::allocateDeviceObject(int passthroughMode, CayfComponentLibContainer* parent, const char* regTokenArg)
{
	// Embedding is online, let us check what we need to do now
	CayfConnectFileInput* newInst = nullptr;
	JVX_SAFE_ALLOCATE_OBJECT(newInst, CayfConnectFileInput(
		"Audio Device Object - EnterChain", false,
		"audio_enterchain", 0, "ayfConnectEnterChain",
		JVX_COMPONENT_ACCESS_DYNAMIC_LIB, JVX_COMPONENT_AUDIO_NODE,
		"audio_device/ayfConnectEnterChain", nullptr, (passthroughMode != 0), parent, regTokenArg));
	return newInst;
}

void
CayfConnectFileInputContainer::deallocateDeviceObject(CayfComponentLib* delMe)
{
	JVX_SAFE_DELETE_OBJECT(delMe);
}

// ========================================================================================================

CayfConnectFileInput::CayfConnectFileInput(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, jvxBool passthroughModeArg, CayfComponentLibContainer* parent, const char* regTokenArg) :
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
CayfConnectFileInput::allocate_main_node()
{
	return jvxSpNMixChainEnterLeave_init(&this->mainObj);
}

jvxErrorType
CayfConnectFileInput::deallocate_main_node()
{
	return jvxSpNMixChainEnterLeave_terminate(this->mainObj);
}

jvxErrorType
CayfConnectFileInput::on_main_node_selected()
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

	// Get the number of output channels
	jvxSize numChannels = parent->audio_parameter_on_start().numOutChans;

	// Add the sub components
	props = reqInterfaceObj<IjvxProperties>(this->mainNode);
	manIf = reqInterfaceObj<IjvxManipulate>(this->mainNode);

	if (props)
	{
		// This is the MixChain input component!
		if (manIf)
		{
			astr = "MixChain - File Input";
			val.assign(&astr);
			resL = manIf->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &val);
		}

		ident.reset("/number_channels_side");
		resL = props->set_property(callGate, jPRIO<jvxSize>(numChannels), ident);
		if (resL != JVX_NO_ERROR)
		{
			/*
			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
			log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
				astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
			JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
			*/
		}

		jvx_bitZSet(sel.bitFieldSelected(), 0); // <- Input
		ident.reset("/operation_mode");
		resL = props->set_property(callGate, jPROSL(sel), ident, jPD(true));
		if (resL != JVX_NO_ERROR)
		{
			/*
			JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
			log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the operation mode to " <<
				astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
			JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
			*/
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfConnectFileInput::before_main_node_unselect()
{
	IjvxProperties* props = nullptr;
	// Add the sub components
	props = reqInterfaceObj<IjvxProperties>(this->mainNode);
	if (props)
	{
	}
	return JVX_NO_ERROR;
}

