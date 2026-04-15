#include "jvx.h"

// ================================================================================================

extern "C"
{
	jvxErrorType jvx_default_connection_rules_add(IjvxDataConnections* theDataConnectionDefRule)
	{
		// std::cout << __FUNCTION__ << ": Custom connection rules hook function, located in " << __FILE__ << std::endl;

		// =========================================================================================================================
		// Add default data connection rule
		// =========================================================================================================================
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;

		if (theDataConnectionDefRule)
		{
			jvxBool dbgOut = false;
			jvxDataConnectionRuleParameters params(false, true, true, dbgOut);

			// Define that at least one process must be implemented based on this rule
			params.minNumConnectionsReady = 1;

			// theDataConnectionDefRule->set_ready_handler(&checkReadyConnections);

			res = theDataConnectionDefRule->create_connection_rule("The Master Audio Connection Rule",
				&theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default");
					assert(res == JVX_NO_ERROR);

					// From Audio Device to input mix chain Node
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0),
						"*", "default", "Bridge-device-to-mix-input", false, false);
					assert(res == JVX_NO_ERROR);

					// From Input Mix Chain Node to Audio Node
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0), 
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0),
						"*", "default", "Bridge-mix-input-to-audio-node-0", false, false);
					assert(res == JVX_NO_ERROR);

					// From Audio Node 0 to Audio Node 1
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 1),
						"*", "default", "Bridge-audio-node-0-to-audio-node-1", false, false);
					assert(res == JVX_NO_ERROR);

					// From Audio Node to Mix Chain Node
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 1),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1),
						"*", "default", "Bridge-audio-node-1-to-mix-output", false, false);
					assert(res == JVX_NO_ERROR);

					// From Mx Chain Node to Audio Device
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1),
						"*", "default",						
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default", "Bridge-mix-output-to-device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}
		}
		return JVX_NO_ERROR;
	}
}
