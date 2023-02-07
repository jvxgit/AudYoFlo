#ifndef _IJVXPROPERTYEXTENDERCHAINCONTROL_H__
#define _IJVXPROPERTYEXTENDERCHAINCONTROL_H__

// Interface to provide callbacks inbetween relevant steps in component actication in 
// sub chains. We may interact or we may skip these functions.
JVX_INTERFACE IayfConnectionStateSwitchNode
{
public:
	virtual ~IayfConnectionStateSwitchNode() {};

	/**
	 * This function is called before a component state switch. In the callback, the component must 
	 * be controlled to change the state. For example, on state switch JVX_STATE_SWITCH_SELECT the component
	 * must be selected. The caller of the callback does NOT run the select itself unless the function
	 * returns error code <JVX_ERROR_UNSUPPORTED>.
	 *///====================================================================================================================
	virtual jvxErrorType runStateSwitch(jvxStateSwitch ss, IjvxNode* node, const char* moduleName, IjvxObject* theOwner = nullptr) = 0;

	/**
	 * This function is called right before the chain is setup by connecting the components. At this moment, all 
	 * components are in state ACTIVE.
	 *///====================================================================================================================
	virtual jvxErrorType componentsAboutToConnect() = 0;
	
	/**
	 * This function is called on every test run of the micro connection. With every test run the processing parameters may have changed
	 * and may yield a specific action for automation. The test should never be run if the components are in state PROCESSING.
	 *///====================================================================================================================
	virtual jvxErrorType runTestChainComplete(jvxErrorType lastResult, IjvxNode* node, const char* moduleName, jvxSize uniqueId) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderChainControl
{
public:
	virtual ~IjvxPropertyExtenderChainControl() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_stateswitch_node_handler(IayfConnectionStateSwitchNode* ssCb) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION attach_component_chain(
		const char* str, jvxComponentType tp = JVX_COMPONENT_AUDIO_NODE, 
		jvxBool activateBeforeAttach = false, jvxSize* uniqueId = nullptr,
		IjvxReferenceSelector* deciderArg = nullptr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION attach_component_chain(
		const char* str, IjvxObject* theObj,  
		jvxSize* uniqueId = nullptr) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION detach_component_chain(
		const char* str, jvxSize uniqueId = JVX_SIZE_UNSELECTED) = 0;
};

#endif
