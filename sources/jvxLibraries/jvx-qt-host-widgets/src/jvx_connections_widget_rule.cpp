#include "jvx_connections_widget.h"
#include "jvx_connections_widget_const.h"

void 
jvx_connections_widget::select_connection_rule(int id)
{
	jvxBitField btfd;
	if (id == 0)
	{
		id_select_rule.id_rule = JVX_SIZE_UNSELECTED;
		id_select_rule_bridgename.id_bridgename = 0;
	}
	else
	{
		id_select_rule.id_rule = id - 1;
		id_select_rule_bridgename.id_bridgename = 0;
	}
	jvx_bitFClear(btfd);
	jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
	update_window_core(btfd);
}

void
jvx_connections_widget::select_connection_rule_bridge(int id)
{
	jvxBitField btfd;
	id_select_rule_bridgename.id_bridgename = id;

	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		duplicate_content_bridge(id_select_rule_bridgename.id_bridgename);
	}
	jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
	update_window_core(btfd);
}

void
jvx_connections_widget::remove_connection_rule()
{
	jvxSize num = 0;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnectionRule* myRule = NULL;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxBitField btfd;
	if (JVX_CHECK_SIZE_SELECTED(id_select_rule.id_rule))
	{
		theDataConnections->number_connection_rules(&num);
		if (id_select_rule.id_rule < num)
		{
			res = theDataConnections->reference_connection_rule(id_select_rule.id_rule, &myRule);
			assert(res == JVX_NO_ERROR);

			if (myRule)
			{
				theDataConnections->uid_for_reference(myRule, &uId);
			}
			res = theDataConnections->return_reference_connection_rule(myRule);
			assert(res == JVX_NO_ERROR);

			res = theDataConnections->remove_connection_rule(uId);
			assert(res == JVX_NO_ERROR);
			
			jvx_bitFClear(btfd);
			jvx_bitSet(btfd, REDRAW_PROCESS_REDRAW_CONNECTION_RULES_SHIFT);
			jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
			update_window_core(btfd);
		}
	}
}


void 
jvx_connections_widget::rdz_select_master_type(int id)
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		theConnectionRuleDropzone.cpMaster = (jvxComponentType)id;
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_select_from_bridge_type(int id)
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_cp_from.tp = (jvxComponentType)id;
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_select_to_bridge_type(int id)
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_cp_to.tp = (jvxComponentType)id;
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_master_fac_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		theConnectionRuleDropzone.masterFactoryWildcard = lineEdit_ruleMasterNameWC->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_master_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		theConnectionRuleDropzone.masterWildcard = lineEdit_ruleMaster->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_master_sid()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxSize val = jvx_string2Size(lineEdit_mastersid->text().toLatin1().data(), err);
		if (!err)
		{
			theConnectionRuleDropzone.cpMaster.slotid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_master_ssid()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxSize val = jvx_string2Size(lineEdit_masterssid->text().toLatin1().data(), err);
		if (!err)
		{
			theConnectionRuleDropzone.cpMaster.slotsubid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_from_fac_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_from_fac_wc = lineEdit_fromBridgeWC->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_from_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_from_wc = lineEdit_fromBridgeWCC->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_from_sid()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxSize val = jvx_string2Size(lineEdit_fromBridgesid->text().toLatin1().data(), err);
		if (!err)
		{
			edit_cp_from.slotid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_from_ssid()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxSize val = jvx_string2Size(lineEdit_fromBridgessid->text().toLatin1().data(), err);
		if (!err)
		{
			edit_cp_from.slotsubid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_to_fac_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_to_fac_wc = lineEdit_toBridgeWC->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_to_wildcard()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		edit_to_wc = lineEdit_toBridgeWCC->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_to_sid()
{
	jvxBitField btfd;
	jvxBool err = false;
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxSize val = jvx_string2Size(lineEdit_toBridgesid->text().toLatin1().data(), err);
		if (!err)
		{
			edit_cp_to.slotid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_to_ssid()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxSize val = jvx_string2Size(lineEdit_toBridgessid->text().toLatin1().data(), err);
		if (!err)
		{
			edit_cp_to.slotsubid = val;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_rulebridgename()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
			
		edit_bridge_name = lineEdit_ruleBridgename->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_edit_rulename()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;

		theConnectionRuleDropzone.rule_name = lineEdit_ruleName->text().toLatin1().data();
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_add_bridgerule()
{
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		jvxBitField btfd;
		jvxBool err = false;
		jvxBool alreadyThere = false;
		if (!edit_bridge_name.empty())
		{
			std::list<jvxConnectionRuleBridge>::iterator elm = theConnectionRuleDropzone.bridges.begin();
			
			for (; elm != theConnectionRuleDropzone.bridges.end(); elm++)
			{
				if (elm->bridge_name == edit_bridge_name)
				{
					alreadyThere = true;
					break;
				}
			}

			if (!alreadyThere)
			{
				jvxConnectionRuleBridge newElm;
				newElm.bridge_name = edit_bridge_name;
				newElm.fromCpTp = edit_cp_from;
				newElm.fromFactoryWildcard = edit_from_fac_wc;
				newElm.fromConnectorWildcard= edit_from_wc;
				newElm.toCpTp = edit_cp_to;
				newElm.toFactoryWildcard = edit_to_fac_wc;
				newElm.toConnectorWildcard = edit_to_wc;
				id_select_rule_bridgename.id_bridgename = theConnectionRuleDropzone.bridges.size();
				theConnectionRuleDropzone.bridges.push_back(newElm);
				
			}
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}

}

void
jvx_connections_widget::rdz_add_rule()
{
	jvxBitField btfd;
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		theConnectionRuleDropzone.interceptors = rule_ic;
		theConnectionRuleDropzone.essential = rule_ess;

		jvxErrorType res = jvx_rule_from_dropzone(theDataConnections, &theConnectionRuleDropzone);
		if (res == JVX_NO_ERROR)
		{
			jvxSize num;
			reset_rule_dropzone();
			theDataConnections->number_connection_rules(&num);
			id_select_rule.id_rule = num - 1;
		}
		jvx_bitSet(btfd, REDRAW_PROCESS_REDRAW_CONNECTION_RULES_SHIFT);
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_rem_bridgerule()
{
	jvxBitField btfd;
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		if (id_select_rule_bridgename.id_bridgename < theConnectionRuleDropzone.bridges.size())
		{
			std::list<jvxConnectionRuleBridge>::iterator elm = theConnectionRuleDropzone.bridges.begin();
			std::advance(elm, id_select_rule_bridgename.id_bridgename);
			theConnectionRuleDropzone.bridges.erase(elm);
			jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
			update_window_core(btfd);
		}
	}

}

void
jvx_connections_widget::rdz_copy()
{
	jvxBitField btfd;
	jvxSize j;
	jvxSize num = 0;
	IjvxDataConnectionRule* myRule = NULL;
	jvxApiString str1, str2;
	jvxComponentIdentification cpTp;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numB = 0;

	if (JVX_CHECK_SIZE_SELECTED(id_select_rule.id_rule))
	{
		theDataConnections->number_connection_rules(&num);
		if (id_select_rule.id_rule < num)
		{
			res = theDataConnections->reference_connection_rule(id_select_rule.id_rule, &myRule);
			assert(res == JVX_NO_ERROR);

			myRule->description_rule(&str1, NULL);
			theConnectionRuleDropzone.rule_name = str1.std_str();

			str1.clear();
			myRule->get_master(&theConnectionRuleDropzone.cpMaster, &str1, &str2);
			theConnectionRuleDropzone.masterFactoryWildcard = str1.std_str();
			theConnectionRuleDropzone.masterWildcard = str2.std_str();

			theConnectionRuleDropzone.bridges.clear();
			myRule->number_bridges(&numB);
			for (j = 0; j < numB; j++)
			{
				jvxConnectionRuleBridge newElm;

				str1.clear();
				myRule->get_bridge(j, &str1, NULL, NULL, NULL, NULL, NULL, NULL);
				newElm.bridge_name = str1.std_str();

				str1.clear();
				str2.clear();

				myRule->get_bridge(j, NULL, &newElm.fromCpTp, &str1, &str2, NULL, NULL, NULL);
				newElm.fromFactoryWildcard = str1.std_str();
				newElm.fromConnectorWildcard = str2.std_str();

				str1.clear();
				str2.clear();


				myRule->get_bridge(j, NULL, NULL, NULL, NULL, &newElm.toCpTp, &str1, &str2);
				newElm.toFactoryWildcard = str1.std_str();
				newElm.toConnectorWildcard = str2.std_str();

				id_select_rule_bridgename.id_bridgename = theConnectionRuleDropzone.bridges.size();
				theConnectionRuleDropzone.bridges.push_back(newElm);
				duplicate_content_bridge(id_select_rule_bridgename.id_bridgename);
			}
			id_select_rule.id_rule = JVX_SIZE_UNSELECTED;

			jvx_bitSet(btfd, REDRAW_PROCESS_REDRAW_CONNECTION_RULES_SHIFT);
			jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
			update_window_core(btfd);
		}
	}
}

void
jvx_connections_widget::rdz_clear()
{
	jvxBitField btfd;
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_rule.id_rule))
	{
		reset_rule_dropzone();
		id_select_rule.id_rule = JVX_SIZE_UNSELECTED;
		jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
		update_window_core(btfd);
	}
}

void
jvx_connections_widget::rdz_clicked_interceptors(bool tog)
{
	jvxBitField btfd;
	rule_ic = tog;
	jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
	update_window_core(btfd);
}

void
jvx_connections_widget::rdz_clicked_essentials(bool tog)
{
	jvxBitField btfd;
	rule_ess = tog;
	jvx_bitSet(btfd, REDRAW_PROCESS_SHOW_CONNECTION_RULE_SHIFT);
	update_window_core(btfd);
}

void
jvx_connections_widget::reset_rule_dropzone()
{
	theConnectionRuleDropzone.rule_name = "";
	theConnectionRuleDropzone.interceptors = false;
	theConnectionRuleDropzone.cpMaster.tp = JVX_COMPONENT_UNKNOWN;
	theConnectionRuleDropzone.cpMaster.slotid = JVX_SIZE_UNSELECTED;
	theConnectionRuleDropzone.cpMaster.slotsubid = JVX_SIZE_UNSELECTED;
	theConnectionRuleDropzone.masterFactoryWildcard = "";
	theConnectionRuleDropzone.masterWildcard = "";
	theConnectionRuleDropzone.bridges.clear();

	edit_bridge_name = "";

	edit_cp_to.tp = JVX_COMPONENT_UNKNOWN;
	edit_cp_to.slotid = JVX_SIZE_SLOT_RETAIN;
	edit_cp_to.slotsubid = JVX_SIZE_SLOT_RETAIN;
	edit_to_fac_wc = "";
	edit_to_wc = "";

	edit_cp_from.tp = JVX_COMPONENT_UNKNOWN;
	edit_cp_from.slotid = JVX_SIZE_SLOT_RETAIN;
	edit_cp_from.slotsubid = JVX_SIZE_SLOT_RETAIN;
	edit_from_fac_wc = "";
	edit_from_wc = "";
}

void
jvx_connections_widget::duplicate_content_bridge(jvxSize id)
{
	std::list<jvxConnectionRuleBridge>::iterator elm = theConnectionRuleDropzone.bridges.begin();
	if (id < theConnectionRuleDropzone.bridges.size())
	{
		std::advance(elm, id);
		edit_bridge_name = elm->bridge_name;
		edit_from_wc = elm->fromConnectorWildcard;
		edit_from_fac_wc = elm->fromFactoryWildcard;
		edit_cp_from = elm->fromCpTp;
		edit_to_wc = elm->toConnectorWildcard;
		edit_to_fac_wc = elm->toFactoryWildcard;
		edit_cp_to = elm->toCpTp;
	}
}
