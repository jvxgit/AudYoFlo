#include "jvxLibHost.h"

// =============================================================================
// Interface IjvxReportStateSwitch
// =============================================================================

/**
 * Function called right before the state switch is about to happen. We may provide 
 * a control handle on this operation as soon as the component is selected.
 */// ==============================================================================
jvxErrorType 
jvxLibHost::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp)
{
	jvx_componentIdPropsCombo refProps;
	std::map<jvxComponentIdentification, oneCheckoutPropReference>::iterator prop_elm;

	// Forward state switch to automation function
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss, 
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);

	elm.set_immediate(true);
	switch (ss)
	{
	case JVX_STATE_SWITCH_UNSELECT:

		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, refProps);

		this->request_command(elm);

		jvx_componentIdentification_properties_erase(
			involvedHost.hHost, theRegisteredComponentReferenceTriples,
			tp);

		// Clear out the property reference at this point
		// to prevent any following accesses to the unselected component!
		prop_elm = checkedOutPropReferences.find(tp);
		if (prop_elm != checkedOutPropReferences.end())
		{
			checkedOutPropReferences.erase(prop_elm);
		}
		break;

	case JVX_STATE_SWITCH_DEACTIVATE:
		jvx_componentIdentification_properties_find(
			theRegisteredComponentReferenceTriples,
			tp, refProps);

		this->request_command(elm);
		break;

	case JVX_STATE_SWITCH_POSTPROCESS:
		this->request_command(elm);
		break;

	case JVX_STATE_SWITCH_STOP:
		this->request_command(elm);
		break;

	default:
		break;
	}
	return(JVX_NO_ERROR);
}

/**
 * Function called right after the state switch has happened. We may return
 * a control handle on this operation as soon as the component is unselected.
 */// ==============================================================================
jvxErrorType
jvxLibHost::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc)
{
	// Function called in case a component is set into a new state
	jvx_componentIdPropsCombo refProps;
	CjvxReportCommandRequest_ss elm(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH, tp, ss,
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_AUTOMATION);
	elm.set_immediate(true);

	if (suc == JVX_NO_ERROR)
	{
		switch (ss)
		{
		case JVX_STATE_SWITCH_SELECT:
			jvx_componentIdentification_properties_direct_add(
				involvedHost.hHost, theRegisteredComponentReferenceTriples,
				tp, &refProps);

			this->request_command(elm);
			break;

		case JVX_STATE_SWITCH_ACTIVATE:		

			jvx_componentIdentification_properties_find(
				theRegisteredComponentReferenceTriples,
				tp, refProps);

			this->request_command(elm);
			break;

		case JVX_STATE_SWITCH_PREPARE:
			this->request_command(elm);
			break;

		case JVX_STATE_SWITCH_START:
			this->request_command(elm);
			break;

		default:
			break;
		}
	}
	return JVX_NO_ERROR;
}