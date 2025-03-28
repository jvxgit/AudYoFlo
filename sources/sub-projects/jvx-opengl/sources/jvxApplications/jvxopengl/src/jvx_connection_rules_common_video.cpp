#include "jvx.h"

extern "C"
{
	jvxErrorType jvx_default_connection_rules_add(IjvxDataConnections* theDataConnectionDefRule)
	{
		std::cout << __FUNCTION__ << ": Custom connection rules hook function, located in " << __FILE__ << std::endl;

		// =========================================================================================================================
		// Add default data connection rule
		// =========================================================================================================================
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;

		if (theDataConnectionDefRule)
		{
			jvxDataConnectionRuleParameters params(false, true);
			theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
			res = theDataConnectionDefRule->create_connection_rule("The Default Video Connection Rule", &theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_VIDEO_DEVICE, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE),
						"*", "default");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_VIDEO_DEVICE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_VIDEO_DEVICE, JVX_SIZE_SLOT_RETAIN, JVX_SIZE_SLOT_RETAIN),
						"*", "default", "VBridge_device_to_device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}
			// add the default data processing chain
		}
		return JVX_NO_ERROR;
	}
}