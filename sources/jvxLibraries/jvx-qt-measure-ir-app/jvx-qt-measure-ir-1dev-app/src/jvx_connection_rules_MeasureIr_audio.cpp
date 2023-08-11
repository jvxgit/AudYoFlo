#include "jvx.h"

extern "C"
{
	jvxErrorType jvx_default_connection_rules_add_specialization(jvxComponentIdentification* tpEnque, jvxBool is1dev);

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
			jvxDataConnectionRuleParameters params(false, true, true);

			res = theDataConnectionDefRule->create_connection_rule("Audio -> SpNMeasureIr -> ChanAnn -> Audio", 
				&theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				// Find the component type to involve in queue
				jvxComponentIdentification tpEnqueue = jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0);
				jvx_default_connection_rules_add_specialization(&tpEnqueue, true);

				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default", "Audio to Measure Ir", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "default", "Measure Ir to Channel Annouce", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "default",
						tpEnqueue,
						"*", "default", "Channel Annouce to Specific", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						tpEnqueue,
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default", "Speaker Eq to Audio", false, false);
					assert(res == JVX_NO_ERROR);


					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}
		}
		return JVX_NO_ERROR;
	}
}