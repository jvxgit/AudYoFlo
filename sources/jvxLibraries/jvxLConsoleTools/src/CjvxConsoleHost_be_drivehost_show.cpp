#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"

#include <iostream>
#include <sstream>
// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxConsoleHost_be_drivehost::show_current_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst_ret)
{
	jvxErrorType res = JVX_NO_ERROR;
	CjvxJsonElement jelm_result;

	jvxBool produce_sil = false;
	jvxBool produce_numdigs = false;
	jvxBool produce_timeout = false;
	jvxBool produce_propb64 = false;
	jvxBool produce_propcompact = false;

	if (dh_command.family == JVX_DRIVEHOST_FAMILY_SHOW)
	{
		if (dh_command.address == JVX_DRIVEHOST_ADRESS_CONFIG)
		{
			if (args.size() <= 1)
			{
				produce_sil = true;
				produce_timeout = true;
				CjvxHostJsonCommandsShow::show_current_config(dh_command, args, off, jelmlst_ret);
			}
			else if (args.size() > 1)
			{
				res = CjvxHostJsonCommandsShow::show_current_config(
					dh_command, args, off, jelmlst_ret);
				if (res == JVX_NO_ERROR)
				{
					return res;
				}
				else
				{
					jelmlst_ret.deleteAll();
					res = JVX_NO_ERROR;
				}

				if (args[1] == "silent_mode")
				{
					produce_sil = true;
				}
				else if (args[1] == "timeout_seq")
				{
					produce_timeout = true;
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
					JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, ("Argument <" + args[1] + "> is not a valid option."));
				}
			}
			if (res == JVX_NO_ERROR)
			{
				if (produce_sil)
				{
					if (config_act.silent_out)
					{
						jelm_result.makeAssignmentString("silent_mode", jvx_make_yes);
					}
					else
					{
						jelm_result.makeAssignmentString("silent_mode", jvx_make_no);
					}
					jelmlst_ret.addConsumeElement(jelm_result);
				}
				
				if (produce_timeout)
				{
					jelm_result.makeAssignmentInt("timeout_seq", JVX_SIZE_INT(config.timeout_seq_msec));
					jelmlst_ret.addConsumeElement(jelm_result);
				}
				
			}
		}
	}

	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::show_dropzone(const oneDrivehostCommand& dh_command,
	const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;
	jvxDrivehostConnectionShow showmode = JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL;

	jvxSize idS;
	jvxSize i;
	jvxApiString fldDescr;
	jvxApiString fldDescror;
	CjvxJsonElement jelm;
	CjvxJsonArray jarr;
	std::string key;
	std::string values;
	jvxCBitField pMode = 0;
	jsec.deleteAll();
	IjvxDataConnections* connections = NULL;
	jvxSize specId = JVX_SIZE_UNSELECTED;
	std::string specName;
	jvxBool err = false;;
	jvxBool arg2set = false;
	std::string errTxt;
	jvxApiString fldStr;
	jvxApiString strMF;
	CjvxJsonElementList jelmall;
	CjvxJsonElement jelml;
	jvxComponentIdentification tpId;
	IjvxConnectionMasterFactory* masFac = NULL;
	IjvxConnectionMaster* mas = NULL;
	IjvxConnectorFactory* iconFac = NULL;
	IjvxInputConnector* icon = NULL;
	IjvxConnectorFactory* oconFac = NULL;
	IjvxOutputConnector* ocon = NULL;
	IjvxConnectorFactory* conFac = NULL;
	std::string commandExt;
	std::string filterExp;

	assert(involvedHost.hHost);

	if (args.size() >= 2)
	{
		commandExt = args[1];
	}
	if (args.size() >= 3)
	{
		filterExp = args[2];
	}

	res = involvedHost.hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
	if ((res == JVX_NO_ERROR) && connections)
	{
		if (commandExt == "master_fac")
		{
			CjvxJsonArray jelmarr;
			connections->number_connection_master_factories(&num);
			for (i = 0; i < num; i++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;
				jvxSize num_cons = 0;
				jvxSize j = 0;
				connections->reference_connection_master_factory(i, &masFac, NULL);
				if (masFac)
				{
					jvx_request_interfaceToObject(masFac, NULL, &tpId, &strMF);

					jvxBool showme = false;
					if (filterExp.empty())
					{
						showme = true;
					}
					else
					{
						if (jvx_compareStringsWildcard(filterExp, strMF.std_str()))
						{
							showme = true;
						}
					}

					if (showme)
					{
						JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
						jelmarrelmll.addConsumeElement(jelmarrelml);

						JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_NAME(jelmarrelml, strMF.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						CjvxJsonArray jelmarrarr;
						masFac->number_connector_masters(&num_cons);
						for (j = 0; j < num_cons; j++)
						{
							CjvxJsonArrayElement jelmarrarrelm;
							CjvxJsonElementList jelmarrarrelmll;
							CjvxJsonElement jelmarrarrelml;
							masFac->reference_connector_master(j, &mas);
							if (mas)
							{
								mas->descriptor_master(&fldStr);
								JVX_CREATE_CONNECTION_DROPZONE_MASTER_NAME(jelmarrarrelml, fldStr.std_str());
								jelmarrarrelmll.addConsumeElement(jelmarrarrelml);

								masFac->return_reference_connector_master(mas);
								mas = NULL;

								jelmarrarrelm.makeSection(jelmarrarrelmll);
							}
							jelmarrarr.addConsumeElement(jelmarrarrelm);
						}
						jelmarrelml.makeArray("mas_connectors", jelmarrarr);
						jelmarrelmll.addConsumeElement(jelmarrelml);

						connections->return_reference_connection_master_factory(masFac);
						masFac = NULL;

						jelmarrelm.makeSection(jelmarrelmll);
						jelmarr.addConsumeElement(jelmarrelm);
					}
				}
			}
			jelm.makeArray("mas_list_dz", jelmarr);
			jsec.addConsumeElement(jelm);
		}
		else if (commandExt == "connector_fac")
		{
			CjvxJsonArray jelmarr;
			connections->number_connection_factories(&num);
			for (i = 0; i < num; i++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;
				jvxSize num_cons = 0;
				jvxSize j = 0;
				connections->reference_connection_factory(i, &conFac, NULL);
				if (conFac)
				{
					jvx_request_interfaceToObject(conFac, NULL, &tpId, &strMF);

					jvxBool showme = false;
					if (filterExp.empty())
					{
						showme = true;
					}
					else
					{
						if (jvx_compareStringsWildcard(filterExp, strMF.std_str()))
						{
							showme = true;
						}
					}

					if (showme)
					{
						JVX_CREATE_CONNECTION_DROPZONE_CONNECTOR_FACTORY_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
						jelmarrelmll.addConsumeElement(jelmarrelml);

						JVX_CREATE_CONNECTION_DROPZONE_CONNECTOR_FACTORY_NAME(jelmarrelml, strMF.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						CjvxJsonArray jelmarrarr;

						// =================================================

						conFac->number_input_connectors(&num_cons);
						for (j = 0; j < num_cons; j++)
						{
							CjvxJsonArrayElement jelmarrarrelm;
							CjvxJsonElementList jelmarrarrelmll;
							CjvxJsonElement jelmarrarrelml;
							conFac->reference_input_connector(j, &icon);
							if (icon)
							{
								icon->descriptor_connector(&fldStr);
								JVX_CREATE_CONNECTION_DROPZONE_ICON_NAME(jelmarrarrelml, fldStr.std_str());
								jelmarrarrelmll.addConsumeElement(jelmarrarrelml);

								conFac->return_reference_input_connector(icon);
								icon = NULL;

								jelmarrarrelm.makeSection(jelmarrarrelmll);
							}
							jelmarrarr.addConsumeElement(jelmarrarrelm);
						}
						jelmarrelml.makeArray("input_connectors", jelmarrarr);
						jelmarrelmll.addConsumeElement(jelmarrelml);

						// =================================================

						conFac->number_output_connectors(&num_cons);
						for (j = 0; j < num_cons; j++)
						{
							CjvxJsonArrayElement jelmarrarrelm;
							CjvxJsonElementList jelmarrarrelmll;
							CjvxJsonElement jelmarrarrelml;
							conFac->reference_output_connector(j, &ocon);
							if (ocon)
							{
								ocon->descriptor_connector(&fldStr);
								JVX_CREATE_CONNECTION_DROPZONE_OCON_NAME(jelmarrarrelml, fldStr.std_str());
								jelmarrarrelmll.addConsumeElement(jelmarrarrelml);

								conFac->return_reference_output_connector(ocon);
								ocon = NULL;

								jelmarrarrelm.makeSection(jelmarrarrelmll);

							}
							jelmarrarr.addConsumeElement(jelmarrarrelm);
						}
						jelmarrelml.makeArray("output_connectors", jelmarrarr);

						// ========================================================

						jelmarrelmll.addConsumeElement(jelmarrelml);
						connections->return_reference_connection_factory(conFac);
						conFac = NULL;

						jelmarrelm.makeSection(jelmarrelmll);
						jelmarr.addConsumeElement(jelmarrelm);
					}
				}
			}
			jelm.makeArray("con_list_dz", jelmarr);
			jsec.addConsumeElement(jelm);
		}
		else if (commandExt.empty())
		{
			// =================================================================
			// =================================================================

			JVX_CREATE_CONNECTION_DROPZONE_PROCESS_NAME(jelml, process_name);
			jelmall.addConsumeElement(jelml);

			CjvxJsonArray jelmarr;
			auto elm = lst_masters.begin();
			jvxSize cnt = 0;
			for (; elm != lst_masters.end(); elm++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;
				connections->reference_connection_master_factory_uid(elm->identify.mas_fac_uid, &masFac);
				if (masFac)
				{
					jvx_request_interfaceToObject(masFac, NULL, &tpId, &strMF);
					JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
					jelmarrelmll.addConsumeElement(jelmarrelml);

					JVX_CREATE_CONNECTION_DROPZONE_MASTER_FACTORY_NAME(jelmarrelml, strMF.std_str());
					jelmarrelmll.addConsumeElement(jelmarrelml);

					masFac->reference_connector_master(elm->identify.mas_id, &mas);
					if (mas)
					{
						IjvxDataConnectionProcess* proc = NULL;
						mas->descriptor_master(&fldStr);
						JVX_CREATE_CONNECTION_DROPZONE_MASTER_NAME(jelmarrelml, fldStr.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						mas->associated_process(&proc);
						if (proc == NULL)
						{
							JVX_CREATE_CONNECTION_DROPZONE_MASTER_AVAIL(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
						}
						else
						{
							JVX_CREATE_CONNECTION_DROPZONE_MASTER_AVAIL(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
						}
						masFac->return_reference_connector_master(mas);
						mas = NULL;
					}
					connections->return_reference_connection_master_factory(masFac);
					masFac = NULL;
				}
				if (cnt == id_select_master)
				{
					JVX_CREATE_CONNECTION_DROPZONE_MASTER_SELECTED(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				else
				{
					JVX_CREATE_CONNECTION_DROPZONE_MASTER_SELECTED(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				jelmarrelm.makeSection(jelmarrelmll);
				jelmarr.addConsumeElement(jelmarrelm);
			}
			jelml.makeArray("master", jelmarr);
			jelmall.addConsumeElement(jelml);

			// ===================================================================0

			//CjvxJsonArray jelmarr;
			auto elmi = lst_inputs.begin();
			cnt = 0;
			for (; elmi != lst_inputs.end(); elmi++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;
				connections->reference_connection_factory_uid(elmi->identify.fac_uid, &iconFac);
				if (iconFac)
				{
					jvx_request_interfaceToObject(iconFac, NULL, &tpId, &strMF);
					JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
					jelmarrelmll.addConsumeElement(jelmarrelml);

					JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_NAME(jelmarrelml, strMF.std_str());
					jelmarrelmll.addConsumeElement(jelmarrelml);

					iconFac->reference_input_connector(elmi->identify.icon_id, &icon);
					if (icon)
					{
						IjvxDataConnectionCommon* proc = NULL;
						icon->descriptor_connector(&fldStr);
						JVX_CREATE_CONNECTION_DROPZONE_ICON_NAME(jelmarrelml, fldStr.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						icon->associated_common_icon(&proc);
						if (proc == NULL)
						{
							JVX_CREATE_CONNECTION_DROPZONE_ICON_AVAIL(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
						}
						else
						{
							JVX_CREATE_CONNECTION_DROPZONE_ICON_AVAIL(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
						}

						iconFac->return_reference_input_connector(icon);
						icon = NULL;
					}
					connections->return_reference_connection_factory(iconFac);
					iconFac = NULL;
				}
				if (cnt == id_select_icon)
				{
					JVX_CREATE_CONNECTION_DROPZONE_ICON_SELECTED(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				else
				{
					JVX_CREATE_CONNECTION_DROPZONE_ICON_SELECTED(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				jelmarrelm.makeSection(jelmarrelmll);
				jelmarr.addConsumeElement(jelmarrelm);
			}
			jelml.makeArray("icons", jelmarr);
			jelmall.addConsumeElement(jelml);

			// ===================================================================0

			//CjvxJsonArray jelmarr;
			auto elmo = lst_outputs.begin();
			cnt = 0;
			for (; elmo != lst_outputs.end(); elmo++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;
				connections->reference_connection_factory_uid(elmo->identify.fac_uid, &oconFac);
				if (oconFac)
				{
					IjvxDataConnectionCommon* proc = NULL;
					jvx_request_interfaceToObject(oconFac, NULL, &tpId, &strMF);
					JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
					jelmarrelmll.addConsumeElement(jelmarrelml);

					JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_NAME(jelmarrelml, strMF.std_str());
					jelmarrelmll.addConsumeElement(jelmarrelml);

					oconFac->reference_output_connector(elmo->identify.ocon_id, &ocon);
					if (ocon)
					{
						ocon->descriptor_connector(&fldStr);
						JVX_CREATE_CONNECTION_DROPZONE_OCON_NAME(jelmarrelml, fldStr.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						ocon->associated_common_ocon(&proc);
						if (proc == NULL)
						{
							JVX_CREATE_CONNECTION_DROPZONE_OCON_AVAIL(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
						}
						else
						{
							JVX_CREATE_CONNECTION_DROPZONE_OCON_AVAIL(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
						}

						oconFac->return_reference_output_connector(ocon);
						ocon = NULL;
					}
					connections->return_reference_connection_factory(oconFac);
					oconFac = NULL;
				}
				if (cnt == id_select_ocon)
				{
					JVX_CREATE_CONNECTION_DROPZONE_OCON_SELECTED(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				else
				{
					JVX_CREATE_CONNECTION_DROPZONE_OCON_SELECTED(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				jelmarrelm.makeSection(jelmarrelmll);
				jelmarr.addConsumeElement(jelmarrelm);
			}
			jelml.makeArray("ocons", jelmarr);
			jelmall.addConsumeElement(jelml);

			// ===================================================================0

			//CjvxJsonArray jelmarr;
			auto elmb = lst_bridges.begin();
			cnt = 0;
			for (; elmb != lst_bridges.end(); elmb++)
			{
				CjvxJsonArrayElement jelmarrelm;
				CjvxJsonElementList jelmarrelmll;
				CjvxJsonElement jelmarrelml;

				JVX_CREATE_CONNECTION_DROPZONE_BRIDGE_NAME(jelmarrelml, elmb->bridge_name);
				jelmarrelmll.addConsumeElement(jelmarrelml);

				//
				connections->reference_connection_factory_uid(elmb->identify_out.fac_uid, &oconFac);
				if (oconFac)
				{
					jvx_request_interfaceToObject(oconFac, NULL, &tpId, &strMF);
					JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_FROM_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
					jelmarrelmll.addConsumeElement(jelmarrelml);

					JVX_CREATE_CONNECTION_DROPZONE_OCON_FACTORY_FROM_NAME(jelmarrelml, strMF.std_str());
					jelmarrelmll.addConsumeElement(jelmarrelml);

					oconFac->reference_output_connector(elmb->identify_out.ocon_id, &ocon);
					if (ocon)
					{
						ocon->descriptor_connector(&fldStr);
						JVX_CREATE_CONNECTION_DROPZONE_OCON_FROM_NAME(jelmarrelml, fldStr.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						oconFac->return_reference_output_connector(ocon);
						ocon = NULL;
					}
					connections->return_reference_connection_factory(oconFac);
					oconFac = NULL;
				}

				//
				connections->reference_connection_factory_uid(elmb->identify_in.fac_uid, &iconFac);
				if (iconFac)
				{
					jvx_request_interfaceToObject(iconFac, NULL, &tpId, &strMF);
					JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_FROM_IDENTITY(jelmarrelml, jvxComponentIdentification_txt(tpId));
					jelmarrelmll.addConsumeElement(jelmarrelml);

					JVX_CREATE_CONNECTION_DROPZONE_ICON_FACTORY_FROM_NAME(jelmarrelml, strMF.std_str());
					jelmarrelmll.addConsumeElement(jelmarrelml);

					iconFac->reference_input_connector(elmb->identify_in.icon_id, &icon);
					if (icon)
					{
						icon->descriptor_connector(&fldStr);
						JVX_CREATE_CONNECTION_DROPZONE_ICON_FROM_NAME(jelmarrelml, fldStr.std_str());
						jelmarrelmll.addConsumeElement(jelmarrelml);

						iconFac->return_reference_input_connector(icon);
						icon = NULL;
					}
					connections->return_reference_connection_factory(iconFac);
					iconFac = NULL;
				}
				if (cnt == id_select_bridge)
				{
					JVX_CREATE_CONNECTION_DROPZONE_BRIDGE_SELECTED(jelmarrelml, JVX_INDICATE_TRUE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				else
				{
					JVX_CREATE_CONNECTION_DROPZONE_BRIDGE_SELECTED(jelmarrelml, JVX_INDICATE_FALSE_SHORT);
					jelmarrelmll.addConsumeElement(jelmarrelml);
				}
				jelmarrelm.makeSection(jelmarrelmll);
				jelmarr.addConsumeElement(jelmarrelm);
			}
			jelml.makeArray("bridges", jelmarr);
			jelmall.addConsumeElement(jelml);

			res = involvedHost.hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));


			JVX_CREATE_CONNECTION_DROPZONE(jelm, jelmall);
			jsec.addConsumeElement(jelm);
		}
		else
		{
			JVX_CREATE_ERROR_RETURN(jsec, JVX_ERROR_INVALID_ARGUMENT, "Option <" + commandExt + "> is not valid.");
		}
	}
	return JVX_NO_ERROR;
}