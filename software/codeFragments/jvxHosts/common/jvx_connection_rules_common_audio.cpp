#include "jvx.h"

// #define JVX_SPECIFIC_MAIN_NODE JVX_COMPONENT_AUDIO_NODE,0,0

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

			res = theDataConnectionDefRule->create_connection_rule("The Default Audio Connection Rule", 
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

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default",
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "default", "ABridge_dev_to_node", false, false);

					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default", "ABridge_node_to_device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}

			res = theDataConnectionDefRule->create_connection_rule("The Secondary Audio Connection Rule",
				&theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 1),
						"*", "default");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 1),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default", "ABridge_dev_to_node", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 1),
						"*", "default", "ABridge_node_to_device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}

			res = theDataConnectionDefRule->create_connection_rule("The Secondary Audio To First Audio Rule (NTASK)",
				&theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary",
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "async-secondary-n", "ABridge_dev_to_node", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "async-secondary-n",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary", "ABridge_node_to_device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}

			res = theDataConnectionDefRule->create_connection_rule("The Secondary Audio To First Audio Rule (VTASK)",
				&theDataConnectionDefRule_id, &params);
			if (res == JVX_NO_ERROR)
			{
				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
				jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
				res = theDataConnectionDefRule->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					res = theDataConnectionDefRuleHdl->specify_master(jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary",
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "async-secondary-v", "ABridge_dev_to_node", false, false);
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
#ifdef JVX_SPECIFIC_MAIN_NODE
						jvxComponentIdentification(JVX_SPECIFIC_MAIN_NODE),
#else
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
#endif
						"*", "async-secondary-v",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary", "ABridge_node_to_device", false, false);
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}
		}
		return JVX_NO_ERROR;
	}
}