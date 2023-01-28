#include "CjvxHostJsonCommandsShow.h"
#include "CjvxHostJsonDefines.h"
#include <iostream>
#include <sstream>

extern "C"
{
#include "jvx-crc.h"
}

jvxErrorType
CjvxHostJsonCommandsShow::produce_component_core(CjvxJsonElementList& jsec)
{
	jvxSize i,j;
	jvxApiString fldDescr;
	jvxApiString fldDescror;
	CjvxJsonElement jelm;
	CjvxJsonArray jarr;
	std::string key;
	std::string values;
	jvxCBitField pMode = 0;
	jvxErrorType resL = JVX_NO_ERROR;

	for (i = 1; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		CjvxJsonElementList jelmlstl;
		CjvxJsonElement jelml;
		CjvxJsonArrayElement jarrelm;
		CjvxJsonArray jarrlocal;
		jvxComponentIdentification tp((jvxComponentType)i);
		jvxState stat = JVX_STATE_NONE;
		CjvxJsonElement jelma;
		jvxBool anyElementAdded = false;
		jvxSize numSlots = 0;
		jvxSize numSubSlots = 0;
		jvxComponentIdentification parentTp(JVX_COMPONENT_UNKNOWN);

		JVX_CREATE_COMPONENT_TYPE(jelml, jvxComponentType_txt(tp.tp));
		jelmlstl.addConsumeElement(jelml);

		resL = hHost->number_slots_component_system(tp, &numSlots, NULL, &parentTp.tp);
		if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			JVX_CREATE_SLOT_STATE(jelml, "not registered");
			jelmlstl.addConsumeElement(jelml);
		}
		else
		{
			if (parentTp != JVX_COMPONENT_UNKNOWN)
			{
				if (resL == JVX_ERROR_ID_OUT_OF_BOUNDS)
				{
					JVX_CREATE_SLOT_STATE(jelml, "parent inactive");
					jelmlstl.addConsumeElement(jelml);
				}
				else
				{
					CjvxJsonElementList jlstll;

					JVX_CREATE_NUMBER_SLOTS(jelml, JVX_SIZE_INT(numSlots));
					jelmlstl.addConsumeElement(jelml);

					for (j = 0; j < numSlots; j++)
					{
						CjvxJsonElement jelmll;

						parentTp.slotid = j;
						parentTp.slotsubid = 0;

						JVX_CREATE_COMPONENT_PARENT_IDENTIFICATION(jelmll, jvxComponentIdentification_txt(parentTp));
						jlstll.addConsumeElement(jelmll);

						tp.slotid = j;

						resL = hHost->number_slots_component_system(tp, NULL, &numSubSlots, NULL);
						JVX_CREATE_NUMBER_SUB_SLOTS(jelmll, JVX_SIZE_INT(numSubSlots));
						jlstll.addConsumeElement(jelmll);
					}
					jelml.makeSection("device_with_parent", jlstll);
					jelmlstl.addConsumeElement(jelml);
				}
			}
			else
			{
				JVX_CREATE_SLOT_STATE(jelml, "no parent");
				jelmlstl.addConsumeElement(jelml);

				JVX_CREATE_NUMBER_SLOTS(jelml, JVX_SIZE_INT(numSlots));
				jelmlstl.addConsumeElement(jelml);
			}
		}


		// ========================================================================

		create_active_component_one_type(tp.tp, jarrlocal, anyElementAdded);
		if (anyElementAdded)
		{
			JVX_CREATE_ACTIVE_COMPONENTS(jelma, jarrlocal);
			jelmlstl.addConsumeElement(jelma);
		}

		jvx_cbitSet(pMode, JVX_JSON_PRINT_MODE_COMPACT_SHIFT);
		jelmlstl.setPrintMode(pMode);
		jarrelm.makeSection(jelmlstl);
		jarr.addConsumeElement(jarrelm);

		// ========================================================================
	}
	JVX_CREATE_COMPONENTS(jelm, jarr);
	jsec.addConsumeElement(jelm);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::create_active_component_one_type(jvxComponentType tpelm, CjvxJsonArray& jarr, jvxBool& anyElementAdded)
{
	jvxSize j, k;
	CjvxJsonElementList jelmlstl;
	CjvxJsonElement jelml;
	CjvxJsonArrayElement jarrelm;
	jvxApiString fldDescr;
	jvxApiString fldDescror;
	jvxSize szSlots = JVX_SIZE_UNSELECTED;
	jvxSize szSubSlots = JVX_SIZE_UNSELECTED;
	jvxSize idS;
	jvxState stat = JVX_STATE_NONE;
	jvxCBitField pMode = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize uId = JVX_SIZE_UNSELECTED;

	jvxComponentType parentTp = JVX_COMPONENT_UNKNOWN;
	jvxComponentType childTp = JVX_COMPONENT_UNKNOWN;
	jvxComponentIdentification tp(tpelm);
	anyElementAdded = false;

	hHost->number_slots_component_system(tp, &szSlots, &szSubSlots, &parentTp, &childTp);
	if (parentTp == JVX_COMPONENT_UNKNOWN)
	{
		for (j = 0; j < szSlots; j++)
		{
			anyElementAdded = true;
			tp.slotid = j;
			JVX_CREATE_COMPONENT_IDENTIFICATION(jelml, jvxComponentIdentification_txt(tp));
			//values.clear();

			jelmlstl.addConsumeElement(jelml);
			hHost->selection_component(tp, &idS);
			fldDescr.assign("--");
			fldDescror.assign("--");
			if (JVX_CHECK_SIZE_SELECTED(idS))
			{
				hHost->description_selected_component(tp, &fldDescr);
				hHost->descriptor_selected_component(tp, &fldDescror);
				hHost->state_selected_component(tp, &stat);
			}
			JVX_CREATE_COMPONENT_DESCRIPTION(jelml, fldDescr.std_str());
			jelmlstl.addConsumeElement(jelml);
			
			JVX_CREATE_COMPONENT_DESCRIPTOR(jelml, fldDescror.std_str());
			jelmlstl.addConsumeElement(jelml);
			
			JVX_CREATE_COMPONENT_STATE(jelml, jvxState_txt(stat));
			jelmlstl.addConsumeElement(jelml);

			hHost->unique_id_selected_component(tp, &uId);
			JVX_CREATE_COMPONENT_UID(jelml, jvx_size2String(uId));
			jelmlstl.addConsumeElement(jelml);

			if (childTp != JVX_COMPONENT_UNKNOWN)
			{
				CjvxJsonArray jarr_dev;
				jvxComponentIdentification tpC = tp;
				tpC.tp = childTp;
				tpC.slotid = j;
				resL = hHost->number_slots_component_system(tpC, NULL, &szSubSlots);
				if (resL == JVX_NO_ERROR)
				{
					CjvxJsonArrayElement jarr_dev_elm;
					CjvxJsonElementList jarr_dev_elm_lst;
					CjvxJsonElement jarr_dev_elm_lst_elm;
					for (k = 0; k < szSubSlots; k++)
					{
						// jarr_dev_elm.makeSection()
						tpC.slotsubid = k;
						JVX_CREATE_COMPONENT_IDENTIFICATION(jarr_dev_elm_lst_elm, jvxComponentIdentification_txt(tpC));

						jarr_dev_elm_lst.addConsumeElement(jarr_dev_elm_lst_elm);
						hHost->selection_component(tpC, &idS);
						fldDescr.assign("--");
						fldDescror.assign("--");
						if (JVX_CHECK_SIZE_SELECTED(idS))
						{
							hHost->description_selected_component(tpC, &fldDescr);
							hHost->descriptor_selected_component(tpC, &fldDescror);
							hHost->state_selected_component(tpC, &stat);
						}
						
						JVX_CREATE_COMPONENT_DESCRIPTION(jarr_dev_elm_lst_elm, fldDescr.std_str());
						jarr_dev_elm_lst.addConsumeElement(jarr_dev_elm_lst_elm);
						
						JVX_CREATE_COMPONENT_DESCRIPTOR(jarr_dev_elm_lst_elm, fldDescror.std_str());
						jarr_dev_elm_lst.addConsumeElement(jarr_dev_elm_lst_elm);
						
						JVX_CREATE_COMPONENT_STATE(jarr_dev_elm_lst_elm, jvxState_txt(stat));
						jarr_dev_elm_lst.addConsumeElement(jarr_dev_elm_lst_elm);

						hHost->unique_id_selected_component(tpC, &uId);
						JVX_CREATE_COMPONENT_UID(jarr_dev_elm_lst_elm, jvx_size2String(uId));
						jarr_dev_elm_lst.addConsumeElement(jarr_dev_elm_lst_elm);
						jvx_cbitSet(pMode, JVX_JSON_PRINT_MODE_COMPACT_SHIFT);
						jarr_dev_elm_lst.setPrintMode(pMode);

						jarr_dev_elm.makeSection(jarr_dev_elm_lst);
						jarr_dev.addConsumeElement(jarr_dev_elm);
					}

					jelml.makeArray("devices", jarr_dev);
					jelmlstl.addConsumeElement(jelml);

				}
			}
			
			jvx_cbitSet(pMode, JVX_JSON_PRINT_MODE_COMPACT_SHIFT);
			jelmlstl.setPrintMode(pMode);

			jarrelm.makeSection(jelmlstl);
			jarr.addConsumeElement(jarrelm);
		}
	}
	

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxHostJsonCommandsShow::show_components(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
	jvxSize off, CjvxJsonElementList& jsec)
{
	jvxApiString fldDescr;
	jvxApiString fldDescror;
	CjvxJsonElement jelm;
	CjvxJsonArray jarr;
	std::string key;
	std::string values;
	jvxCBitField pMode = 0;
	jvxErrorType resL = JVX_NO_ERROR;
	jsec.deleteAll();

	assert(hHost);
		
	produce_component_core(jsec);

	
	/*
	std::string txt;
	jsec.printString(txt, JVX_JSON_PRINT_JSON, 0, "P", "I", false);
	std::cout << std::endl << txt << std::endl;
	
	txt.clear();
	jsec.printString(txt, JVX_JSON_PRINT_TXTCONSOLE, 0, "P", "I", false);
	std::cout << std::endl << txt << std::endl;
	*/
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_connections(const oneDrivehostCommand& dh_command,
	const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	jvxDrivehostConnectionShow showmode = JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL;

	jvxSize i;
	jvxApiString fldDescr;
	jvxApiString fldDescror;

	CjvxJsonArray jarr;
	std::string key;
	std::string values;
	jvxCBitField pMode = 0;
	jsec.deleteAll();

	jvxSize specId = JVX_SIZE_UNSELECTED;
	std::string specName;
	jvxBool err = false;;
	jvxBool arg2set = false;


	assert(hHost);

	if (args.size() >= 2)
	{
		if (args[1] == "path")
		{
			showmode = JVX_DRIVEHOST_CONNECTION_SHOW_PATH;
		}
		else if ((args[1] == "normal") || (args[1].empty()))
		{
			showmode = JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL;
		}
		else if (args[1] == "last")
		{
			showmode = JVX_DRIVEHOST_CONNECTION_SHOW_LAST;
		}
		else
		{
			JVX_CREATE_ERROR_RETURN(jsec, JVX_ERROR_INVALID_ARGUMENT, "Option <" + args[1] + "> is not valid.");
		}
	}
	if (args.size() >= 3)
	{
		specName = args[2];
		specId = jvx_string2Size(specName, err);
		if (err)
		{
			// It is not a string, use it as a name
			specId = JVX_SIZE_UNSELECTED;
		}
		arg2set = true;
	}

	output_connections_core(jsec, showmode, arg2set, specId,specName );
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxHostJsonCommandsShow::output_connections_core(
	CjvxJsonElementList& jsec,
	jvxDrivehostConnectionShow showmode,
	jvxBool arg2set, jvxSize specId, const std::string& specName)
{
	jvxSize num = 0, i = 0;
	std::string errTxt;
	CjvxJsonElement jelm;
	IjvxDataConnections* connections = NULL;
	// Just list all connections
	jvxErrorType res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
	if ((res == JVX_NO_ERROR) && connections)
	{
		CjvxJsonArray jarr;
		connections->number_connections_process(&num);
		if (arg2set)
		{
			if (JVX_CHECK_SIZE_SELECTED(specId))
			{
				if (specId < num)
				{
					CjvxJsonArrayElement jarrelm;
					std::string txt;
					CjvxJsonElementList jelmoneseq;
					res = output_one_process(connections, specId, specName, jelmoneseq, errTxt, showmode);
					if (res == JVX_NO_ERROR)
					{
						jarrelm.makeSection(jelmoneseq);
						jarr.addConsumeElement(jarrelm);
					}
					else
					{
						JVX_CREATE_ERROR_RETURN(jsec, res, errTxt);
					}
				}
				else
				{
					JVX_CREATE_ERROR_RETURN(jsec, JVX_ERROR_ID_OUT_OF_BOUNDS, ("Specified id for connection <" + jvx_size2String(specId) + "> is out of bounds."));
				}
			} // if (JVX_CHECK_SIZE_SELECTED(specId))
			else
			{
				CjvxJsonArrayElement jarrelm;
				std::string txt;
				CjvxJsonElementList jelmoneseq;
				res = output_one_process(connections, specId, specName, jelmoneseq, errTxt, showmode);
				if (res == JVX_NO_ERROR)
				{
					jarrelm.makeSection(jelmoneseq);
					jarr.addConsumeElement(jarrelm);
				}
				else
				{
					JVX_CREATE_ERROR_RETURN(jsec, res, errTxt);
				}
			}
		} // if (arg1set)
		else
		{
			for (i = 0; i < num; i++)
			{
				CjvxJsonArrayElement jarrelm;
				std::string txt;
				CjvxJsonElementList jelmoneseq;
				res = output_one_process(connections, i, "", jelmoneseq, errTxt, showmode);
				if (res == JVX_NO_ERROR)
				{
					jarrelm.makeSection(jelmoneseq);
					jarr.addConsumeElement(jarrelm);
				}
				else
				{
					JVX_CREATE_ERROR_NO_RETURN(jelmoneseq, res, errTxt);
				}
			} // for(i = 0; i < num; i++)
		}

		res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
		JVX_CREATE_CONNECTIONS(jelm, jarr);
		jsec.addConsumeElement(jelm);
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_connection_rules(const oneDrivehostCommand& dh_command,
	const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num = 0;

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

	assert(hHost);

	if (args.size() >= 3)
	{
		specName = args[2];
		specId = jvx_string2Size(specName, err);
		if (err)
		{
			// It is not a string, use it as a name
			specId = JVX_SIZE_UNSELECTED;
		}
		arg2set = true;
	}

	// Just list all connections
	res = hHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&connections));
	if ((res == JVX_NO_ERROR) && connections)
	{
		CjvxJsonArray jarr;
		connections->number_connection_rules(&num);
		if (arg2set)
		{
			if (JVX_CHECK_SIZE_SELECTED(specId))
			{
				if (specId < num)
				{
					CjvxJsonArrayElement jarrelm;
					std::string txt;
					CjvxJsonElementList jelmoneseq;
					res = output_one_connection_rule(connections, specId, specName, jelmoneseq, errTxt);
					if (res == JVX_NO_ERROR)
					{
						jarrelm.makeSection(jelmoneseq);
						jarr.addConsumeElement(jarrelm);
					}
					else
					{
						JVX_CREATE_ERROR_RETURN(jsec, res, errTxt);
					}
				}
				else
				{
					JVX_CREATE_ERROR_RETURN(jsec, JVX_ERROR_ID_OUT_OF_BOUNDS, ("Specified id for connection <" + jvx_size2String(specId) + "> is out of bounds."));
				}
			} // if (JVX_CHECK_SIZE_SELECTED(specId))
			else
			{
				CjvxJsonArrayElement jarrelm;
				std::string txt;
				CjvxJsonElementList jelmoneseq;
				res = output_one_connection_rule(connections, specId, specName, jelmoneseq, errTxt);
				if (res == JVX_NO_ERROR)
				{
					jarrelm.makeSection(jelmoneseq);
					jarr.addConsumeElement(jarrelm);
				}
				else
				{
					JVX_CREATE_ERROR_RETURN(jsec, res, errTxt);
				}
			}
		} // if (arg1set)
		else
		{
			for (i = 0; i < num; i++)
			{
				CjvxJsonArrayElement jarrelm;
				std::string txt;
				CjvxJsonElementList jelmoneseq;
				res = output_one_connection_rule(connections, i, "", jelmoneseq, errTxt);
				if (res == JVX_NO_ERROR)
				{
					jarrelm.makeSection(jelmoneseq);
					jarr.addConsumeElement(jarrelm);
				}
				else
				{
					JVX_CREATE_ERROR_NO_RETURN(jelmoneseq, res, errTxt);
				}
			} // for(i = 0; i < num; i++)
		}

		res = hHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(connections));
		JVX_CREATE_CONNECTION_RULES(jelm, jarr);
		jsec.addConsumeElement(jelm);
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_system_compact(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxSize i;
	jvxSize szSlots = 0;
	jvxSize szSSlots = JVX_SIZE_UNSELECTED;
	jvxComponentType parentTp = JVX_COMPONENT_UNKNOWN;
	jvxSize selIdx = JVX_SIZE_UNSELECTED;

	CjvxJsonArray jarr_comps;
	CjvxJsonElement jelm_comps;
	CjvxJsonElement jelmoutall;
	CjvxJsonElementList jelmoutl;
	CjvxJsonElementList jelmoutll;

	for (i = 1; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		CjvxJsonElementList jelmlst_entries;
		CjvxJsonElement jelm_entry;
		CjvxJsonArrayElement jarr_entry;
		jvxSize uId = JVX_SIZE_UNSELECTED;
		jvxBool anyElementAdded = false;

		create_active_component_one_type((jvxComponentType)i, jarr_comps, anyElementAdded);

#if 0
		involvedHost.hHost->number_slots_component(tpL, &szSlots, NULL, &parentTp);
		if (szSlots > 0)
		{ 
			if (parentTp != JVX_COMPONENT_UNKNOWN)
			{
				for (j = 0; j < szSlots; j++)
				{
					tpL.slotid = j;
					involvedHost.hHost->number_slots_component(tpL, NULL, &szSSlots, NULL);

					for (k = 0; k < szSSlots; k++)
					{
						tpL.slotsubid = k;
						selIdx = JVX_SIZE_UNSELECTED;
						involvedHost.hHost->selection_component(tpL, &selIdx);
						if (JVX_CHECK_SIZE_SELECTED(selIdx))
						{
							jvxApiString nm;

							JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_entry, jvxComponentIdentification_txt(tpL));
							jelmlst_entries.addConsumeElement(jelm_entry);

							involvedHost.hHost->description_component(tpL, &nm);
							JVX_CREATE_COMPONENT_DESCRIPTION(jelm_entry, nm.std_str());
							jelmlst_entries.addConsumeElement(jelm_entry);

							involvedHost.hHost->descriptor_component(tpL, &nm);
							JVX_CREATE_COMPONENT_DESCRIPTOR(jelm_entry, nm.std_str());
							jelmlst_entries.addConsumeElement(jelm_entry);

							stat = JVX_STATE_NONE;
							involvedHost.hHost->state_component(tpL, &stat);
							
							JVX_CREATE_COMPONENT_STATE(jelm_entry, jvxState_txt(stat));
							jelmlst_entries.addConsumeElement(jelm_entry);

							involvedHost.hHost->unique_id_component(tpL, &uId);
							JVX_CREATE_COMPONENT_UID(jelm_entry, jvx_size2String(uId));
							jelmlst_entries.addConsumeElement(jelm_entry);

							jarr_entry.makeSection(jelmlst_entries);
							jarr_comps.addConsumeElement(jarr_entry);
						}
					}
				}
			}
			else
			{
				for (j = 0; j < szSlots; j++)
				{
					tpL.slotid = j;
					selIdx = JVX_SIZE_UNSELECTED;
					involvedHost.hHost->selection_component(tpL, &selIdx);
					if (JVX_CHECK_SIZE_SELECTED(selIdx))
					{
						jvxApiString nm;

						JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_entry, jvxComponentIdentification_txt(tpL));
						jelmlst_entries.addConsumeElement(jelm_entry);

						involvedHost.hHost->description_component(tpL, &nm);
						JVX_CREATE_COMPONENT_DESCRIPTION(jelm_entry, nm.std_str());
						jelmlst_entries.addConsumeElement(jelm_entry);

						involvedHost.hHost->descriptor_component(tpL, &nm);
						JVX_CREATE_COMPONENT_DESCRIPTOR(jelm_entry, nm.std_str());
						jelmlst_entries.addConsumeElement(jelm_entry);

						stat = JVX_STATE_NONE;
						involvedHost.hHost->state_component(tpL, &stat);

						JVX_CREATE_COMPONENT_STATE(jelm_entry, jvxState_txt(stat));
						jelmlst_entries.addConsumeElement(jelm_entry);

						involvedHost.hHost->unique_id_component(tpL, &uId);
						JVX_CREATE_COMPONENT_UID(jelm_entry, jvx_size2String(uId));
						jelmlst_entries.addConsumeElement(jelm_entry);

						jarr_entry.makeSection(jelmlst_entries);
						jarr_comps.addConsumeElement(jarr_entry);
					}
				}
			}
		}
#endif
	}

	JVX_CREATE_SELECTED(jelm_comps, jarr_comps);
	jelmoutl.addConsumeElement(jelm_comps);

	// ====================================================

	output_sequencer_status(jelmoutll);
	JVX_CREATE_SEQUENCER(jelm_comps, jelmoutll);
	jelmoutl.addConsumeElement(jelm_comps);

	// ====================================================

	output_connections_core(jelmoutll, JVX_DRIVEHOST_CONNECTION_SHOW_COMPACT);
	jelm_comps.makeSection("connections", jelmoutll);
	jelmoutl.addConsumeElement(jelm_comps);

	// ====================================================

	jelmoutall.makeSection("system", jelmoutl);
	jsec.addConsumeElement(jelmoutall);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_version(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxSize szSlots = 0;
	jvxSize szSSlots = JVX_SIZE_UNSELECTED;
	jvxComponentType parentTp = JVX_COMPONENT_UNKNOWN;
	jvxSize selIdx = JVX_SIZE_UNSELECTED;

	CjvxJsonElement jelm_version;
	CjvxJsonElement jelm_entry;
	CjvxJsonElementList jelmlst_entries;

	CjvxJsonArray jarr_comps;
	CjvxJsonArrayElement jarr_elm;

	JVX_CREATE_VERSION_ME(jelm_entry, JVX_IDENT_STRING);
	jelmlst_entries.addConsumeElement(jelm_entry);

	JVX_CREATE_VERSION_GIT(jelm_entry, JVX_VERSION_STRING);
	jelmlst_entries.addConsumeElement(jelm_entry);

	JVX_CREATE_VERSION_DATA(jelm_entry, JVX_DATA_STRING);
	jelmlst_entries.addConsumeElement(jelm_entry);

	jelm_version.makeSection("version", jelmlst_entries);
	jsec.addConsumeElement(jelm_version);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_system(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString  fldStr;
	jvxSize numSlots = 0;
	jvxSize numSubSlots = 0;
	CjvxJsonArray jarr;
	CjvxJsonElement jelm;
	jvxComponentIdentification parentTp = JVX_COMPONENT_UNKNOWN;
	jvxCBitField pMode = 0;
	CjvxJsonElementList jelmoutl;
	CjvxJsonElementList jelmoutll;
	CjvxJsonElement jelmout;
	CjvxJsonElement jelmoutall;
	std::string token = "status";

	assert(hHost);

	if (args.size() > off)
	{
		token = args[off];
	}

	if (token == "compact")
	{
		return show_system_compact(dh_command, args, off, jsec);
	}
	else if (token == "status")
	{
		// Return full system status
		produce_component_core(jelmoutl);
		// jelmout.makeSection("components", jelmoutll);
		// jelmoutl.addConsumeElement(jelmout);

		// ====================================================
		
		output_sequencer_status(jelmoutll);
		JVX_CREATE_SEQUENCER(jelmout, jelmoutll);
		jelmoutl.addConsumeElement(jelmout);

		// ====================================================
		
		output_connections_core(jelmoutll, JVX_DRIVEHOST_CONNECTION_SHOW_COMPACT);
		jelmout.makeSection("connections", jelmoutll);
		jelmoutl.addConsumeElement(jelmout);

		// ====================================================

		jelmoutall.makeSection("system", jelmoutl);
		jsec.addConsumeElement(jelmoutall);

		return JVX_NO_ERROR;
	}
	JVX_CREATE_ERROR_RETURN(jsec, JVX_ERROR_INVALID_ARGUMENT, ("Invalid system command <" + token + ">."));
	return JVX_NO_ERROR;	
}

jvxErrorType
CjvxHostJsonCommandsShow::show_sequencer(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst)
{
	IjvxSequencer* sequencer = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString  fldStr;
	jvxSequencerQueueType tp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
	jvxSize num = 0;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxSize idSS = JVX_SIZE_UNSELECTED;
	jvxBool showRunQueue = false;
	jvxBool showLeaveQueue = false;
	jvxBool err = false;
	jvxSize i;	
	CjvxJsonElement jelm;

	/*
	CjvxJsonElementList jelmlstl;
	CjvxJsonElement jelml;
	CjvxJsonElementList jelmlstll;
	CjvxJsonElement jelmll;
	CjvxJsonElementList jelmlstlll;
	CjvxJsonElement jelmlll;

	jvxCBitField pMode = 0;
	jvx_cbitSet(pMode, JVX_JSON_PRINT_MODE_ADD_LAYER_OUTPUT_COMPACT);

	jelmlll.makeAssignmentString("a", "b");
	jelmlstlll.addConsumeElement(jelmlll);
	jelmlll.makeAssignmentString("c", "d");
	jelmlstlll.addConsumeElement(jelmlll);

	jelmll.makeSection("e", jelmlstlll);
	jelmlstll.addConsumeElement(jelmll);

	jelml.makeSection("f", jelmlstll);
	jelmlstl.addConsumeElement(jelml);

	jelmlll.makeAssignmentString("z", "y");
	jelmlstlll.addConsumeElement(jelmlll);
	jelmlll.makeAssignmentString("w", "x");
	jelmlstlll.addConsumeElement(jelmlll);

	jelmll.makeSection("g", jelmlstlll);
	jelmlstll.addConsumeElement(jelmll);
	jelmlstll.setPrintMode(pMode);

	jelml.makeSection("h", jelmlstll);
	jelmlstl.addConsumeElement(jelml);

	std::string txt;
	jelmlstl.printString(txt, JVX_JSON_PRINT_TXTCONSOLE, 0, "\t", "\t", false);
	std::cout << std::endl << txt << std::endl;
	txt.clear();
	jelmlstl.printString(txt, JVX_JSON_PRINT_JSON, 0, "\t", "\t", false);
	std::cout << std::endl << txt << std::endl;
	*/

	if (args.size() > off)
	{
		idS = jvx_string2Size(args[off], err);
		if (err)
		{
			if (args[off] == "status")
			{
				CjvxJsonElementList jelmlstl;
				CjvxJsonElement jelml;
				output_sequencer_status(jelmlstl);
				jelml.makeSection("sequencer", jelmlstl);
				jelmlst.addConsumeElement(jelml);
				return JVX_NO_ERROR;
			}
			else
			{
				JVX_CREATE_ERROR_RETURN(jelmlst, JVX_ERROR_INVALID_ARGUMENT, ("Invalid specification of squence id <" + args[off] + ">."));
			}
		}
		
	}

	if (args.size() > off + 1)
	{
		if (args[off + 1] == jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_RUN].friendly)
		{
			showRunQueue = true;
		}
		else if (args[off+1] == jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_LEAVE].friendly)
		{
			showRunQueue = true;
		}
		else
		{ 
			JVX_CREATE_ERROR_RETURN(jelmlst, JVX_ERROR_INVALID_ARGUMENT, ("Invalid specification of queue type <" + args[off + 1] + ">."));
		}
	}
	else
	{
		showRunQueue = true;
		showLeaveQueue = true;
	}

	if (args.size() > off + 2)
	{
		idSS = jvx_string2Size(args[off+2], err);
		if (err)
		{
			JVX_CREATE_ERROR_RETURN(jelmlst, JVX_ERROR_INVALID_ARGUMENT, ("Invalid specification of squence id <" + args[off + 2] + ">."));
		}
	}
	
	res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		CjvxJsonArray jarr;
		sequencer->number_sequences(&num);
		for(i = 0; i < num; i++)
		{ 
			CjvxJsonArrayElement jarrelm;
			std::string txt;
			CjvxJsonElementList jelmoneseq;
			output_one_sequence(sequencer, i, showRunQueue, showLeaveQueue, idSS, jelmoneseq);
			jarrelm.makeSection(jelmoneseq);
			jarr.addConsumeElement(jarrelm);
		} // for(i = 0; i < num; i++)
		res = hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		JVX_CREATE_SEQUENCES(jelm, jarr);
		jelmlst.addConsumeElement(jelm);
	} // res = runtime.theHostRef->request_hidden_interface(JVX_INTERFACE
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_one_sequence(IjvxSequencer* sequencer, jvxSize i, jvxBool showRunQueue, jvxBool showLeaveQueue, jvxSize idSS, CjvxJsonElementList& lstelmr)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numStepsRun, numStepsLeave;
	jvxApiString  fldStr;
	jvxApiString  fldStrLabel;
	jvxSize j;
	jvxBool markedForProcess = false;
	jvxBool isDefault = false;
	CjvxJsonElement elmr;
	CjvxJsonElementList jelmlqueues;

	std::string txt;

	sequencer->description_sequence(i, &fldStr, &fldStrLabel, &numStepsRun, &numStepsLeave, &markedForProcess, &isDefault);

	JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_DESCRIPTION(elmr, fldStr.std_str());
	lstelmr.addConsumeElement(elmr);

	JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_LABEL(elmr, fldStrLabel.std_str());
	lstelmr.addConsumeElement(elmr); 

	if (markedForProcess)
	{
		elmr.makeAssignmentString("sequence_active", jvx_make_yes);
	}
	else
	{
		elmr.makeAssignmentString("sequence_active", jvx_make_no);
	}
	if (isDefault)
	{
		elmr.makeAssignmentString("sequence_default", jvx_make_yes);
	}
	else
	{
		elmr.makeAssignmentString("sequence_default", jvx_make_no);
	}
	lstelmr.addConsumeElement(elmr);


	if (showRunQueue)
	{
		CjvxJsonElement jelmq;
		CjvxJsonArray jarr;
		if (JVX_CHECK_SIZE_UNSELECTED(idSS))
		{
			for (j = 0; j < numStepsRun; j++)
			{
				CjvxJsonArrayElement jarrelm;
				CjvxJsonElementList jelmlst;
				output_one_step_sequence(sequencer, i, j, JVX_SEQUENCER_QUEUE_TYPE_RUN, jelmlst);
				jarrelm.makeSection(jelmlst);
				jarr.addConsumeElement(jarrelm);
			}
		}
		else
		{
			if (idSS < numStepsRun)
			{
				CjvxJsonArrayElement jarrelm;
				CjvxJsonElementList jelmlst;
				output_one_step_sequence(sequencer, i, idSS, JVX_SEQUENCER_QUEUE_TYPE_RUN, jelmlst);
				jarrelm.makeSection(jelmlst);
				jarr.addConsumeElement(jarrelm);
			}
			else
			{
				JVX_CREATE_ERROR_RETURN(lstelmr, JVX_ERROR_ELEMENT_NOT_FOUND, "Sequence id <" + jvx_size2String(idSS) + "is out of bounds of valid sequence ids (the number is " + jvx_size2String(numStepsRun) + ").");
			}
		}
		jelmq.makeArray(jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_RUN].friendly, jarr);
		jelmlqueues.addConsumeElement(jelmq);
		//txt.clear();
		//jelmlqueues.printString(txt, JVX_JSON_PRINT_TXTCONSOLE, 0, "", "", false);
	}

	if (showLeaveQueue)
	{
		CjvxJsonElement jelmq;
		CjvxJsonArray jarr;
		if (JVX_CHECK_SIZE_UNSELECTED(idSS))
		{
			for (j = 0; j < numStepsLeave; j++)
			{
				CjvxJsonArrayElement jarrelm;
				CjvxJsonElementList jelmlst;
				output_one_step_sequence(sequencer, i, j, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, jelmlst);
				jarrelm.makeSection(jelmlst);
				jarr.addConsumeElement(jarrelm);
			}
		}
		else
		{
			if (idSS < numStepsRun)
			{
				CjvxJsonArrayElement jarrelm;
				CjvxJsonElementList jelmlst;
				output_one_step_sequence(sequencer, i, idSS, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, jelmlst);
				jarrelm.makeSection(jelmlst);
				jarr.addConsumeElement(jarrelm);
			}
			else
			{
				JVX_CREATE_ERROR_RETURN(lstelmr, JVX_ERROR_ELEMENT_NOT_FOUND, "Sequence id <" + jvx_size2String(idSS) + "is out of bounds of valid sequence ids (the number is " + jvx_size2String(numStepsLeave) + ").");
			}
		}
		jelmq.makeArray(jvxSequencerQueueType_str[JVX_SEQUENCER_QUEUE_TYPE_LEAVE].friendly, jarr);
		jelmlqueues.addConsumeElement(jelmq);
		//txt.clear();
		//jelmlqueues.printString(txt, JVX_JSON_PRINT_TXTCONSOLE, 0, "", "", false);
	}

	JVX_CREATE_COMPONENT_SEQ_QUEUS(elmr, jelmlqueues);
	lstelmr.addConsumeElement(elmr);
	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_one_process(IjvxDataConnections* connections, jvxSize idx, const std::string& specName,
	CjvxJsonElementList& lstelmr, std::string& errTxt, jvxDrivehostConnectionShow showmode)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString  fldStr;
	CjvxJsonElement elmr;
	CjvxJsonElementList elmlp;
	jvxSize num = 0;
	jvxSize i;
	IjvxDataConnectionProcess* oneProcess = NULL;
	IjvxDataConnectionRule* oneRule = NULL;
	std::string txt;
	jvxSize ruleId = JVX_SIZE_UNSELECTED;
	jvxApiString astr;
	jvxErrorType resL = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

	// Check for index within range was done before
	if (JVX_CHECK_SIZE_UNSELECTED(idx))
	{
		connections->number_connections_process(&num);
		for (i = 0; i < num; i++)
		{
			res = connections->reference_connection_process(i, &oneProcess);
			assert(res == JVX_NO_ERROR);
			assert(oneProcess);
			oneProcess->descriptor_connection(&fldStr);
			connections->return_reference_connection_process(oneProcess);
			
			if (fldStr.std_str() == specName)
			{
				idx = i;
				break;
			}
		}
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idx))
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "Could not find specified element <" + specName;
	}
	else
	{
		res = connections->reference_connection_process(idx, &oneProcess);
		if ((res == JVX_NO_ERROR) && (oneProcess))
		{
			JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
			jvxBool testok = false;
			jvxState stat = JVX_STATE_NONE;
			jvxSize uid = JVX_SIZE_UNSELECTED;
			oneProcess->descriptor_connection(&fldStr);
			connections->uid_for_reference(oneProcess, &uid);

			oneProcess->misc_connection_parameters(&ruleId, nullptr);

			JVX_CREATE_CONNECTION_DESCRIPTOR(elmr, fldStr.std_str());
			lstelmr.addConsumeElement(elmr);
			
			JVX_CREATE_CONNECTION_UID(elmr, uid);
			lstelmr.addConsumeElement(elmr);

			// Only name and uid in compact mode
			if (showmode != JVX_DRIVEHOST_CONNECTION_SHOW_COMPACT)
			{
				if (JVX_CHECK_SIZE_SELECTED(ruleId))
				{
					JVX_CREATE_CONNECTION_ISFROMRULE(elmr, jvx_make_yes);
				}
				else
				{
					JVX_CREATE_CONNECTION_ISFROMRULE(elmr, jvx_make_no);
				}
				lstelmr.addConsumeElement(elmr);

				oneProcess->status(&stat);
				oneProcess->status_chain(&testok JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				JVX_CREATE_CONNECTION_STATUS(elmr, jvxState_txt(stat));
				lstelmr.addConsumeElement(elmr);

				if (testok)
				{
					JVX_CREATE_CONNECTION_LAST_TEST(elmr, JVX_INDICATE_TRUE_SHORT);
				}
				else
				{
					JVX_CREATE_CONNECTION_LAST_TEST(elmr, JVX_INDICATE_FALSE_SHORT);
				}
				lstelmr.addConsumeElement(elmr);

				resL = oneProcess->check_process_ready(&astr);
				if (resL == JVX_NO_ERROR)
				{
					JVX_CREATE_CONNECTION_IS_READY(elmr, JVX_INDICATE_TRUE_SHORT);
					lstelmr.addConsumeElement(elmr);
				}
				else
				{
					JVX_CREATE_CONNECTION_IS_READY(elmr, JVX_INDICATE_FALSE_SHORT);
					lstelmr.addConsumeElement(elmr);

					JVX_CREATE_CONNECTION_REASON_IF_NOT(elmr, astr.std_str());
					lstelmr.addConsumeElement(elmr);
				}


				switch (showmode)
				{
				case JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL:
					output_one_process_details(oneProcess, elmr);

					break;
				case JVX_DRIVEHOST_CONNECTION_SHOW_PATH:

					res = oneProcess->transfer_forward_chain(JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON, &elmlp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					elmr.makeSection("process_path", elmlp);
					break;
				case JVX_DRIVEHOST_CONNECTION_SHOW_LAST:
					std::ostringstream sstr;
					fdb->printResult(sstr, 0);
					elmr.makeAssignmentString("process_last_error", sstr.str());
					break;
				}
				lstelmr.addConsumeElement(elmr);
			}
			connections->return_reference_connection_process(oneProcess);
			oneProcess = NULL;
		}
	}
	
	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_one_process_details(IjvxDataConnectionProcess* oneProcess, CjvxJsonElement& elmr)
{
	jvxApiString fldStr;
	IjvxConnectionMaster* theMas = NULL;
	IjvxConnectionMasterFactory* theMasFac = NULL;
	jvxComponentIdentification tpId;
	jvxApiString strMF;
	CjvxJsonElementList elml;
	CjvxJsonElement elmlr;
	oneProcess->associated_master(&theMas);
	if (theMas)
	{
		theMas->name_master(&fldStr);
		JVX_CREATE_MASTER_NAME(elmlr, fldStr.std_str());
		elml.addConsumeElement(elmlr);

		/*
		theMas->descriptor_master(&fldStr);
		JVX_CREATE_MASTER_DESCRIPTION(elmlr, fldStr.std_str());
		elml.addConsumeElement(elmlr);
		*/

		theMas->parent_master_factory(&theMasFac);
		if (theMasFac)
		{
			jvx_request_interfaceToObject(theMasFac, NULL, &tpId, &strMF);
			JVX_CREATE_MASTER_IDENTIFICATION(elmlr, jvxComponentIdentification_txt(tpId));
			elml.addConsumeElement(elmlr);

			JVX_CREATE_MASTER_PARENT_NAME(elmlr, strMF.std_str());
			elml.addConsumeElement(elmlr);
		}
	}
	else
	{
		JVX_CREATE_MASTER_NAME(elmlr, "none associated");
		elml.addConsumeElement(elmlr);
	}

	CjvxJsonArray elmarr;
	jvxSize i, num = 0;
	IjvxOutputConnector* ocon = NULL;
	IjvxInputConnector* icon = NULL;

	oneProcess->number_bridges(&num);
	for (i = 0; i < num; i++)
	{
		IjvxConnectorBridge* theBridge = NULL;
		oneProcess->reference_bridge(i, &theBridge);
		assert(theBridge);
		CjvxJsonArrayElement elmarrelm;
		CjvxJsonElement newelm;
		CjvxJsonElementList newelml;
		CjvxJsonElement newelmloc;

		theBridge->descriptor_bridge(&fldStr);
		JVX_CREATE_BRIDGE_NAME(newelmloc, fldStr.std_str());
		newelml.addConsumeElement(newelmloc);

		theBridge->reference_connect_from(&ocon);
		if (ocon)
		{
			ocon->descriptor_connector(&fldStr);
			JVX_CREATE_OCON_NAME(newelmloc, fldStr.std_str());
			newelml.addConsumeElement(newelmloc);

			IjvxConnectorFactory* theOconFac = NULL;
			ocon->parent_factory(&theOconFac);
			if (theOconFac)
			{
				jvx_request_interfaceToObject(theOconFac, NULL, &tpId, &strMF);
				JVX_CREATE_OCON_PARENT_IDENTIFICATION(newelmloc, jvxComponentIdentification_txt(tpId));
				newelml.addConsumeElement(newelmloc);

				JVX_CREATE_OCON_PARENT_NAME(newelmloc, strMF.std_str());
				newelml.addConsumeElement(newelmloc);
			}
			else
			{
				assert(0);
			}
			theBridge->return_reference_connect_from(ocon);
			ocon = NULL;
		}

		theBridge->reference_connect_to(&icon);
		if (icon)
		{
			icon->descriptor_connector(&fldStr);
			JVX_CREATE_ICON_NAME(newelmloc, fldStr.std_str());
			newelml.addConsumeElement(newelmloc);

			IjvxConnectorFactory* theIconFac = NULL;
			icon->parent_factory(&theIconFac);
			if (theIconFac)
			{
				jvx_request_interfaceToObject(theIconFac, NULL, &tpId, &strMF);
				JVX_CREATE_ICON_PARENT_IDENTIFICATION(newelmloc, jvxComponentIdentification_txt(tpId));
				newelml.addConsumeElement(newelmloc);

				JVX_CREATE_ICON_PARENT_NAME(newelmloc, strMF.std_str());
				newelml.addConsumeElement(newelmloc);
			}
			else
			{
				assert(0);
			}
			theBridge->return_reference_connect_to(icon);
			icon = NULL;
		} // if (icon)
		oneProcess->return_reference_bridge(theBridge);
		theBridge = NULL;

		elmarrelm.makeSection(newelml);
		elmarr.addConsumeElement(elmarrelm);
	}

	elmlr.makeArray("bridges", elmarr);
	elml.addConsumeElement(elmlr);

	elmr.makeSection("process", elml);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_one_rule_details(IjvxDataConnectionRule* oneRule, CjvxJsonElement& elmr)
{
	jvxApiString fldStr;
	jvxSize i; jvxSize num = 0;
	IjvxConnectionMaster* theMas = NULL;
	IjvxConnectionMasterFactory* theMasFac = NULL;
	jvxComponentIdentification tpId;
	jvxApiString fldStr1, fldStr2;
	jvxApiString fldStr_from_fac, fldStr_from_con;
	jvxApiString fldStr_to_fac, fldStr_to_con;
	jvxComponentIdentification tpId_from; 
	jvxComponentIdentification tpId_to; 
	CjvxJsonElementList elml;
	CjvxJsonElement elmlr;
	oneRule->get_master(&tpId, &fldStr1, &fldStr2);

	JVX_CREATE_CONNECTION_RULE_MAS_IDENTIFICATION(elmlr, jvxComponentIdentification_txt(tpId));
	elml.addConsumeElement(elmlr);

	JVX_CREATE_CONNECTION_RULE_MAS_FAC_EXPRESSION(elmlr, fldStr1.std_str());
	elml.addConsumeElement(elmlr);

	JVX_CREATE_CONNECTION_RULE_MAS_EXPRESSION(elmlr, fldStr2.std_str());
	elml.addConsumeElement(elmlr);

	CjvxJsonArray elmarr;
	oneRule->number_bridges(&num);
	for (i = 0; i < num; i++)
	{
		oneRule->get_bridge(i, &fldStr1, &tpId_from, &fldStr_from_fac, &fldStr_from_con,
			&tpId_to, &fldStr_to_fac, &fldStr_to_con);

		CjvxJsonArrayElement elmarrelm;
		CjvxJsonElement newelm;
		CjvxJsonElementList newelml;
		CjvxJsonElement newelmloc;

		JVX_CREATE_CONNECTION_RULE_BRIDGE_NAME(newelmloc, fldStr1.std_str());
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_IDENTIFICATION_FROM(newelmloc, jvxComponentIdentification_txt(tpId_from));
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_FAC_NAME_FROM(newelmloc, fldStr_from_fac.std_str());
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_OCON_NAME_FROM(newelmloc, fldStr_from_con.std_str());
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_IDENTIFICATION_TO(newelmloc, jvxComponentIdentification_txt(tpId_to));
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_FAC_NAME_TO(newelmloc, fldStr_to_fac.std_str());
		newelml.addConsumeElement(newelmloc);

		JVX_CREATE_CONNECTION_RULE_ICON_NAME_TO(newelmloc, fldStr_to_con.std_str());
		newelml.addConsumeElement(newelmloc);

		elmarrelm.makeSection(newelml);
		elmarr.addConsumeElement(elmarrelm);
	}

	elmlr.makeArray("bridges", elmarr);
	elml.addConsumeElement(elmlr);

	elmr.makeSection("connection_rule", elml);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_one_connection_rule(IjvxDataConnections* connections, jvxSize idx, const std::string& specName,
	CjvxJsonElementList& lstelmr, std::string& errTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString  fldStr;
	CjvxJsonElement elmr;
	CjvxJsonElementList elmlp;
	jvxSize num = 0;
	jvxSize i;
	IjvxDataConnectionProcess* oneProcess = NULL;
	IjvxDataConnectionRule* oneRule = NULL;
	std::string txt;
	jvxBool isDefaultRule = false;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

	// Check for index within range was done before
	if (JVX_CHECK_SIZE_UNSELECTED(idx))
	{
		connections->number_connection_rules(&num);
		for (i = 0; i < num; i++)
		{
			res = connections->reference_connection_rule(i, &oneRule);
			assert(res == JVX_NO_ERROR);
			assert(oneRule);
			oneRule->description_rule(&fldStr, NULL);

			connections->return_reference_connection_rule(oneRule);
			if (fldStr.std_str() == specName)
			{
				idx = i;
				break;
			}
		}
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idx))
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "Could not find specified connection rule <" + specName + ">.";
	}
	else
	{
		res = connections->reference_connection_rule(idx, &oneRule);
		if ((res == JVX_NO_ERROR) && (oneRule))
		{
			oneRule->description_rule(&fldStr, &isDefaultRule);
			JVX_CREATE_CONNECTION_RULE_NAME(elmr, fldStr.std_str());
			lstelmr.addConsumeElement(elmr);

			if (isDefaultRule)
			{
				JVX_CREATE_CONNECTION_RULE_DEFAULT(elmr, jvx_make_yes);
			}
			else
			{
				JVX_CREATE_CONNECTION_RULE_DEFAULT(elmr, jvx_make_no);
			}
			output_one_rule_details(oneRule, elmr);

			lstelmr.addConsumeElement(elmr);
			connections->return_reference_connection_process(oneProcess);
			oneProcess = NULL;
		}
	}
	return res;
}

jvxErrorType 
CjvxHostJsonCommandsShow::output_one_step_sequence(IjvxSequencer* sequencer, jvxSize i, jvxSize j, jvxSequencerQueueType qTp, CjvxJsonElementList& jelmlst)
{
	jvxApiString  fldStr;
	jvxApiString  fldStr_lab;
	jvxApiString  fldStr_lab_true;
	jvxApiString  fldStr_lab_false;
	jvxSequencerElementType elementTp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxComponentIdentification targetTp = JVX_COMPONENT_UNKNOWN;
	jvxSize fId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout_ms = -1;
	CjvxJsonElement jelm;
	jvxBool b_action = false;
	jvxCBitField assoc_mode = 0;
	std::string txt;

	sequencer->description_step_sequence(i, j, qTp,
		&fldStr, &elementTp, &targetTp,
		&fId, &timeout_ms,
		&fldStr_lab, &fldStr_lab_true, &fldStr_lab_false, &b_action,
		&assoc_mode);

	JVX_CREATE_COMPONENT_DESCRIPTION(jelm, fldStr.std_str());
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_TYPE(jelm, jvxSequencerElementType_txt(elementTp));
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_TARGET_IDENTIFICATION(jelm, jvxComponentIdentification_txt(targetTp));
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_FID(jelm, jvx_size2String(fId));
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_TIMEOUT_MS(jelm, jvx_size2String(timeout_ms));
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL(jelm, fldStr_lab.std_str());
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL_TRUE(jelm, fldStr_lab_true.std_str());
	jelmlst.addConsumeElement(jelm);
	JVX_CREATE_COMPONENT_SEQ_ELEMENT_LABEL_FALSE(jelm, fldStr_lab_false.std_str());
	jelmlst.addConsumeElement(jelm);
	if (b_action)
	{
		JVX_CREATE_COMPONENT_SEQ_ELEMENT_BREAK_ACTION(jelm, jvx_make_yes);
	}
	else
	{
		JVX_CREATE_COMPONENT_SEQ_ELEMENT_BREAK_ACTION(jelm, jvx_make_no);
	}
	jelmlst.addConsumeElement(jelm);

	JVX_CREATE_COMPONENT_SEQ_ELEMENT_ASSOC_MODE(jelm, jvx_cbitfield2Hexstring(assoc_mode));
	jelmlst.addConsumeElement(jelm);
	/*
	response += "\t\tStep #";
	response += jvx_size2String(j);
	response += ":";
	response += "\"";
	response += fldStr.std_str();
	response += "\"";
	response += ":";
	response += jvxSequencerElementType_txt(elementTp);
	response += ":";
	response += jvxComponentIdentification_txt(targetTp);
	response += ":";
	response += jvx_size2String(fId);
	response += ":";
	response += jvx_size2String(timeout_ms);
	response += ":";
	response += "\"";
	response += fldStr_lab.std_str();
	response += "\"";
	response += ":";
	response += "\"";
	response += fldStr_lab_true.std_str();
	response += "\"";
	response += ":";
	response += "\"";
	response += fldStr_lab_false.std_str();
	response += "\"";
	response += "\n";
	*/
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_single_component(
	const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, 
	const std::string& addArg,
	jvxSize off, CjvxJsonElementList& jelmlst_ret)
{
	jvxSize num, i, j;
	jvxSize idS = JVX_SIZE_UNSELECTED;
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	jvxApiString fldStr;
	jvxErrorType resL;
	// jvxState stat = JVX_STATE_NONE;
	jvxFlagTag theAccessFlags = JVX_ACCESS_ROLE_DEFAULT;
	jvxCallManagerProperties callGate;
	std::string errTxt;
	if (hHost)
	{
		jvxBool callWithoutCommand = true;
		jvxComponentIdentification tp((jvxComponentType)dh_command.subaddress, dh_command.subsubaddress, dh_command.subsubsubaddress);
		if (args.size() > off)
		{
			std::string actionString;
			if (args.size() > off)
			{
				actionString = args[off];
			}
			if (actionString == "property")
			{
				jvxBool content_only = false;
				jvxBool compact = false;
				for(j = 0; j < addArg.size(); j++)
				{
					switch (addArg[j])
					{
						case 'c':
							content_only = true;
							break;
						case 'C':
							compact = true;
							break;
						default:
							// Unknown option ignored
							break;
						
					}					
				}

				std::string idtarget;
				
				if (args.size() > off + 1)
				{
					idtarget = args[off + 1];
				}
				
				std::string propBr = jvx_parseStringFromBrackets(idtarget, '[', ']');
				if (propBr.empty())
				{
					res = CjvxHostJsonCommandsShow::show_property_component(
						tp, idtarget, args, off+1, jelmlst_ret, content_only, compact, errTxt);
					if (res != JVX_NO_ERROR)
					{
						JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
					}
				}
				else
				{
					res = CjvxHostJsonCommandsShow::show_property_component_list(
						tp, propBr, jelmlst_ret, content_only, compact, errTxt);
					if (res != JVX_NO_ERROR)
					{
						JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
					}
				}
				return res;
			}
			else
			{
				std::string filter_prop_str;
				std::vector<std::string> filter_exprs;

				std::string filter_purpose;
				if (actionString == "properties")
				{
					jvx_propertyReferenceTriple theTriple;

					if (args.size() > 2)
					{
						filter_purpose = args[2];
					}
					else
					{
						filter_purpose = "list";
					}

					if (args.size() > 3)
					{
						filter_prop_str = args[3]; // return content for only one dedicated property
					}


					hHost->selection_component(tp, &idS);
					if (JVX_CHECK_SIZE_SELECTED(idS))
					{
						IjvxObject* theObject = NULL;
						jvxHandle* theHdl = NULL;
						//jelmlst_ret
						CjvxJsonElement jelm_onesec;

						/*
						JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_onesec, jvxComponentIdentification_txt(tp));
						jelmlst_ret.addConsumeElement(jelm_onesec);
						*/

						res = show_property_list(tp, filter_purpose, filter_prop_str, jelmlst_ret, errTxt);
						if (res != JVX_NO_ERROR)
						{
							JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
						}
						callWithoutCommand = false;
					}
					else
					{
						errTxt = "No component of type <";
						errTxt += jvxComponentType_txt(tp.tp);
						errTxt += "> has been selected.";
						JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
					}
				}
				else
				{
					if (actionString == "slots")
					{
						jvxSize slotId = JVX_SIZE_UNSELECTED;
						if (args.size() > (off + 1))
						{
							jvxBool err = false;
							slotId = jvx_string2Size(args[off + 1], err);
						}

						CjvxJsonElement jelm_slots;
						jvxSize szS = 0;
						jvxSize szSS = 0;
						jvxComponentType cTp = JVX_COMPONENT_UNKNOWN;
						jvxComponentType pTp = JVX_COMPONENT_UNKNOWN;
						hHost->number_slots_component_system(tp, &szS, &szSS, &pTp, &cTp);
						if (pTp != JVX_COMPONENT_UNKNOWN)
						{
							CjvxJsonArray jelmarr;
							for (i = 0; i < szS; i++)
							{
								if (JVX_CHECK_SIZE_SELECTED(slotId))
								{
									if (i != slotId)
									{
										continue;
									}
								}
								CjvxJsonElementList jelmlst_entries;
								CjvxJsonElement jelm_entry;
								CjvxJsonArrayElement jarr_entry;

								IjvxObject* obj = nullptr;
								IjvxTechnology* tPtr = nullptr;
								jvxComponentIdentification tpGet = pTp;
								tpGet.slotid = i;

								hHost->request_object_selected_component(tpGet, &obj);
								if (obj)
								{
									jvxApiString astr;

									jelm_entry.makeAssignmentSize("slotid", i);
									jelmlst_entries.addConsumeElement(jelm_entry);

									tPtr = castFromObject<IjvxTechnology>(obj);
									assert(tPtr);
									tPtr->description(&astr);

									jelm_entry.makeAssignmentString("description", astr.std_str());
									jelmlst_entries.addConsumeElement(jelm_entry);

									JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_entry, jvxComponentIdentification_txt(tpGet));
									jelmlst_entries.addConsumeElement(jelm_entry);

									// =========================================================================

									// All subslots
									hHost->number_slots_component_system(tp, nullptr, &szSS, nullptr, nullptr);

									CjvxJsonArray subjelmarr;
									jvxComponentIdentification tpGetSub = tp;
									tpGetSub.slotid = tpGet.slotid;

									for (j = 0; j < szSS; j++)
									{
										CjvxJsonElementList jelmlst_subentries;
										CjvxJsonElement jelm_subentry;
										CjvxJsonArrayElement jarr_subentry;

										IjvxObject* obj = nullptr;
										IjvxDevice* dPtr = nullptr;
										tpGetSub.slotsubid = i;

										hHost->request_object_selected_component(tpGetSub, &obj);
										if (obj)
										{
											jvxApiString astr;

											jelm_subentry.makeAssignmentSize("subslotid", i);
											jelmlst_subentries.addConsumeElement(jelm_subentry);

											dPtr = castFromObject<IjvxDevice>(obj);
											assert(dPtr);
											dPtr->description(&astr);

											jelm_subentry.makeAssignmentString("description", astr.std_str());
											jelmlst_subentries.addConsumeElement(jelm_subentry);

											JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_subentry, jvxComponentIdentification_txt(tpGet));
											jelmlst_subentries.addConsumeElement(jelm_subentry);
										}
										jarr_subentry.makeSection(jelmlst_subentries);
										subjelmarr.addConsumeElement(jarr_subentry);

									} // for (j = 0; j < szSS; j++)
									jelm_entry.makeArray("subslots", subjelmarr);
									jelmlst_entries.addConsumeElement(jelm_entry);
								}
								jarr_entry.makeSection(jelmlst_entries);
								jelmarr.addConsumeElement(jarr_entry);

							} // for (i = 0; i < szS; i++)
							jelm_slots.makeArray("slots", jelmarr);
							jelmlst_ret.addConsumeElement(jelm_slots);

						}
						else
						{
							//errTxt = "This command can only be called for technology components.";
							//JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
							CjvxJsonArray jelmarr;
							for (i = 0; i < szS; i++)
							{
								CjvxJsonElementList jelmlst_entries;
								CjvxJsonElement jelm_entry;
								CjvxJsonArrayElement jarr_entry;

								IjvxObject* obj = nullptr;
								IjvxTechnology* tPtr = nullptr;
								IjvxNode* nPtr = nullptr;
								jvxComponentIdentification tpGet = tp;
								tpGet.slotid = i;

								hHost->request_object_selected_component(tpGet, &obj);
								if (obj)
								{
									jvxApiString astr;

									jelm_entry.makeAssignmentSize("slotid", i);
									jelmlst_entries.addConsumeElement(jelm_entry);

									tPtr = castFromObject<IjvxTechnology>(obj);
									nPtr = castFromObject<IjvxNode>(obj);
									if (tPtr)
									{
										tPtr->description(&astr);
									}
									else
									{
										assert(nPtr);
										nPtr->description(&astr);
									}

									jelm_entry.makeAssignmentString("description", astr.std_str());
									jelmlst_entries.addConsumeElement(jelm_entry);

									JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_entry, jvxComponentIdentification_txt(tpGet));
									jelmlst_entries.addConsumeElement(jelm_entry);
								}

								jarr_entry.makeSection(jelmlst_entries);
								jelmarr.addConsumeElement(jarr_entry);
							}// for (i = 0; i < szS; i++)
							jelm_slots.makeArray("slots", jelmarr);
							jelmlst_ret.addConsumeElement(jelm_slots);
						}
						callWithoutCommand = false;
					}
					else
					{
						if (actionString == "devices")
						{
							jvxSize slotId = JVX_SIZE_UNSELECTED;
							if (args.size() > (off + 1))
							{
								jvxBool err = false;
								slotId = jvx_string2Size(args[off + 1], err);
							}

							CjvxJsonElement jelm_devices;
							IjvxObject* obj = nullptr;
							IjvxTechnology* tPtr = nullptr;
							jvxComponentTypeClass cls = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
							
							jvxComponentIdentification tpGet;
							/*
							* I think, we do not need the folowing code: each device can only 
							* be associated to exactly one slot. Therefore, we can use the information about the 
							* associated slot directly.
							* 
							* @(1)
							* 
							tpGet.tp = (jvxComponentType)(tp.tp + 1);
							tpGet.slotid = tp.slotid;
							jvxSize szSS = 0;
							hHost->number_slots_component_system(tpGet, nullptr, &szSS);
							std::vector<jvxSize> slotIds;
							for (j = 0; j < szSS; j++)
							{
								jvxSize selId = JVX_SIZE_UNSELECTED;
								tpGet.slotsubid = j;
								hHost->selection_component(tpGet, &selId);
								slotIds.push_back(selId);
							}
							*/							
							res = hHost->request_object_selected_component(tp, &obj);
							if ((res == JVX_NO_ERROR) && obj)
							{
								tPtr = castFromObject<IjvxTechnology>(obj);
								if (tPtr)
								{
									tPtr->number_devices(&num);
									CjvxJsonArray jelmarr;
									for (i = 0; i < num; i++)
									{
										CjvxJsonElementList jelmlst_entries;
										CjvxJsonElement jelm_entry;
										CjvxJsonArrayElement jarr_entry;
										jvxState stat = JVX_STATE_NONE;

										jvxApiString astr;
										jvxComponentIdentification tpGet;
										jvxDeviceCapabilities caps;
										tPtr->description_device(i, &astr);
										jelm_entry.makeAssignmentString("description", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										tPtr->descriptor_device(i, &astr);
										jelm_entry.makeAssignmentString("descriptor", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										tPtr->status_device(i, &stat); 
										JVX_CREATE_COMPONENT_STATE(jelm_entry, jvxState_txt(stat));
										jelmlst_entries.addConsumeElement(jelm_entry);

										tPtr->capabilities_device(i, caps);
										tPtr->location_info_device(i, tpGet);
										
										show_device_capabilities(jelm_entry, caps);
										jelmlst_entries.addConsumeElement(jelm_entry);

										/*
										* The following code no longer in use. Check the comment above (@1)
										for (j = 0; j < slotIds.size(); j++)
										{
											if (slotIds[j] == i)
											{
												CjvxJsonArrayElement jelmarrelmslot;
												jelmarrelmslot.makeAssignmentInt(j);
												jelmarrslots.addConsumeElement(jelmarrelmslot);
											}
										}
										*/
										CjvxJsonArray jelmarrslots;
										if (JVX_CHECK_SIZE_SELECTED(tpGet.slotsubid))
										{
											CjvxJsonArrayElement jelmarrelmslot;
											jelmarrelmslot.makeAssignmentSize(tpGet.slotsubid);
											jelmarrslots.addConsumeElement(jelmarrelmslot);
										}

										jvxSize numArrElm = 0;
										jelmarrslots.numElements(&numArrElm);
										if (numArrElm)
										{
											jelm_entry.makeArray("slots", jelmarrslots);
											jelmlst_entries.addConsumeElement(jelm_entry);
										}
										jarr_entry.makeSection(jelmlst_entries);
										jelmarr.addConsumeElement(jarr_entry);
									}
									jelm_devices.makeArray("devices", jelmarr);
									jelmlst_ret.addConsumeElement(jelm_devices);
								}
								else
								{
									res = JVX_ERROR_UNSUPPORTED;
									errTxt = "Error accessing devices for component type <";
									errTxt += jvxComponentType_txt(tp.tp);
									errTxt += ">, reason: ";
									errTxt += jvxErrorType_descr(res);
									JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);
								}
							
							}
							else
							{
								errTxt = "Target technology with slotid <" + jvx_size2String(tp.slotid) + "> not accessible for component type <";
								errTxt += jvxComponentType_txt(tp.tp);
								JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);
							}
							
							callWithoutCommand = false;
						}
						else
						{
							if (actionString == "options")
							{
								jvxSize slotId = JVX_SIZE_UNSELECTED;
								if (args.size() > (off + 1))
								{
									jvxBool err = false;
									slotId = jvx_string2Size(args[off + 1], err);
								}

								CjvxJsonElement jelm_options;
								jvxSize szS = 0;
								jvxSize szSS = 0;
								jvxComponentType pTp = JVX_COMPONENT_UNKNOWN;
								hHost->number_slots_component_system(tp, &szS, &szSS, &pTp);
								if(pTp == JVX_COMPONENT_UNKNOWN)
								{
									jvxComponentIdentification tpGet;
									tpGet.tp = tp.tp;
									tpGet.slotsubid = 0;
									std::vector<jvxSize> slotIds;
									for (j = 0; j < szS; j++)
									{
										jvxSize selId = JVX_SIZE_UNSELECTED;
										tpGet.slotid = j;
										hHost->selection_component(tpGet, &selId);
										slotIds.push_back(selId);
									}

									hHost->number_components_system(tp, &num);
									CjvxJsonArray jelmarr;
									for (i = 0; i < num; i++)
									{
										CjvxJsonElementList jelmlst_entries;
										CjvxJsonElement jelm_entry;
										CjvxJsonArrayElement jarr_entry;

										jvxApiString astr;
										jvxComponentIdentification tpGet;

										hHost->description_component_system(tp, i, &astr);
										jelm_entry.makeAssignmentString("description", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										hHost->descriptor_component_system(tp, i, &astr);
										jelm_entry.makeAssignmentString("descriptor", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										/*
										* The status would be slot specific, therefore we do not return it here
										hHost->status_component_system(i, &stat);
										JVX_CREATE_COMPONENT_STATE(jelm_entry, jvxState_txt(stat));
										jelmlst_entries.addConsumeElement(jelm_entry);
										*/
										CjvxJsonArray jelmarrslots;
										for (j = 0; j < slotIds.size(); j++)
										{
											if (slotIds[j] == i)
											{
												CjvxJsonArrayElement jelmarrelmslot;
												jelmarrelmslot.makeAssignmentSize(j);
												jelmarrslots.addConsumeElement(jelmarrelmslot);
											}
										}

										jvxSize numArrElm = 0;
										jelmarrslots.numElements(&numArrElm);
										if (numArrElm)
										{
											jelm_entry.makeArray("slots", jelmarrslots);
											jelmlst_entries.addConsumeElement(jelm_entry);
										}

										jarr_entry.makeSection(jelmlst_entries);
										jelmarr.addConsumeElement(jarr_entry);
									}
									jelm_options.makeArray("options", jelmarr);
									jelmlst_ret.addConsumeElement(jelm_options);
								} // if(pTp == JVX_COMPONENT_UNKNOWN)
								else
								{
									jvxComponentIdentification tpGet;
									tpGet.tp = tp.tp;
									tpGet.slotid = tp.slotid;
									std::vector<jvxSize> slotSubIds;
									for (j = 0; j < szSS; j++)
									{
										jvxSize selId = JVX_SIZE_UNSELECTED;
										tpGet.slotsubid = j;
										hHost->selection_component(tpGet, &selId);
										slotSubIds.push_back(selId);
									}

									hHost->number_components_system(tp, &num);
									CjvxJsonArray jelmarr;
									for (i = 0; i < num; i++)
									{
										CjvxJsonElementList jelmlst_entries;
										CjvxJsonElement jelm_entry;
										CjvxJsonArrayElement jarr_entry;

										jvxApiString astr;
										jvxComponentIdentification tpGet;
										jvxDeviceCapabilities caps;
										hHost->description_component_system(tp, i, &astr);
										jelm_entry.makeAssignmentString("description", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										hHost->descriptor_component_system(tp, i, &astr);
										jelm_entry.makeAssignmentString("descriptor", astr.std_str());
										jelmlst_entries.addConsumeElement(jelm_entry);

										CjvxJsonArray jelmarrslots;
										for (j = 0; j < slotSubIds.size(); j++)
										{
											if (slotSubIds[j] == i)
											{
												CjvxJsonArrayElement jelmarrelmslot;
												jelmarrelmslot.makeAssignmentSize(j);
												jelmarrslots.addConsumeElement(jelmarrelmslot);
											}
										}

										jelm_entry.makeArray("subslots", jelmarrslots);
										jelmlst_entries.addConsumeElement(jelm_entry);

										jarr_entry.makeSection(jelmlst_entries);
										jelmarr.addConsumeElement(jarr_entry);
									}
									jelm_options.makeArray("options", jelmarr);
									jelmlst_ret.addConsumeElement(jelm_options);

								}
								callWithoutCommand = false;
							}
							else
							{
								/*
								* It is not a valid sub command but still, it could be an additional property
								*
								errTxt = "Invalid operation " + args[1] + " when addressing component type <";
								errTxt += jvxComponentType_txt(tp.tp);
								JVX_CREATE_ERROR_RETURN(jelmlst_ret, JVX_ERROR_ELEMENT_NOT_FOUND, errTxt);
								*/
							}
						}
					}
				}
			}
		}

		if (callWithoutCommand)
		{
			// If we are here, there is either no command or we add properties to address

			// Show only the names of the available components - and highlight the selected one
			// = tpAll[dh_command.subaddress];
			res = hHost->selection_component(tp, &idS);
			if ((res == JVX_NO_ERROR) || (res == JVX_ERROR_ID_OUT_OF_BOUNDS))
			{
				res = hHost->number_components_system(tp, &num);
				if (res == JVX_NO_ERROR)
				{
					CjvxJsonArray jarr_comps;
					CjvxJsonElement jelm_comps;
					for (i = 0; i < num; i++)
					{
						CjvxJsonElementList jelmlst_entries;
						CjvxJsonElement jelm_entry;
						CjvxJsonArrayElement jarr_entry;

						JVX_CREATE_COMPONENT_TYPE(jelm_entry, jvxComponentType_txt((jvxSize)tp.tp));
						jelmlst_entries.addConsumeElement(jelm_entry);

						if (idS == i)
						{
							JVX_CREATE_COMPONENT_SELECTED(jelm_entry, JVX_INDICATE_TRUE_SHORT);
						}
						else
						{
							JVX_CREATE_COMPONENT_SELECTED(jelm_entry, JVX_INDICATE_FALSE_SHORT);
						}
						jelmlst_entries.addConsumeElement(jelm_entry);

						hHost->description_component_system(tp, i, &fldStr);
						JVX_CREATE_COMPONENT_DESCRIPTION(jelm_entry, fldStr.std_str());
						jelmlst_entries.addConsumeElement(jelm_entry);

						hHost->descriptor_component_system(tp, i, &fldStr);
						JVX_CREATE_COMPONENT_DESCRIPTOR(jelm_entry, fldStr.std_str());
						jelmlst_entries.addConsumeElement(jelm_entry);

						stat = JVX_STATE_NONE;
						if (idS == i)
						{
							hHost->state_selected_component(tp, &stat);

							jvxBool addedProp = false;
							CjvxJsonElementList jelmlst_props;
							CjvxJsonElementList jelmlst_props_cont;

							if (args.size() > off)
							{
								const std::vector<std::string>* argsInPtr = &args;
								jvxSize off = 1;
								std::string props = args[off];
								if (!props.empty())
								{
									// Add property in brackets?
									std::string propBr = jvx_parseStringFromBrackets(props, '[', ']');

									if (propBr.empty())
									{
										// Single property
										jvxErrorType resP = this->show_property_component(tp, props, args, off, jelmlst_props_cont,
											false, false, errTxt);
										if (resP == JVX_NO_ERROR)
										{
											jelm_entry.makeSection(props, jelmlst_props_cont);
											jelmlst_props.addConsumeElement(jelm_entry);

											jelm_entry.makeSection("properties", jelmlst_props);
											jelmlst_entries.addConsumeElement(jelm_entry);
										}
										else
										{
											JVX_CREATE_ERROR_RETURN(jelmlst_ret, resP, errTxt);
										}
							
										//elm_entry.makeAssignmentString("return_code", jvxErrorType_str[resP].friendly);
										//jelmlst_props_cont.insertConsumeElementFront(jelm_entry);

									}
									else
									{
										jvxErrorType resP = this->show_property_component_list(tp, propBr, jelmlst_entries, false, false, errTxt);
										if (resP == JVX_NO_ERROR)
										{
											jelm_entry.makeSection(props, jelmlst_props_cont);
											jelmlst_props.addConsumeElement(jelm_entry);

											jelm_entry.makeSection("properties", jelmlst_props);
											jelmlst_entries.addConsumeElement(jelm_entry);
										}
										else
										{
											JVX_CREATE_ERROR_RETURN(jelmlst_ret, resP, errTxt);
										}
									}
								}
							}
						}

						JVX_CREATE_COMPONENT_STATE(jelm_entry, jvxState_txt(stat));
						jelmlst_entries.addConsumeElement(jelm_entry);

						jarr_entry.makeSection(jelmlst_entries);
						jarr_comps.addConsumeElement(jarr_entry);
					}
					jelm_comps.makeArray("all_components", jarr_comps);
					jelmlst_ret.addConsumeElement(jelm_comps);
				}
				else
				{
					errTxt = "Error accessing number of components for component type <";
					errTxt += jvxComponentType_txt(tp.tp);
					errTxt += ">, reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);

				}
			}
			else
			{
				if (JVX_ERROR_ELEMENT_NOT_FOUND)
				{
					errTxt = "No instance of component <";
					errTxt += jvxComponentType_txt(tp.tp);
					errTxt += "> has been selected.";
					JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);
				}
				else
				{
					errTxt = "Error accessing component type <";
					errTxt += jvxComponentType_txt(tp.tp);
					errTxt += ">, reason: ";
					errTxt += jvxErrorType_descr(res);
					JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);

				}
			}
		}
		
		
		/*
		* I have removed this here: why would we switch the active selection, we may address each
		* component individually anyway
		if (res == JVX_NO_ERROR)
		{
			tpAllRef[dh_command.subaddress] = tp;
		}
		*/
		return res;
	}

	return JVX_ERROR_INTERNAL;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_hosttype_handler(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string message;
	jvxComponentIdentification cpType;
	bool multObj = false;
	jvxCBool cout_mess = c_false;
	IjvxProperties* theProps = NULL;
	std::string nm = "ERROR";
	jvxBool isValid = false;
	jvxSize id = 0;
	jvxApiString  fldStr;
	

	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = this->hHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if ((res == JVX_NO_ERROR) && theTypeHandler)
	{
		
		jvxSize numH = 0;
		CjvxJsonElement jelm_oneType; 
		CjvxJsonArray jarr;
		theTypeHandler->number_types_host(&numH, jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY);
		for (i = 0; i < numH; i++)
		{
			CjvxJsonArrayElement jarrelm;
			jvxComponentType tpLoc[2] = { JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_UNKNOWN };
			res = theTypeHandler->description_type_host(i, &fldStr, NULL, tpLoc, 2, jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY);
			if (res == JVX_NO_ERROR) 
			{
				CjvxJsonElementList jelmlstl;
				CjvxJsonElement jelml;

				JVX_CREATE_COMPONENT_DESCRIPTION(jelml, fldStr.std_str());
				jelmlstl.addConsumeElement(jelml);

				JVX_CREATE_COMPONENT_TYPE(jelml, jvxComponentType_txt(tpLoc[0]));
				jelmlstl.addConsumeElement(jelml);

				JVX_CREATE_COMPONENT_TYPE_SEC(jelml, jvxComponentType_txt(tpLoc[1]));
				jelmlstl.addConsumeElement(jelml);

				jarrelm.makeSection(jelmlstl);
				jarr.addConsumeElement(jarrelm);
			}
			else
			{
				CjvxJsonElementList jelmlstl;
				JVX_CREATE_ERROR_RETURN(jelmlstl, res, ("The hosttype handler description for <JVX_COMPONENT_TYPE_TECHNOLOGY> could not be obtained, reason: " + (std::string)(jvxErrorType_descr(res))));
			}
		}

		JVX_CREATE_TECHNOLOGY_HOSTTYPE(jelm_oneType, jarr);
		jelmret.addConsumeElement(jelm_oneType);

		numH = 0;
		
		// jelm_oneType and jarr should be reset

		theTypeHandler->number_types_host(&numH, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE);
		for (i = 0; i < numH; i++)
		{
			CjvxJsonArrayElement jarrelm;
			jvxComponentType tpLoc = JVX_COMPONENT_UNKNOWN;
			res = theTypeHandler->description_type_host(i, &fldStr, NULL, &tpLoc, 1, jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE);
			if (res == JVX_NO_ERROR) 
			{
				CjvxJsonElementList jelmlstl;
				CjvxJsonElement jelml;

				JVX_CREATE_COMPONENT_DESCRIPTION(jelml, fldStr.std_str());
				jelmlstl.addConsumeElement(jelml);

				JVX_CREATE_COMPONENT_TYPE(jelml, jvxComponentType_txt(tpLoc));
				jelmlstl.addConsumeElement(jelml);

				//message = textMessagePrioToString(("Host type handler entry " + jvx_size2String(i) + " for " + " <JVX_COMPONENT_TYPE_NODE>:" + oneLine).c_str(), JVX_REPORT_PRIORITY_INFO);
				//postMessage_inThread(message);
				jarrelm.makeSection(jelmlstl);
				jarr.addConsumeElement(jarrelm);
			}
			else
			{
				CjvxJsonElementList jelmlstl;
				JVX_CREATE_ERROR_RETURN(jelmlstl, res, ("The hosttype handler description for <JVX_COMPONENT_TYPE_NODE> could not be obtained, reason: " + (std::string)(jvxErrorType_descr(res))));

			}

		}
		JVX_CREATE_TECHNOLOGY_NODETYPE(jelm_oneType, jarr);
		jelmret.addConsumeElement(jelm_oneType);

		this->hHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}
	else
	{
		CjvxJsonElementList jelmlstl;
		JVX_CREATE_ERROR_RETURN(jelmlstl, res, ("The hosttype handler interface could not be obtained, reason: " + (std::string)(jvxErrorType_descr(res))));

	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_config_line(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlstret)
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	std::string message;
	jvxComponentIdentification cpType;
	bool multObj = false;
	jvxCBool cout_mess = c_false;
	IjvxProperties* theProps = NULL;
	std::string nm = "ERROR";
	jvxBool isValid = false;
	jvxSize id = 0;
	CjvxJsonElement jelm;
	IjvxConfigurationLine* theConfigLines = NULL;
	res = this->hHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLines));
	if ((res == JVX_NO_ERROR) && theConfigLines)
	{
		CjvxJsonArray jarr_lines;

		jvxSize numL = 0;
		theConfigLines->number_lines(&numL);
		for (i = 0; i < numL; i++)
		{
			CjvxJsonArrayElement jarr_line;
			CjvxJsonElementList jelmlst_entry;
			CjvxJsonElement jelm_entry;

			std::string oneLine = "";
			jvxSize numS = 0;
			jvxSize numP = 0;
			jvxSize idP = 0;
			jvxComponentIdentification tp;
			theConfigLines->master_line(i, &tp);
			oneLine = jvxComponentIdentification_txt(tp);
			oneLine += " -> [";

			JVX_CREATE_COMPONENT_IDENTIFICATION(jelm_entry, jvxComponentIdentification_txt(tp));
			jelmlst_entry.addConsumeElement(jelm_entry);

			theConfigLines->number_slaves_line(tp, &numS);

			CjvxJsonArray jarr_slaves;
			for (j = 0; j < numS; j++)
			{
				CjvxJsonArrayElement jarr_oslaves;
				jvxComponentIdentification tpS;
				theConfigLines->slave_line_master(tp, j, &tpS);
				oneLine += jvxComponentIdentification_txt(tpS);
				if (j != numS - 1)
				{
					oneLine += ", ";
				}
				jarr_oslaves.makeString(jvxComponentIdentification_txt(tpS));
				jarr_slaves.addConsumeElement(jarr_oslaves);
			}

			JVX_CREATE_CONFIGLINE_SLAVE(jelm_entry, jarr_slaves);
			jelmlst_entry.addConsumeElement(jelm_entry);

			oneLine += "] <";
			theConfigLines->number_property_ids_line(tp, &numP);
			for (j = 0; j < numP; j++)
			{
				CjvxJsonArrayElement jarr_oslaves;
				theConfigLines->property_id_line_master(tp, j, &idP);
				nm = "ERROR";
				jvx_findPropertyNameId(idP, nm);
				oneLine += nm;
				if (j != numP - 1)
				{
					oneLine += ", ";
				}
				jarr_oslaves.makeString(nm);
				jarr_slaves.addConsumeElement(jarr_oslaves);
			}

			JVX_CREATE_CONFIGLINE_PROPERTY(jelm_entry, jarr_slaves);
			jelmlst_entry.addConsumeElement(jelm_entry);
			oneLine += ">)";

			jarr_line.makeSection(jelmlst_entry);
			jarr_lines.addConsumeElement(jarr_line);

		}
		this->hHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLines));
		jelm.makeArray("cunfiguration_lines", jarr_lines);
		jelmlstret.addConsumeElement(jelm);
	}
	else
	{
		JVX_CREATE_ERROR_RETURN(jelmlstret, res, ("The config line interface could not be obtained, reason: " + (std::string)(jvxErrorType_descr(res))));
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_alive(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst_ret)
{
	CjvxJsonElement jelm_result;
	jelm_result.makeAssignmentString("alive", jvx_make_yes);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::output_sequencer_status(CjvxJsonElementList& jelmlst)
{
	IjvxSequencer* sequencer = NULL;
	jvxApiString  fldStr;
	jvxApiString  fldStrLab;
	CjvxJsonElement jelm;
	jvxErrorType res = hHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&sequencer));
	if ((res == JVX_NO_ERROR) && sequencer)
	{
		jvxState stat = JVX_STATE_NONE;
		jvxSequencerStatus sstat = JVX_SEQUENCER_STATUS_NONE;
		jvxSize idxSeq = JVX_SIZE_UNSELECTED;
		jvxSequencerElementType jvxSeq = JVX_SEQUENCER_TYPE_COMMAND_NONE;
		jvxSequencerQueueType qTp = JVX_SEQUENCER_QUEUE_TYPE_NONE;
		jvxSize idxStep = JVX_SIZE_UNSELECTED;
		jvxBool loop = false;

		sequencer->state_sequencer(&stat);
		sequencer->status_process(&sstat, &idxSeq, &qTp, &idxStep, &loop);

		JVX_CREATE_SEQUENCER_STATE(jelm, jvxState_txt(stat));
		jelmlst.addConsumeElement(jelm);

		JVX_CREATE_SEQUENCER_STATUS(jelm, jvxSequencerStatus_txt(sstat));
		jelmlst.addConsumeElement(jelm);

		JVX_CREATE_SEQUENCER_QUEUE_TYPE(jelm, jvxSequencerQueueType_txt(qTp));
		jelmlst.addConsumeElement(jelm);

		if (JVX_CHECK_SIZE_SELECTED(idxSeq))
		{
			sequencer->description_sequence(idxSeq, &fldStr, &fldStrLab, NULL, NULL, NULL);
			JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_DESCRIPTION(jelm, fldStr.std_str());
			jelmlst.addConsumeElement(jelm);

			JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_LABEL(jelm, fldStrLab.std_str());
			jelmlst.addConsumeElement(jelm);

			if (JVX_CHECK_SIZE_SELECTED(idxStep))
			{
				sequencer->description_step_sequence(idxSeq, idxStep, qTp, &fldStr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_STEP(jelm, fldStr.std_str());
				jelmlst.addConsumeElement(jelm);
			}
			else
			{
				JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_STEP(jelm, "--");
				jelmlst.addConsumeElement(jelm);
			}
		}
		else
		{
			JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_DESCRIPTION(jelm, "--");
			jelmlst.addConsumeElement(jelm);
			JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_LABEL(jelm, "--");
			jelmlst.addConsumeElement(jelm);
			JVX_CREATE_SEQUENCER_CURRENT_SEQUENCE_STEP(jelm, "--");
			jelmlst.addConsumeElement(jelm);
		}

		hHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(sequencer));
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
}

jvxErrorType
CjvxHostJsonCommandsShow::process_command_abstraction(
	const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
	const std::string& addArg,
	jvxSize off, CjvxJsonElementList& jsec, jvxBool* systemUpdate)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (dh_command.family)
	{
	case JVX_DRIVEHOST_FAMILY_SHOW:
		switch (dh_command.address)
		{
		case JVX_DRIVEHOST_ADRESS_COMPONENTS:
			res = show_components(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_CONNECTIONS:
			res = show_connections(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_CONNECTION_RULES:
			res = show_connection_rules(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_SEQUENCER:
			res = show_sequencer(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_HOSTTYPEHANDLER:
			res = show_hosttype_handler(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_SYSTEM:
			res = show_system(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_CONFIGLINE:
			res = show_config_line(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_SINGLE_COMPONENT:
			res = show_single_component(dh_command, args, addArg, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_VERSION:
			res = show_version(dh_command, args, off, jsec);
			break;
		case JVX_DRIVEHOST_ADRESS_ALIVE:
			res = show_alive(dh_command, args, off, jsec);
			break;
		default:
			return JVX_ERROR_UNSUPPORTED;
			break;
			/*
			case JVX_DRIVEHOST_ADRESS_CONNECTION_DROPZONE:
				res = show_dropzone(dh_command, args, off, jsec);
				break;
			case JVX_DRIVEHOST_ADRESS_CONFIG:
				res = show_current_config(dh_command, args, off, jsec);
				break;
			*/
		}
		break;	

	default:
		return JVX_ERROR_UNSUPPORTED;
	}

	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_current_config(
	const oneDrivehostCommand& dh_command, 
	const std::vector<std::string>& args, 
	jvxSize off, CjvxJsonElementList& jelmlst_ret)
{
	CjvxJsonElement jelm_result;
	
	jvxBool produce_numdigs = false;
	jvxBool produce_propb64 = false;
	jvxBool produce_propcompact = false;

	jvxErrorType res = JVX_NO_ERROR;
	if (dh_command.family == JVX_DRIVEHOST_FAMILY_SHOW)
	{
		if (dh_command.address == JVX_DRIVEHOST_ADRESS_CONFIG)
		{
			if (args.size() <= 1)
			{
				produce_propb64 = true;
				produce_numdigs = true;
				produce_propcompact = true;
			}
			else
			{
				if (args.size() > 1)
				{
					if (args[1] == "prop_base64")
					{
						produce_propb64 = true;
					}
					else if (args[1] == "num_digits")
					{
						produce_numdigs = true;
					}
					else if (args[1] == "get_property_compact_mode")
					{
						produce_propcompact = true;
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
						JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, ("Argument <" + args[1] + "> is not a valid option."));
					}
				}
			}
			if (res == JVX_NO_ERROR)
			{
				if (produce_numdigs)
				{
					jelm_result.makeAssignmentSize("num_digits", config_show.numdigits);
					jelmlst_ret.addConsumeElement(jelm_result);
				}
				if (produce_propb64)
				{
					if (config_show.outputPropertyFieldsBase64)
					{
						jelm_result.makeAssignmentString("prop_base64", jvx_make_yes);
					}
					else
					{
						jelm_result.makeAssignmentString("prop_base64", jvx_make_no);
					}
					jelmlst_ret.addConsumeElement(jelm_result);
				}
				if (produce_propcompact)
				{
					if (config_show.get_property_compact)
					{
						jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_yes);
					}
					else
					{
						jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_no);
					}
					jelmlst_ret.addConsumeElement(jelm_result);
				}
			}
		}
	}

	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string actionString;
	std::string errTxt;
	jvxSize szSlots = 0;
	jvxSize szSlotsub = 0;
	CjvxJsonElement jelm_result;
	// Arg1: add sequence, remove sequence, activate sequence, add step, remove step, modify step
	if (args.size() > off)
	{
		actionString = args[off];

		if (actionString == "get_property_compact_mode")
		{
			if (args.size() > 2)
			{
				// "set" operation
				if (jvx_check_yes(args[2]))
				{
					config_show.get_property_compact = c_true;
				}
				else if (jvx_check_no(args[2]))
				{
					config_show.get_property_compact = c_false;
				}
				else
				{
					res = JVX_ERROR_PARSE_ERROR;
					errTxt = "The requested config value <" + args[2] + "> is not known.";
					JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
				}
			}
			else
			{
				if (config_show.get_property_compact)
				{
					jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_yes);
				}
				else
				{
					jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_no);
				}
				jsec.insertConsumeElementFront(jelm_result);
			}
		}
		else if (actionString == "num_digits")
		{
			if (args.size() > 2)
			{
				// "set" operation
				jvxBool err = false;
				jvxSize newVal = jvx_string2Size(args[2], err);
				if (!err)
				{
					config_show.numdigits = newVal;
				}
				else
				{
					res = JVX_ERROR_PARSE_ERROR;
					errTxt = "The value <" + args[2] + "> is not a valid size.";
					JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
				}
			}
			else
			{
				jelm_result.makeAssignmentString("num_digits", jvx_size2String(config_show.numdigits));
				jsec.insertConsumeElementFront(jelm_result);
			}
		}
		
		else if (actionString == "prop_base64")
		{
			if (args.size() > 2)
			{
				// "set" operation
				if (jvx_check_yes(args[2]))
				{
					config_show.outputPropertyFieldsBase64 = c_true;
				}
				else if (jvx_check_no(args[2]))
				{
					config_show.outputPropertyFieldsBase64 = c_false;
				}
				else
				{
					res = JVX_ERROR_PARSE_ERROR;
					errTxt = "The requested config value <" + args[2] + "> is not known.";
					JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
				}
			}
			else
			{
				if (config_show.outputPropertyFieldsBase64)
				{
					jelm_result.makeAssignmentString("prop_base64", jvx_make_yes);
				}
				else
				{
					jelm_result.makeAssignmentString("prop_base64", jvx_make_no);
				}
				jsec.insertConsumeElementFront(jelm_result);
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			errTxt = "The requested operation <" + actionString + "> is not known.";
			JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
		}
	}
	else
	{		
		jsec.insertConsumeElementFront(jelm_result);
		if (config_show.get_property_compact)
		{
			jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_yes);
		}
		else
		{
			jelm_result.makeAssignmentString("get_property_compact_mode", jvx_make_no);
		}
		jsec.insertConsumeElementFront(jelm_result);
		if (config_show.outputPropertyFieldsBase64)
		{
			jelm_result.makeAssignmentString("prop_base64", jvx_make_yes);
		}
		else
		{
			jelm_result.makeAssignmentString("prop_base64", jvx_make_no);
		}
		jsec.insertConsumeElementFront(jelm_result);
		jelm_result.makeAssignmentString("num_digits", jvx_size2String(config_show.numdigits));
		jsec.insertConsumeElementFront(jelm_result);
	}

	return res;
}

jvxErrorType
CjvxHostJsonCommandsShow::show_device_capabilities(CjvxJsonElement& jelm_result, const jvxDeviceCapabilities& caps)
{
	CjvxJsonElementList jelm_lst;
	CjvxJsonElement jelm;
	
	jelm.makeAssignmentString("flow_type", jvxDeviceDataFlowType_txt(caps.flow));
	jelm_lst.addConsumeElement(jelm);

	jelm.makeAssignmentString("caps", jvxDeviceCapabilitiesType_txt(caps.capsFlags));
	jelm_lst.addConsumeElement(jelm);

	jelm.makeAssignmentString("sel", (caps.selectable? JVX_INDICATE_TRUE_SHORT : JVX_INDICATE_FALSE_SHORT));
	jelm_lst.addConsumeElement(jelm);

	jelm.makeAssignmentString("single", (caps.singleton ? JVX_INDICATE_TRUE_SHORT : JVX_INDICATE_FALSE_SHORT));
	jelm_lst.addConsumeElement(jelm);

	jelm.makeAssignmentString("flags", jvx_bitfield162String(caps.flags));
	jelm_lst.addConsumeElement(jelm);

	jelm_result.makeSection("device_caps", jelm_lst);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxHostJsonCommandsShow::requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	jvx_getReferencePropertiesObject(hHost, &theTriple, tp);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsShow::returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp)
{
	jvx_returnReferencePropertiesObject(hHost, &theTriple, tp);
	return JVX_NO_ERROR;
}