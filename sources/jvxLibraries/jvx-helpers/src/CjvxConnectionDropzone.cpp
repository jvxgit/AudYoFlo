#include "jvx-helpers.h"

CjvxConnectionDropzone::CjvxConnectionDropzone()
{
	jvx_reset_dropzone();
}

jvxErrorType 
CjvxConnectionDropzone::connect_process_from_dropzone(IjvxDataConnections* allConnections, 
	IjvxDataConnectionProcess* theProc JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxConnectionMasterFactory* theMasterFactory = NULL;
	IjvxConnectionMaster* theMaster = NULL;
	jvxBool errInBridge = false;
	std::list<lst_elm_bidges>::iterator elmB = lst_bridges.begin();
	std::list<lst_elm_master>::iterator elmMas = lst_masters.begin();
	
	// Is a master selected??
	if (JVX_CHECK_SIZE_UNSELECTED(id_select_master))
	{
		return JVX_ERROR_NOT_READY;
	}

	std::advance(elmMas, id_select_master);
	if (elmMas == lst_masters.end())
	{
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, 
			("Invalid master selection id <" + jvx_size2String(id_select_master) + ">.").c_str(),
			JVX_ERROR_INVALID_SETTING);

		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	allConnections->reference_connection_master_factory_uid(elmMas->identify.mas_fac_uid, &theMasterFactory);
	if (res != JVX_NO_ERROR)
	{
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Failed to get reference master factory.", res);
		goto exit_fail_I;
	}
	assert(theMasterFactory);

	res = theMasterFactory->reference_connector_master(elmMas->identify.mas_id, &theMaster);
	if (res != JVX_NO_ERROR)
	{
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Failed to get reference master.", res);
		goto exit_fail_II;
	}
	assert(theMaster);

	res = theProc->associate_master(theMaster);
	if (res != JVX_NO_ERROR)
	{
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, "Failed to associate master.", res);
		goto exit_fail_II;
	}

	for (; elmB != lst_bridges.end(); elmB++)
	{
		IjvxConnectorFactory* theConFacO = NULL;
		IjvxConnectorFactory* theConFacI = NULL;
		IjvxInputConnectorSelect* icon = NULL;
		IjvxOutputConnectorSelect* ocon = NULL;
		jvxSize uIdB = JVX_SIZE_UNSELECTED;

		res = allConnections->reference_connection_factory_uid(elmB->identify_out.fac_uid, &theConFacO);
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, 
				("Failed to get reference connector factory for output connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
			errInBridge = true;
			break;
		}

		assert(theConFacO);

		res = theConFacO->reference_output_connector(elmB->identify_out.ocon_id, &ocon);
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, 
				("Failed to get reference output connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
			resL = allConnections->return_reference_connection_factory(theConFacO);
			errInBridge = true;
			break;
		}
		assert(ocon);

		// ====================================================================================

		res = allConnections->reference_connection_factory_uid(elmB->identify_in.fac_uid, &theConFacI);
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, 
				("Failed to get reference connector factory for input connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
			resL = theConFacO->return_reference_output_connector(ocon);
			resL = allConnections->return_reference_connection_factory(theConFacO);
			errInBridge = true;
			break;
		}

		assert(theConFacI);

		res = theConFacI->reference_input_connector(elmB->identify_in.icon_id, &icon);
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, 
				("Failed to get reference input connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
			resL = theConFacO->return_reference_output_connector(ocon);
			resL = allConnections->return_reference_connection_factory(theConFacO);
			resL = allConnections->return_reference_connection_factory(theConFacI);
			errInBridge = true;
			break;
		}

		assert(icon);

		res = theProc->create_bridge(ocon, icon, elmB->bridge_name.c_str(), &elmB->uIdB, false, false, elmB->identify_out.oconTriggerId, elmB->identify_in.iconTrigerId); // Currently, no dedicated threads in "normal" connections
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb,
				("Failed to create bridge <" + elmB->bridge_name + ">.").c_str(), res);
			resL = theConFacI->return_reference_input_connector(icon); 
			resL = theConFacO->return_reference_output_connector(ocon);
			resL = allConnections->return_reference_connection_factory(theConFacO);
			resL = allConnections->return_reference_connection_factory(theConFacI);
			errInBridge = true;
			break;
		}

		resL = theConFacO->return_reference_output_connector(ocon);
		assert(resL == JVX_NO_ERROR);
		resL = allConnections->return_reference_connection_factory(theConFacO);
		assert(resL == JVX_NO_ERROR);
		resL = theConFacI->return_reference_input_connector(icon);
		assert(resL == JVX_NO_ERROR);
		resL = allConnections->return_reference_connection_factory(theConFacI);
		assert(resL == JVX_NO_ERROR);
		
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, ("Failed to create bridge <" + elmB->bridge_name + ">.").c_str(), res);
			errInBridge = true;
			break;
		}
	}

	if (errInBridge)
	{
		assert(res != JVX_NO_ERROR);
	}

	if (res == JVX_NO_ERROR)
	{
		res = theProc->link_triggers_connection();

		// Set the connections reference
		theProc->set_connection_context(allConnections);
		res = theProc->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	
	if (res != JVX_NO_ERROR)
	{
		// Remove all bridges
		elmB = lst_bridges.begin();
		for (; elmB != lst_bridges.end(); elmB++)
		{
			theProc->remove_bridge(elmB->uIdB);
		}
		theProc->deassociate_master();
	}	

exit_fail_II:
	theMasterFactory->return_reference_connector_master(theMaster);
exit_fail_I:
	allConnections->return_reference_connection_master_factory(theMasterFactory);

	return res;
}

jvxErrorType
CjvxConnectionDropzone::connect_group_from_dropzone(IjvxDataConnections* allConnections,
	IjvxDataConnectionGroup* theGroup JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool errInBridge = false;
	std::list<lst_elm_bidges>::iterator elmB = lst_bridges.begin();
	std::list<lst_elm_master>::iterator elmMas = lst_masters.begin();

	for (; elmB != lst_bridges.end(); elmB++)
	{
		IjvxConnectorFactory* theConFacO = NULL;
		IjvxConnectorFactory* theConFacI = NULL;
		IjvxInputConnectorSelect* icon = NULL;
		IjvxOutputConnectorSelect* ocon = NULL;
		jvxSize uIdB = JVX_SIZE_UNSELECTED;

		if (JVX_CHECK_SIZE_SELECTED(elmB->identify_out.fac_uid))
		{
			res = allConnections->reference_connection_factory_uid(elmB->identify_out.fac_uid, &theConFacO);
			if (res != JVX_NO_ERROR)
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb,
					("Failed to get reference connector factory for output connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
				errInBridge = true;
				break;
			}

			assert(theConFacO);

			res = theConFacO->reference_output_connector(elmB->identify_out.ocon_id, &ocon);
			if (res != JVX_NO_ERROR)
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb,
					("Failed to get reference output connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
				res = allConnections->return_reference_connection_factory(theConFacO);
				errInBridge = true;
				break;
			}
			assert(ocon);
		}
		// ====================================================================================

		if (JVX_CHECK_SIZE_SELECTED(elmB->identify_in.fac_uid))
		{
			res = allConnections->reference_connection_factory_uid(elmB->identify_in.fac_uid, &theConFacI);
			if (res != JVX_NO_ERROR)
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb,
					("Failed to get reference connector factory for input connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
				res = theConFacO->return_reference_output_connector(ocon);
				res = allConnections->return_reference_connection_factory(theConFacO);
				errInBridge = true;
				break;
			}

			assert(theConFacI);

			res = theConFacI->reference_input_connector(elmB->identify_in.icon_id, &icon);
			if (res != JVX_NO_ERROR)
			{
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb,
					("Failed to get reference input connector on creation of bridge <" + elmB->bridge_name + ">.").c_str(), res);
				res = theConFacO->return_reference_output_connector(ocon);
				res = allConnections->return_reference_connection_factory(theConFacO);
				res = allConnections->return_reference_connection_factory(theConFacI);
				errInBridge = true;
				break;
			}

			assert(icon);
		}
		res = theGroup->create_bridge(ocon, icon, elmB->bridge_name.c_str(), &uIdB, elmB->dedicatedThread, elmB->boostThread);

		if (ocon)
		{
			resL = theConFacO->return_reference_output_connector(ocon);
			assert(resL == JVX_NO_ERROR);
		}
		if (theConFacO)
		{
			resL = allConnections->return_reference_connection_factory(theConFacO);
			assert(resL == JVX_NO_ERROR);
		}
		if (icon)
		{
			resL = theConFacI->return_reference_input_connector(icon);
			assert(resL == JVX_NO_ERROR);
		}
		if (theConFacI)
		{
			resL = allConnections->return_reference_connection_factory(theConFacI);
			assert(resL == JVX_NO_ERROR);
		}
		if (res != JVX_NO_ERROR)
		{
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, ("Failed to create bridge <" + elmB->bridge_name + ">.").c_str(), res);
			errInBridge = true;
			break;
		}
	}

	if (!errInBridge)
	{
		// Set the connections reference
		theGroup->set_connection_context(allConnections);

		// This is only a pre-connect here
		res = theGroup->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			// Remove all bridges
			elmB = lst_bridges.begin();
			for (; elmB != lst_bridges.end(); elmB++)
			{
				theGroup->remove_bridge(elmB->uIdB);
			}
		}
	}
	else
	{
		// Remove all bridges
		elmB = lst_bridges.begin();
		for (; elmB != lst_bridges.end(); elmB++)
		{
			theGroup->remove_bridge(elmB->uIdB);
		}
	}

	return res;
}
void 
CjvxConnectionDropzone::jvx_cleanup_candidates_dropzone(IjvxDataConnections* allConnections)
{
	jvxBool someWorkDone = false;
	do
	{
		someWorkDone = false;
		std::list<lst_elm_icons>::iterator elmIc = lst_inputs.begin();
		for (; elmIc != lst_inputs.end(); elmIc++)
		{
			IjvxConnectorFactory* theConFac = NULL;
			IjvxInputConnectorSelect* icons = NULL;			
			allConnections->reference_connection_factory_uid(elmIc->identify.fac_uid, &theConFac);
			if (theConFac == NULL)
			{
				someWorkDone = true;
				break;
			}
			theConFac->reference_input_connector(elmIc->identify.icon_id, &icons);
			if (icons == NULL)
			{
				someWorkDone = true;
				allConnections->return_reference_connection_factory(theConFac);
				break;
			}
			IjvxInputConnector* iconc = icons->reference_icon();
			if (iconc && (iconc->available_to_connect_icon() == JVX_NO_ERROR))
			{
				someWorkDone = true;
				theConFac->return_reference_input_connector(icons);
				allConnections->return_reference_connection_factory(theConFac);
				break;
			}
			theConFac->return_reference_input_connector(icons);
			allConnections->return_reference_connection_factory(theConFac);
		}
		if (someWorkDone)
		{
			lst_inputs.erase(elmIc);
		}
	} while (someWorkDone);

	someWorkDone = false;
	do
	{
		someWorkDone = false;
		std::list<lst_elm_ocons>::iterator elmOc = lst_outputs.begin();
		for (; elmOc != lst_outputs.end(); elmOc++)
		{
			IjvxConnectorFactory* theConFac = NULL;
			IjvxOutputConnectorSelect* ocons = NULL;
			// IjvxDataConnectionCommon* com = NULL;
			allConnections->reference_connection_factory_uid(elmOc->identify.fac_uid, &theConFac);
			if (theConFac == NULL)
			{
				someWorkDone = true;
				break;
			}
			theConFac->reference_output_connector(elmOc->identify.ocon_id, &ocons);
			if (ocons == NULL)
			{
				someWorkDone = true;
				allConnections->return_reference_connection_factory(theConFac);
				break;
			}
			IjvxOutputConnector* oconc = ocons->reference_ocon();
			if (oconc && (oconc->available_to_connect_ocon() == JVX_NO_ERROR))
			{
				someWorkDone = true;
				theConFac->return_reference_output_connector(ocons);
				allConnections->return_reference_connection_factory(theConFac);
				break;
			}
			theConFac->return_reference_output_connector(ocons);
			allConnections->return_reference_connection_factory(theConFac);
		}
		if (someWorkDone)
		{
			lst_outputs.erase(elmOc);
		}
	} while (someWorkDone);

	someWorkDone = false;
	do
	{
		someWorkDone = false;
		std::list<lst_elm_master>::iterator elmMa = lst_masters.begin();
		for (; elmMa != lst_masters.end(); elmMa++)
		{
			IjvxConnectionMasterFactory* theMasFac = NULL;
			IjvxConnectionMaster* mas = NULL;
			jvxBool isAvail = false;
			allConnections->reference_connection_master_factory_uid(elmMa->identify.mas_fac_uid, &theMasFac);
			if (theMasFac == NULL)
			{
				someWorkDone = true;
				break;
			}
			theMasFac->reference_connector_master(elmMa->identify.mas_id, &mas);
			if (mas == NULL)
			{
				someWorkDone = true;
				allConnections->return_reference_connection_master_factory(theMasFac);
				break;
			}
			mas->available_master(&isAvail);
			if (!isAvail)
			{
				someWorkDone = true;
				theMasFac->return_reference_connector_master(mas);
				allConnections->return_reference_connection_master_factory(theMasFac);
				break;
			}
			theMasFac->return_reference_connector_master(mas);
			allConnections->return_reference_connection_master_factory(theMasFac);
		}
		if (someWorkDone)
		{
			lst_masters.erase(elmMa);
		}
	} while (someWorkDone);
}

void 
CjvxConnectionDropzone::jvx_reset_dropzone()
{
	id_select_bridge = JVX_SIZE_UNSELECTED;
	id_select_icon = JVX_SIZE_UNSELECTED;
	id_select_master = JVX_SIZE_UNSELECTED;
	id_select_ocon = JVX_SIZE_UNSELECTED;
	lst_bridges.clear();;
	lst_inputs.clear();
	lst_inputs.clear();
	lst_masters.clear();
	lst_outputs.clear();
	process_name = "VARIABLE_NOT_SET";
	process_description.clear();
}

void 
CjvxConnectionDropzone::jvx_set_process_name(const char* nm)
{
	process_name = nm;
}

void
CjvxConnectionDropzone::jvx_set_process_description(const char* descr)
{
	process_description = descr;
}

void 
CjvxConnectionDropzone::jvx_add_master(jvxSize mas_fac_uid, jvxSize mas_id, jvxBool reset_id)
{
	lst_elm_master newMaElm;
	newMaElm.identify.mas_fac_uid = mas_fac_uid;
	newMaElm.identify.mas_id = mas_id;
	lst_masters.push_back(newMaElm);
	if (reset_id)
	{
		id_select_master = 0;
	}
}

void
CjvxConnectionDropzone::jvx_add_bridge(const char* bridge_name, jvxSize out_fac_uid, jvxSize out_ocon_id, 
	jvxSize in_fac_uid, jvxSize in_icon_id, jvxBool dedicatedThread, jvxBool boostThread, 
	jvxSize oconTriggerId, jvxSize iconTriggerId)
{
	lst_elm_bidges elm;
	elm.bridge_name = bridge_name;
	elm.identify_in.fac_uid = in_fac_uid;
	elm.identify_in.icon_id = in_icon_id;
	elm.identify_in.iconTrigerId = iconTriggerId;

	elm.identify_out.fac_uid = out_fac_uid;
	elm.identify_out.ocon_id = out_ocon_id;
	elm.identify_out.oconTriggerId = oconTriggerId;

	elm.dedicatedThread = dedicatedThread;
	elm.boostThread = boostThread;
	elm.uIdB = JVX_SIZE_UNSELECTED;
	lst_bridges.push_back(elm);
}

