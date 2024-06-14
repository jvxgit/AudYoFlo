#include "jvxHosts/CjvxDataConnectionRule.h"
#include <sstream>

CjvxDataConnectionRule::CjvxDataConnectionRule(jvxDataConnectionRuleParameters* params, jvxSize id_category)
{
	reset_master();
	if (params)
	{
		params_init = *params;
	}
	catId = id_category;
	/*
	interceptors = interceptors_in;
	essential = essential_in;
	connectProcess = connect_process;
	params_init.dbg_output = debug_output;
	*/
}

void
CjvxDataConnectionRule::set_name(const std::string& nm)
{
	my_rule_name = nm;
};

void
CjvxDataConnectionRule::add_descr(const std::string& descr)
{
	my_object_descr = descr;
};
// ==================================================================================================================

jvxErrorType
CjvxDataConnectionRule::description_rule(jvxApiString* nmReturn, jvxBool* isDefaultRule)
{
	if (nmReturn)
		nmReturn->assign(my_rule_name);
	if (isDefaultRule)
		*isDefaultRule = isDefault;
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxDataConnectionRule::mark_rule_default()
{
	isDefault = true;
	return JVX_NO_ERROR;
}

// ==================================================================================================================

jvxErrorType
CjvxDataConnectionRule::specify_master(const jvxComponentIdentification& cpId, const char* selectionExpressionFac, const char*  selectionExpressionMas)
{
	theMaster.idCp = cpId;
	theMaster.sel_expression_fac = selectionExpressionFac;
	theMaster.sel_expression_macon = selectionExpressionMas;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::reset_master()
{
	my_rule_name = "no-rule-specified";
	theMaster.idCp.tp = JVX_COMPONENT_UNKNOWN;
	theMaster.idCp.slotid = JVX_SIZE_SLOT_RETAIN;
	theMaster.idCp.slotsubid = JVX_SIZE_SLOT_RETAIN;
	theMaster.sel_expression_fac = "*";
	theMaster.sel_expression_macon = "*";
	isDefault = false;

	return JVX_NO_ERROR;
}

// ==================================================================================================================
// ==================================================================================================================

jvxErrorType
CjvxDataConnectionRule::add_bridge_specification(const jvxComponentIdentification& cp_id_from, const char* from_factory, const char* from_connector,
	const jvxComponentIdentification& cp_id_to, const char* to_factory, const char* to_connector, const char* nmBridge,
	jvxBool thread_group, jvxBool boost_group, jvxSize oconIdTrigger, jvxSize iconIdTrigger)
{
	idAndBridge newElm;
	newElm.conn_from.idCp = cp_id_from;
	newElm.conn_from.sel_expression_fac = from_factory;
	newElm.conn_from.sel_expression_macon = from_connector;
	newElm.conn_from.idTrigger = oconIdTrigger;

	newElm.conn_to.idCp = cp_id_to;
	newElm.conn_to.sel_expression_fac = to_factory;
	newElm.conn_to.sel_expression_macon = to_connector;
	newElm.conn_to.idTrigger = iconIdTrigger;

	newElm.bridge_name = nmBridge;
	newElm.thread = thread_group;
	newElm.boost = boost_group;

	std::list<idAndBridge>::iterator elm = theBridges.begin();
	for (; elm != theBridges.end(); elm++)
	{
		if (elm->bridge_name == newElm.bridge_name)
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}
	}

	theBridges.push_back(newElm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::rem_bridge(jvxSize idCnt)
{
	if (idCnt < theBridges.size())
	{
		std::list<idAndBridge>::iterator elm = theBridges.begin();
		std::advance(elm, idCnt);
		theBridges.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnectionRule::rem_all_bridges()
{
	theBridges.clear();
	return JVX_NO_ERROR;
}

// ==================================================================================================================
// ==================================================================================================================
// ==================================================================================================================

jvxErrorType
CjvxDataConnectionRule::get_master(jvxComponentIdentification* cpId, jvxApiString* selectionExpressionFac, jvxApiString* selectionExpressionMaster)
{
	if (cpId)
		*cpId = theMaster.idCp;

	if (selectionExpressionFac)
		selectionExpressionFac->assign(theMaster.sel_expression_fac);

	if (selectionExpressionMaster)
		selectionExpressionMaster->assign(theMaster.sel_expression_macon);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::number_bridges(jvxSize* num)
{
	if (num)
		*num = theBridges.size();
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::get_bridge(jvxSize idx, jvxApiString* nmBridge, jvxComponentIdentification* cp_id_from, jvxApiString* from_factory, jvxApiString* from_conn,
	jvxComponentIdentification* cp_id_to, jvxApiString* to_factory, jvxApiString* to_conn)
{
	jvxSize cnt = 0;
	if (idx < theBridges.size())
	{
		std::list<idAndBridge>::iterator elm = theBridges.begin();
		std::advance(elm, idx);

		if (nmBridge)
		{
			nmBridge->assign(elm->bridge_name);
		}

		if (cp_id_from)
		{
			*cp_id_from = elm->conn_from.idCp;
		}

		if (from_factory)
		{
			from_factory->assign(elm->conn_from.sel_expression_fac);
		}

		if (from_conn)
		{
			from_conn->assign(elm->conn_from.sel_expression_macon);
		}

		// ==================================================================

		if (cp_id_to)
		{
			*cp_id_to = elm->conn_to.idCp;
		}

		if (to_factory)
		{
			to_factory->assign(elm->conn_to.sel_expression_fac);
		}

		if (to_conn)
		{
			to_conn->assign(elm->conn_to.sel_expression_macon);
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

// ==================================================================================

jvxErrorType
CjvxDataConnectionRule::status(jvxSize* numMatched)
{
	return JVX_ERROR_UNSUPPORTED;
}

// ==================================================================================

jvxErrorType
CjvxDataConnectionRule::try_connect_direct(
	IjvxDataConnections* allConnections, 
	IjvxHost* theHost, 
	jvxBool* connection_established, 
	IjvxReport* rep, 
	jvxSize* proc_id,
	jvxBool* report_automation,
	jvxSize ruleId)
{
	jvxSize i, j, k, l, m, n;
	jvxErrorType resL = JVX_NO_ERROR;

	IjvxConnectionMasterFactory* theMF = NULL;
	IjvxConnectionMaster* theCM = NULL;

	jvxSize uIdMF = JVX_SIZE_UNSELECTED;
	IjvxConnectionMaster* theM = NULL;
	IjvxConnectorFactory* theConnFac = NULL;

	IjvxConnectorFactory* theCF = NULL;
	jvxSize uIdCF = JVX_SIZE_UNSELECTED;

	jvxSize checkFailed = false;
	jvxApiString errmess;

	jvxSize numMF = 0;
	jvxSize numCM = 0;
	jvxComponentIdentification ptTpMa;
	
	jvxApiString strMF;
	jvxApiString strCM;

	CjvxConnectionDescription theNewConnection;

	if (connection_established)
		*connection_established = false;

	if (report_automation)
		*report_automation = params_init.report_automation;

	if (params_init.dbg_output)
	{
		JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep)
		JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Running Auto-connect-rule <" << my_rule_name << ">." << std::flush;
		JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
	}
	if (params_init.connect_process)
	{
		if (params_init.dbg_output)
		{
			JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
			JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to connect a Process." << std::flush;
			JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
		}

		if (params_init.dbg_output)
		{
			JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
			JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to activate master, component identification = <" <<
				jvxComponentIdentification_txt(theMaster.idCp) << ",  master factory <" << theMaster.sel_expression_fac << ">" <<
				", master <" << theMaster.sel_expression_macon << ">." << std::flush;
			JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
		}

		allConnections->number_connection_master_factories(&numMF);
		for (i = 0; i < numMF; i++)
		{
			resL = allConnections->reference_connection_master_factory(i, &theMF, NULL);
			assert(resL == JVX_NO_ERROR);

			// =========================================================
			jvx_request_interfaceToObject(theMF, NULL, &ptTpMa, &strMF, NULL);
			assert(resL == JVX_NO_ERROR);

			if (
				(
				((ptTpMa.tp == theMaster.idCp.tp) || (theMaster.idCp.tp == JVX_COMPONENT_UNKNOWN)) &&
					((ptTpMa.slotid == theMaster.idCp.slotid) || (theMaster.idCp.slotid == JVX_SIZE_DONTCARE)) &&
					((ptTpMa.slotsubid == theMaster.idCp.slotsubid) || (theMaster.idCp.slotsubid == JVX_SIZE_DONTCARE))))
			{
				if (params_init.dbg_output)
				{
					JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
					JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying master factory <" <<
						theMaster.sel_expression_fac << "> vs. <" <<
						strMF.std_str() << ">." << std::flush;
					JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
				}
				if (jvx_compareStringsWildcard(theMaster.sel_expression_fac, strMF.std_str()))
				{
					theNewConnection.master_factory_name = strMF.std_str();
					theNewConnection.master_factory_tp = ptTpMa;

					theMF->number_connector_masters(&numCM);
					for (j = 0; j < numCM; j++)
					{
						resL = theMF->reference_connector_master(j, &theCM);
						assert(resL == JVX_NO_ERROR);
						theCM->descriptor_master(&strCM);

						if (params_init.dbg_output)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying master <" <<
								theMaster.sel_expression_macon << "> vs. <" <<
								strCM.std_str() << ">." << std::flush;
							JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
						}
						if (jvx_compareStringsWildcard(theMaster.sel_expression_macon, strCM.std_str()))
						{
							IjvxDataConnectionProcess* proc = NULL;
							theCM->associated_process(&proc);

							if (proc == NULL)
							{
								theNewConnection.master_name = strCM.std_str();

								if (params_init.dbg_output)
								{
									JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
									JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Found master, component identification = <" <<
										jvxComponentIdentification_txt(theNewConnection.master_factory_tp) << 
										", master factory <" << theNewConnection.master_factory_name << ">" <<
										", master <" << theNewConnection.master_name << ">." << std::flush;
									JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
								}

								jvxBool bridgesOk = true;
								bridgesOk = _try_auto_connect_bridge_part(allConnections, theNewConnection, theHost, rep);

								if (bridgesOk)
								{
									JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
									theNewConnection.process_name = this->my_rule_name;
									theNewConnection.process_description = this->my_object_descr;

									if (params_init.dbg_output)
									{
										JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
										JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": +++ Trying to create process/group for rule <" <<
											theNewConnection.process_name << ">." <<
											std::flush;
										JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
									}

									resL = theNewConnection.create_process_group_from_description(
										params_init.connect_process, 
										allConnections, 
										ruleId, // The rule id is the uid of the rule as stored by the dataProcesses struct. We may specify our own ruleId if we run a rule manually
										params_init.preventStoreConfig,
										params_init.interceptors, 
										params_init.essential, 
										proc_id,
										params_init.dbg_output,
										params_init.report_automation, catId
										JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
									// JVX_CONNECTION_FEEDBACK_CALL_A_NULL
									// JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
									if (resL == JVX_NO_ERROR)
									{
										// theNewConnection.mark_from_rule();
										if (connection_established)
										{
											/* The cat id must be set before the connect happens since new connected processes
											 * are reported in a sync callback. 
											 * OLD CODE::
											if (JVX_CHECK_SIZE_SELECTED(catId))
											{
												IjvxDataConnectionProcess* proc = nullptr;
												allConnections->reference_connection_process_uid(
													*proc_id, &proc);
												if (proc)
												{
													proc->set_category_id(catId);
													allConnections->return_reference_connection_process(proc);
												}
											}
											* ::
											*/
											if (params_init.dbg_output)
											{
												JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
												JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "+++ +++ Autoconnect created new connection <" <<
													theNewConnection.process_name << ">" << std::flush;
												JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
											}

											*connection_established = true;
										}
									}
									else
									{
										if (params_init.dbg_output)
										{
											jvxBool err = false;
											JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
											JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "Failed to autoconnect, reason: " <<
												JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING_NEXT_ERROR(fdb_loc, err) << std::flush;
											JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
										}
									}
								}
								else
								{
									if (params_init.dbg_output)
									{
										JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
										JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No process/group for rule <" <<
											theNewConnection.process_name << "> created." <<
											std::flush;
										JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
									}
								}
								theNewConnection.connectors.clear();
								theNewConnection.master_factory_name.clear();
								theNewConnection.master_factory_tp = JVX_COMPONENT_UNKNOWN;
								theNewConnection.master_name.clear();

							} // if (proc == NULL)
							else
							{
								if (params_init.dbg_output)
								{
									jvxApiString strProc;
									proc->descriptor(&strProc);

									JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
									JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Master is already in use in connection with descriptor <" << strProc.std_str() << ">." << std::flush;
									JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
								}
							}
						}// if (jvx_compareStringsWildcard(theMaster.sel_expression_macon, strCM.std_str()))
						else
						{
							if (params_init.dbg_output)
							{
								JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
								JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
								JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
							}
						}
						theMF->return_reference_connector_master(theCM);
						theCM = NULL;
					}
				}
				else
				{
					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}
				}
			}
			allConnections->return_reference_connection_master_factory(theMF);
			theMF = NULL;
		}
	}
	else
	{
		if (params_init.dbg_output)
		{
			JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
			JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Connect a Group." << std::flush;
			JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
		}

		jvxBool bridgesOk = true;
		bridgesOk = _try_auto_connect_bridge_part(allConnections, theNewConnection, theHost, rep);

		if (bridgesOk)
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
			theNewConnection.process_name = this->my_rule_name;
			theNewConnection.process_description = this->my_object_descr;

			if (params_init.dbg_output)
			{
				JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
				JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to create process/group for rule <" <<
					theNewConnection.process_name << ">." <<
					std::flush;
				JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
			}

			resL = theNewConnection.create_process_group_from_description(
				params_init.connect_process, 
				allConnections, ruleId, 
				params_init.preventStoreConfig,			
				params_init.interceptors, 
				params_init.essential, 
				proc_id,
				params_init.dbg_output,
				params_init.report_automation,
				catId
				JVX_CONNECTION_FEEDBACK_CALL_A_NULL);
			//JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));
			if (resL == JVX_NO_ERROR)
			{
				// theNewConnection.mark_from_rule();
				if (connection_established)
				{
					JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
					JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "+++ +++ Autoconnect created new connection <" <<
						theNewConnection.process_name << ">" << std::flush;
					JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					*connection_established = true;
				}
			}
			else
			{
				JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
				JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "Failed to autoconnect, reason: " <<
					JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING(fdb_loc) << std::flush;
				JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
			}
		}
		else
		{
			if (params_init.dbg_output)
			{
				JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
				std::cout << "-->" << __FUNCTION__ << ": No process/group for rule <" <<
					theNewConnection.process_name << "> created." <<
					std::endl;
				JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
			}
		}
	}

	return JVX_NO_ERROR;
};

jvxBool
CjvxDataConnectionRule::_try_auto_connect_bridge_part(IjvxDataConnections* allConnections, CjvxConnectionDescription& theNewConnection, IjvxHost* theHost, IjvxReport* rep)
{
	jvxSize i, j, k, l, m, n;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize numCF = 0;
	jvxApiString strCF_F;
	IjvxConnectorFactory* theCF_F = NULL;

	jvxSize numOC_F = 0;
	IjvxOutputConnectorSelect* theOcon_F = NULL;
	jvxApiString strOc_F;

	jvxComponentIdentification ptTp_F;
	jvxComponentIdentification tpCompare;

	jvxBool bridgesOk = true;

	if (theBridges.size())
	{
		CjvxDescriptBridge theBridge;
		allConnections->number_connection_factories(&numCF);

		std::list<idAndBridge>::iterator elmB = theBridges.begin();
		for (; elmB != theBridges.end(); elmB++)
		{
			if (params_init.dbg_output)
			{
				JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
				JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to connect bridge <" << elmB->bridge_name << ">." << std::flush;
				JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
			}
			//elmB->bridge_name
			jvxBool bridgeCreated = false;
			jvxBool run_shortcut = false;

			if (!params_init.connect_process)
			{
				if (elmB->conn_from.idCp.tp == JVX_COMPONENT_UNKNOWN)
				{
					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Output Connector is an entry port." << std::flush;
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}
					run_shortcut = true;
				}
			}
			if (run_shortcut)
			{
				theBridge.from.uid = JVX_SIZE_UNSELECTED;

				bridgeCreated = _try_auto_connect_bridge_part_finalize(
					allConnections,
					theNewConnection,
					theHost,
					elmB,
					theBridge,
					rep);
				if (bridgeCreated)
				{
					theBridge.boost_thread = false;
					if (elmB->boost)
					{
						theBridge.boost_thread = true;
					}
					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Boost thread is " << std::flush;
						if (theBridge.boost_thread)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "ON." << std::flush;
						}
						else
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "OFF." << std::flush;
						}
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					theBridge.ded_thread = false;
					if(elmB->thread)
					{
						theBridge.ded_thread = true;
					}

					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Dedicated thread is " << std::flush;
						if (theBridge.ded_thread)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "ON." << std::flush;
						}
						else
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "OFF." << std::flush;
						}
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": +++ Adding the bridge to checked list." << std::flush;
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					theNewConnection.connectors.push_back(theBridge);
				}
				else
				{
					bridgesOk = false;
					break;
				}
			}
			else
			{
				tpCompare = elmB->conn_from.idCp;
				if(params_init.dbg_output)
				{
					JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
					JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to activate output connector, component identification = <" <<
						jvxComponentIdentification_txt(tpCompare) << ">,  connector factory <" << elmB->conn_from.sel_expression_fac << ">" <<
						", connector <" << elmB->conn_from.sel_expression_macon << ">." << std::flush;
					JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
				}
				theBridge.from.uid = 0; // Need to reset this value to prevent unconnected bridges in successive iterations
				for (k = 0; k < numCF; k++)
				{
					resL = allConnections->reference_connection_factory(k, &theCF_F, NULL);
					assert(resL == JVX_NO_ERROR);

					jvx_request_interfaceToObject(theCF_F, NULL, &ptTp_F, &strCF_F, NULL);
					assert(resL == JVX_NO_ERROR);

					resL = jvx_match_slots(theHost, tpCompare, theNewConnection.master_factory_tp);

					if (resL == JVX_NO_ERROR)
					{
						if (
							(
							((ptTp_F.tp == tpCompare.tp) || (tpCompare.tp == JVX_COMPONENT_UNKNOWN)) &&
								((ptTp_F.slotid == tpCompare.slotid) || (tpCompare.slotid == JVX_SIZE_DONTCARE)) &&
								((ptTp_F.slotsubid == tpCompare.slotsubid) || (tpCompare.slotsubid == JVX_SIZE_DONTCARE))))
						{
							if (params_init.dbg_output)
							{
								JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
								JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying connector factory <" <<
									elmB->conn_from.sel_expression_fac << "> vs. <" <<
									strCF_F.std_str() << ">." << std::flush;
								JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
							}

							if (jvx_compareStringsWildcard(elmB->conn_from.sel_expression_fac, strCF_F.std_str()))
							{
								theBridge.from.connector_factory_name = strCF_F.std_str();
								theBridge.from.tp = tpCompare;
								theBridge.from.oconTriggerId = elmB->conn_from.idTrigger;

								resL = theCF_F->number_output_connectors(&numOC_F);
								assert(resL == JVX_NO_ERROR);
								for (l = 0; l < numOC_F; l++)
								{
									theCF_F->reference_output_connector(l, &theOcon_F);
									assert(resL == JVX_NO_ERROR);

									theOcon_F->descriptor_connector(&strOc_F);

									if (params_init.dbg_output)
									{
										JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
										JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying connector <" <<
											elmB->conn_from.sel_expression_macon << "> vs. <" <<
											strOc_F.std_str() << ">." << std::flush;
										JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
									}
									if (jvx_compareStringsWildcard(elmB->conn_from.sel_expression_macon, strOc_F.std_str()))
									{
										IjvxDataConnectionCommon* ref = NULL;
										IjvxOutputConnector* ocon = theOcon_F->reference_ocon();
										if (ocon)
										{
											if (ocon->available_to_connect_ocon() == JVX_NO_ERROR)
											{
												theBridge.from.connector_name = strOc_F.std_str();

												if (params_init.dbg_output)
												{
													JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep)

														JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Found output connector, component identification = <" <<
														jvxComponentIdentification_txt(theBridge.from.tp) <<
														">, connector factory <" << theBridge.from.connector_factory_name << ">" <<
														", connector <" << theBridge.from.connector_name << ">." << std::flush;

													JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
												}

												bridgeCreated = _try_auto_connect_bridge_part_finalize(
													allConnections,
													theNewConnection,
													theHost,
													elmB,
													theBridge,
													rep);
											} // if (ocon->available_to_connect_ocon() == JVX_NO_ERROR)
											else
											{
												if (params_init.dbg_output)
												{
													JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
													JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Connector not available." << std::flush;
													JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
												}
											}
										} // if (ocon)
									} // if (jvx_compareStringsWildcard(elmB->conn_from.sel_expression_macon, strOc_F.std_str()))
									else
									{
										if (params_init.dbg_output)
										{
											JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
											JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
											JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
										}
									}
									theCF_F->return_reference_output_connector(theOcon_F);
									theOcon_F = NULL;
									if (bridgeCreated)
									{
										break;
									}
								} // for (m = 0; m < numCF; m++)
							} // if (jvx_compareStringsWildcard(elmB->conn_from.sel_expression_fac, strCF_F.std_str()))
							else
							{
								if (params_init.dbg_output)
								{
									JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
									JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
									JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
								}
							}
						} // if ((((ptTp_F.tp == tpCompare.tp) || (tpCompare.tp == JVX_COMPONENT_UNKNOWN)) ......)
					} // if (resL == JVX_NO_ERROR)

					allConnections->return_reference_connection_factory(theCF_F);
					theCF_F = NULL;

					if (bridgeCreated)
					{
						break;
					}
				}// for (k = 0; k < numCF; k++)

				if (bridgeCreated)
				{
					theBridge.boost_thread = false;
					if (elmB->boost)
					{
						theBridge.boost_thread = true;
					}

					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Boost thread is " << std::flush;
						if (theBridge.boost_thread)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "ON." << std::flush;
						}
						else
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "OFF." << std::flush;
						}
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					theBridge.ded_thread = false;
					if (elmB->thread)
					{
						theBridge.ded_thread = true;
					}

					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Dedicated thread is " << std::flush;
						if (theBridge.ded_thread)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "ON." << std::flush;
						}
						else
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "OFF." << std::flush;
						}
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "--> " << __FUNCTION__ << ": Adding the bridge to checked list." << std::flush;
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}
					theNewConnection.connectors.push_back(theBridge);
				}
				else
				{
					bridgesOk = false;
					break;
				}
			}// if connectProcess
		} // Bridge loop: for (; elmB != theBridges.end(); elmB++)
	}
	return bridgesOk;
}

jvxBool
CjvxDataConnectionRule::_try_auto_connect_bridge_part_finalize(IjvxDataConnections* allConnections,
	CjvxConnectionDescription& theNewConnection, 
	IjvxHost* theHost,
	std::list<idAndBridge>::iterator& elmB,
	CjvxDescriptBridge& theBridge, IjvxReport* rep)
{
	jvxSize m,n;
	jvxBool bridgeCreated = false;
	jvxSize numCF = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxApiString strCF_T;
	IjvxConnectorFactory* theCF_T = NULL;

	jvxSize numIC_T = 0;
	IjvxInputConnectorSelect* theIcon_T = NULL;
	jvxApiString strIc_T;
	jvxComponentIdentification ptTp_T;
	jvxComponentIdentification tpCompare;

	// ==============================================================
	// Here, check output bridge part
	// ==============================================================
	jvxBool run_shortcut = false;

	if (!params_init.connect_process)
	{
		if (elmB->conn_to.idCp.tp == JVX_COMPONENT_UNKNOWN)
		{
			run_shortcut = true;
		}
	}
	if (run_shortcut)
	{
		theBridge.to.uid = JVX_SIZE_UNSELECTED;
		theBridge.bridge_name = elmB->bridge_name;

		// Positive for this bridge
		bridgeCreated = true;
	}
	else
	{
		tpCompare = elmB->conn_to.idCp;
		if (params_init.dbg_output)
		{
			JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
			JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying to activate input connector, component identification = <" <<
				jvxComponentIdentification_txt(tpCompare) << ">,  connector factory <" << elmB->conn_to.sel_expression_fac << ">" <<
				", connector <" << elmB->conn_to.sel_expression_macon << ">." << std::flush;
			JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
		}

		theBridge.to.uid = 0; // Need to reset this value to prevent unconnected bridges in successive iterations
		allConnections->number_connection_factories(&numCF);
		for (m = 0; m < numCF; m++)
		{
			resL = allConnections->reference_connection_factory(m, &theCF_T, NULL);
			assert(resL == JVX_NO_ERROR);

			jvx_request_interfaceToObject(theCF_T, NULL, &ptTp_T, &strCF_T, NULL);
			assert(resL == JVX_NO_ERROR);

			resL = jvx_match_slots(theHost, tpCompare, theNewConnection.master_factory_tp);

			if (resL == JVX_NO_ERROR)
			{
				if (
					(
					((ptTp_T.tp == tpCompare.tp) || (tpCompare.tp == JVX_COMPONENT_UNKNOWN)) &&
						((ptTp_T.slotid == tpCompare.slotid) || (tpCompare.slotid == JVX_SIZE_DONTCARE)) &&
						((ptTp_T.slotsubid == tpCompare.slotsubid) || (tpCompare.slotsubid == JVX_SIZE_DONTCARE))))
				{
					if (params_init.dbg_output)
					{
						JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
						JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying connector factory <" <<
							elmB->conn_to.sel_expression_fac << "> vs. <" <<
							strCF_T.std_str() << ">." << std::flush;
						JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
					}

					if (jvx_compareStringsWildcard(elmB->conn_to.sel_expression_fac, strCF_T.std_str()))
					{

						theBridge.to.connector_factory_name = strCF_T.std_str();
						theBridge.to.tp = tpCompare;
						theBridge.to.iconTriggerId = elmB->conn_to.idTrigger;

						resL = theCF_T->number_input_connectors(&numIC_T);
						assert(resL == JVX_NO_ERROR);
						for (n = 0; n < numIC_T; n++)
						{
							theCF_T->reference_input_connector(n, &theIcon_T);
							assert(resL == JVX_NO_ERROR);
							theIcon_T->descriptor_connector(&strIc_T);

							if (params_init.dbg_output)
							{
								JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
								JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Trying connector <" <<
									elmB->conn_to.sel_expression_macon << "> vs. <" << 
									strIc_T.std_str() << ">." << std::flush;
								JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
							}

							if (jvx_compareStringsWildcard(elmB->conn_to.sel_expression_macon,
								strIc_T.std_str()))
							{
								IjvxDataConnectionCommon* ref = NULL;
								IjvxInputConnector* icon = theIcon_T->reference_icon();
								if (icon && (icon->available_to_connect_icon() == JVX_NO_ERROR))
								{
									theBridge.to.connector_name = strIc_T.std_str();
									theBridge.bridge_name = elmB->bridge_name;

									if (params_init.dbg_output)
									{
										JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
										JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Found input connector, component identification = <" <<
											jvxComponentIdentification_txt(theBridge.to.tp) <<
											">, connector factory <" << theBridge.to.connector_factory_name << ">" <<
											", connector <" << theBridge.to.connector_name << ">." << std::flush;
										JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
									}
									// Positive for this bridge
									bridgeCreated = true;
								}
								else
								{
									JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
									JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": Found input connector, component identification = <" <<
										jvxComponentIdentification_txt(theBridge.to.tp) <<
										">, connector factory <" << theBridge.to.connector_factory_name << ">" <<
										", connector <" << theBridge.to.connector_name << ">, but connector is already in use." << std::flush;
									JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
								}
							}
							else
							{
								if (params_init.dbg_output)
								{
									JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
									JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
									JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
								}
							}
							theCF_T->return_reference_input_connector(theIcon_T);
							theIcon_T = NULL;
							if (bridgeCreated)
							{
								break;
							}
						} // for (n = 0; n < numIC_T; n++)
					} // if (jvx_compareStringsWildcard(elmB->conn_to.sel_expression_fac, strCF_T.std_str()))
					else
					{
						if (params_init.dbg_output)
						{
							JVX_OUTPUT_REPORT_DEBUG_COUT_START(rep);
							JVX_OUTPUT_REPORT_DEBUG_COUT_REF << "-->" << __FUNCTION__ << ": No Match." << std::flush;
							JVX_OUTPUT_REPORT_DEBUG_COUT_STOP(rep);
						}
					}
				} // if ((((ptTp_T.tp == tpCompare.tp) || (tpCompare.tp == JVX_COMPONENT_UNKNOWN)) ...)
			} // if (resL == JVX_NO_ERROR)
			resL = allConnections->return_reference_connection_factory(theCF_T);
			theCF_T = NULL;

			if (bridgeCreated)
			{
				break;
			}
		} // for (m = 0; m < numCF; m++)
	}
	return bridgeCreated;
}
/*
jvxErrorType _try_auto_disconnect(IjvxDataConnections* allConnections, IjvxConnectorFactory* rem_this, IjvxConnectionMasterFactory* and_this)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool runitagain = true;
	while (runitagain)
	{
		runitagain = false;
		std::list<oneStartedConnection>::iterator elm = startedConnections.begin();
		for (; elm != startedConnections.end(); elm++)
		{
			jvxBool constraintDeactivate = false;

			if (elm->theMasterFac == and_this)
			{
				constraintDeactivate = true;
			}

			if (elm->theConnFac == rem_this)
			{
				constraintDeactivate = true;
			}

			std::map<jvxSize, oneSlaveConnection>::iterator elmS = elm->lstSlaveConnectors.begin();
			for (; elmS != elm->lstSlaveConnectors.end(); elmS++)
			{
				if (rem_this == elmS->second.theConnFac)
				{
					constraintDeactivate = true;
					break;
				}
			}

			if (constraintDeactivate)
			{
				stop_process(allConnections, &elm->theProc);

				elm->lstBridgeContainers.clear();

				res = allConnections->remove_connection_process(elm->theProc_id);
				assert(res == JVX_NO_ERROR);

				elm->theProc_id = JVX_SIZE_UNSELECTED;

				// ===================================================
				// Cleanup the stored set for processing
				// ===================================================
				cleanup_containers(allConnections, *elm);
				startedConnections.erase(elm);
				runitagain = true;
				break;
			}
		}
	}
	return JVX_NO_ERROR;
};
*/

jvxErrorType cleanup_containers(IjvxDataConnections* allConnections, oneStartedConnection& theContainer)
{
	jvxErrorType resL = JVX_NO_ERROR;
	assert(0);
	return resL;
}

// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 
// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 
// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

jvxErrorType
CjvxDataConnectionRule::get_master_factory(IjvxDataConnections* allConnections, IjvxConnectionMasterFactory** theMF,
	jvxSize* uIdMF, jvxComponentIdentification& idTarget)
{
	IjvxConnectionMasterFactory* theFactory = NULL;
	jvxSize numMF = 0;
	jvxSize i;
	jvxComponentIdentification tpId;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxApiString strMF;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	assert(theMF != NULL);
	assert(uIdMF != NULL);

	*theMF = NULL;
	*uIdMF = JVX_SIZE_UNSELECTED;

	// Browse all master factories
	allConnections->number_connection_master_factories(&numMF);
	for (i = 0; i < numMF; i++)
	{
		// Temporary hold pointer to master factory
		res = allConnections->reference_connection_master_factory(i, &theFactory, &uId);

		assert(res == JVX_NO_ERROR);

		// Find component identification and name
		jvx_request_interfaceToObject(theFactory, NULL, &tpId, &strMF, NULL);

		// Check the correct component identification
		if (tpId.tp == idTarget.tp)
		{
			if ((tpId.slotid == idTarget.slotid) || (idTarget.slotid == JVX_SIZE_DONTCARE))
			{
				if ((tpId.slotsubid == idTarget.slotsubid) || (idTarget.slotsubid == JVX_SIZE_DONTCARE))
				{
					*theMF = theFactory;
					*uIdMF = uId;
					return JVX_NO_ERROR;
				}
			}
		}
		else
		{
			allConnections->return_reference_connection_master_factory(theFactory);
		}
	}
	return res;
}

jvxErrorType
CjvxDataConnectionRule::release_master_factory(IjvxDataConnections* allConnections, IjvxConnectionMasterFactory* theMF)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(allConnections);
	if (theMF)
	{
		res = allConnections->return_reference_connection_master_factory(theMF);
		return res;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

jvxErrorType
CjvxDataConnectionRule::get_connector_factory(IjvxDataConnections* allConnections, IjvxConnectorFactory** theCF,
	jvxSize* uIdCF, jvxComponentIdentification& idTarget)
{
	IjvxConnectorFactory* theFactory = NULL;
	jvxSize numCF = 0;
	jvxSize i;
	jvxComponentIdentification tpId;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxApiString strMF;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	assert(theCF != NULL);
	assert(uIdCF != NULL);

	*theCF = NULL;
	*uIdCF = JVX_SIZE_UNSELECTED;

	// Browse all master factories
	allConnections->number_connection_factories(&numCF);
	for (i = 0; i < numCF; i++)
	{
		// Temporary hold pointer to master factory
		res = allConnections->reference_connection_factory(i, &theFactory, &uId);

		assert(res == JVX_NO_ERROR);

		// Find component identification and name
		jvx_request_interfaceToObject(theFactory, NULL, &tpId, &strMF, NULL);

		// Check the correct component identification
		if (tpId.tp == idTarget.tp)
		{
			if ((tpId.slotid == idTarget.slotid) || (idTarget.slotid == JVX_SIZE_DONTCARE))
			{
				if ((tpId.slotsubid == idTarget.slotsubid) || (idTarget.slotsubid == JVX_SIZE_DONTCARE))
				{
					*theCF = theFactory;
					*uIdCF = uId;
					return JVX_NO_ERROR;
				}
			}
		}
		else
		{
			allConnections->return_reference_connection_factory(theFactory);
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnectionRule::release_connector_factory(IjvxDataConnections* allConnections, IjvxConnectorFactory* theCF)
{
	jvxErrorType res = JVX_NO_ERROR;
	assert(allConnections);
	if (theCF)
	{
		res = allConnections->return_reference_connection_factory(theCF);
		return res;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

jvxErrorType
CjvxDataConnectionRule::get_master(IjvxConnectionMasterFactory* theFactory, IjvxConnectionMaster** theM, const std::string & expression)
{
	jvxSize numMI = 0;
	jvxSize j;
	IjvxConnectionMaster* theMaster = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool isAvail = false;
	jvxApiString strMI;

	assert(theM != NULL);

	*theM = NULL;

	theFactory->number_connector_masters(&numMI);
	for (j = 0; j < numMI; j++)
	{
		res = theFactory->reference_connector_master(j, &theMaster);
		assert(res == JVX_NO_ERROR);
		assert(theMaster);

		isAvail = false;

		// Chek if master is available
		theMaster->available_master(&isAvail);

		if (isAvail)
		{
			res = theMaster->name_master(&strMI);
			assert(res == JVX_NO_ERROR);
			if (jvx_compareStringsWildcard(expression, strMI.std_str()))
			{
				*theM = theMaster;
				return JVX_NO_ERROR;
			}
		}
		res = theFactory->return_reference_connector_master(theMaster);
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxDataConnectionRule::release_master(IjvxConnectionMasterFactory* theFactory, IjvxConnectionMaster* theM)
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(theFactory);

	if (theM)
	{
		res = theFactory->return_reference_connector_master(theM);
		return res;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + +  + + 

jvxErrorType
CjvxDataConnectionRule::get_configuration(IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section, jvxFlagTag flagtag)
{
	jvxConfigData* datTmp = NULL;
	jvxConfigData* datBridge = NULL;
	jvxSize cnt;
	jvxValue val;
	std::string prefix = "JVX_CONNECTION_RULE_MASTER";

	theWriter->createAssignmentString(&datTmp, "JVX_CONNECTION_RULE_NAME", this->my_rule_name.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentValue(&datTmp, "PROCESS_INTERCEPTORS", params_init.interceptors);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentValue(&datTmp, "PROCESS_ESSENTIAL", params_init.essential);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentString(&datTmp, (prefix + "COMPONENT_TYPE").c_str(), jvxComponentType_txt(this->theMaster.idCp.tp));
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	val.assign((jvxSize)this->theMaster.idCp.slotid);
	theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_ID").c_str(), val);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	val.assign((jvxSize)this->theMaster.idCp.slotsubid);
	theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_SUB_ID").c_str(), val);
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentString(&datTmp, (prefix + "FACTORY_WILDCARD").c_str(), this->theMaster.sel_expression_fac.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	theWriter->createAssignmentString(&datTmp, (prefix + "MASTER_WILDCARD").c_str(), this->theMaster.sel_expression_macon.c_str());
	theWriter->addSubsectionToSection(add_to_this_section, datTmp);
	datTmp = NULL;

	cnt = 0;
	std::list<idAndBridge>::iterator elm = theBridges.begin();
	for (; elm != theBridges.end(); elm++)
	{
		theWriter->createEmptySection(&datBridge, ("CONNECTION_RULE_BRIDE_ID_" + jvx_size2String(cnt)).c_str());

		// ===================================================================================

		// Return config from single bridge
		theWriter->createAssignmentString(&datTmp, " JVX_CONNECTION_RULE_BRIDGE_NAME", elm->bridge_name.c_str());
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		prefix = "CONNECTOR_FROM";

		// Return config from single bridge
		theWriter->createAssignmentString(&datTmp, (prefix + "COMPONENT_TYPE").c_str(), jvxComponentType_txt(elm->conn_from.idCp.tp));
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		val.assign((jvxSize)elm->conn_from.idCp.slotid);
		theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_ID").c_str(), val);
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		val.assign((jvxSize)elm->conn_from.idCp.slotsubid);
		theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_SUB_ID").c_str(), val);
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		theWriter->createAssignmentString(&datTmp, (prefix + "FACTORY_WILDCARD").c_str(), elm->conn_from.sel_expression_fac.c_str());
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		theWriter->createAssignmentString(&datTmp, (prefix + "CONNECTOR_WILDCARD").c_str(), elm->conn_from.sel_expression_macon.c_str());
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		// ==============================================================================================

		prefix = "CONNECTOR_TO";

		// Return config from single bridge
		theWriter->createAssignmentString(&datTmp, (prefix + "COMPONENT_TYPE").c_str(), jvxComponentType_txt(elm->conn_to.idCp.tp));
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		val.assign((jvxSize)elm->conn_to.idCp.slotid);
		theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_ID").c_str(), val);
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		val.assign((jvxSize)elm->conn_to.idCp.slotsubid);
		theWriter->createAssignmentValue(&datTmp, (prefix + "SLOT_SUB_ID").c_str(), val);
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		theWriter->createAssignmentString(&datTmp, (prefix + "FACTORY_WILDCARD").c_str(), elm->conn_to.sel_expression_fac.c_str());
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		theWriter->createAssignmentString(&datTmp, (prefix + "CONNECTOR_WILDCARD").c_str(), elm->conn_to.sel_expression_macon.c_str());
		theWriter->addSubsectionToSection(datBridge, datTmp);
		datTmp = NULL;

		// ==============================================================================================

		theWriter->addSubsectionToSection(add_to_this_section, datBridge);
		cnt++;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::update_connected(jvxBool added)
{
	if (added)
	{
		connectedCnt++;
	}
	else
	{
		connectedCnt--;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionRule::ready_for_start(jvxApiString* reason_if_not)
{
	if (connectedCnt < params_init.minNumConnectionsReady)
	{
		std::string txt = "Connection rule <" + this->my_rule_name + ">: ";
		txt += "Minimum of <" + jvx_size2String(params_init.minNumConnectionsReady) + "> connections required to start";
		reason_if_not->assign(txt);
		return JVX_ERROR_NOT_READY;
	}
	return JVX_NO_ERROR;
}
