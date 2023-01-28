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
			jvxDataConnectionRuleParameters params(false, true, true);

			res = theDataConnectionDefRule->create_connection_rule("Audio -> SpNMeasureIr -> ChanAnn -> Audio", 
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
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default", "Audio to Measure Ir");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "default", "Measure Ir to Channel Annouce");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
						"*", "default", "Channel Annouce to Speaker EQ");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 0),
						"*", "default", "Speaker Eq to Audio");
					assert(res == JVX_NO_ERROR);


					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}

			/*
			res = theDataConnectionDefRule->create_connection_rule("Audio_01 -> HoaEnc -> AsyncIO -> Audio_01",
				&theDataConnectionDefRule_id, false, true);
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
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0), // hoaEncoder
						"*", "default", "Sec Input to Hoa Encoder Node");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default", "Hoa Encoder Node to Async Node");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "default",
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_DEVICE, 0, 1),
						"*", "default", "Async Node to Sec Output");
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}

			res = theDataConnectionDefRule->create_connection_rule("AsyncIO -> Hoa2Dev -> AsyncIO",
				&theDataConnectionDefRule_id, false, true);
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
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "async-secondary", "Async Output to Hoa2Dev.2");
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
						"*", "async-secondary",
						jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
						"*", "async-secondary", "Hoa2Dev.2 to Async Input");
					assert(res == JVX_NO_ERROR);

					theDataConnectionDefRuleHdl->mark_rule_default();
				}
			}
			*/
		}
		return JVX_NO_ERROR;
	}
}