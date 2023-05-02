#include "jvx-helpers.h"

CjvxDescriptBridge::CjvxDescriptBridge()
{
	this->boost_thread = false;
	this->ded_thread = false;
	this->from.uid = 0;
	this->to.uid = 0;
}

CjvxConnectionDescription::CjvxConnectionDescription()
{
}

jvxErrorType 
CjvxConnectionDescription::create_process_group_from_description(
	jvxBool isProcess, 
	IjvxDataConnections* allConnections, 
	jvxSize ruleId,
	jvxBool preventStoreConfig,
	jvxBool interceptors, 
	jvxBool essential_for_start, 
	jvxSize* uidCreated, 
	jvxBool verbose_mode,
	jvxBool report_global,
	jvxSize procCatId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// =================================================================
	// From what was found in config file, try to select the master
	// =================================================================
	jvxSize numMaFac = 0;
	jvxSize i;
	IjvxConnectionMasterFactory* theMasFac = NULL;
	IjvxConnectionMaster* mas = NULL;
	IjvxDataConnectionProcess* refP = NULL;

	std::string txt;

	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxApiString ttmp;
	jvxComponentIdentification tpIdMaFac;
	std::string errtxt;
	CjvxConnectionDropzone theDropzone;
	theDropzone.jvx_reset_dropzone();
	theDropzone.jvx_set_process_name(process_name.c_str());
	if (!process_description.empty())
	{
		theDropzone.jvx_set_process_description(process_description.c_str());
	}

	//lst_elm_master newMaElm;
	jvxSize mas_fac_uid = JVX_SIZE_UNSELECTED;
	jvxSize mas_id = JVX_SIZE_UNSELECTED;
	jvxBool foundbutbusy = false;

	if (isProcess)
	{
		allConnections->number_connection_master_factories(&numMaFac);
		for (i = 0; i < numMaFac; i++)
		{
			allConnections->reference_connection_master_factory(i, &theMasFac, &uId);
			if (theMasFac)
			{
				ttmp.clear();
				jvx_request_interfaceToObject(theMasFac, NULL, &tpIdMaFac, &ttmp, NULL);
				if (
					(ttmp.std_str() == master_factory_name) &&
					(tpIdMaFac == master_factory_tp))
				{
					mas_fac_uid = uId;
					break;
				}
			}
			allConnections->return_reference_connection_master_factory(theMasFac);
			theMasFac = NULL;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(mas_fac_uid))
		{
			errtxt = "Master factory for component <" + master_factory_name + ">, " +
				jvxComponentIdentification_txt(master_factory_tp) + ", could not be found.";
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);

			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		assert(theMasFac);

		jvxSize numMa = 0;
		foundbutbusy = false;
		theMasFac->number_connector_masters(&numMa);
		for (i = 0; i < numMa; i++)
		{

			theMasFac->reference_connector_master(i, &mas);
			ttmp.clear();
			mas->descriptor_master(&ttmp);
			if (ttmp.std_str() == master_name)
			{
				mas->associated_process(&refP);
				if (refP == NULL)
				{
					mas_id = i;
					break;
				}
				else
				{
					foundbutbusy = true;
				}
			}
			theMasFac->return_reference_connector_master(mas);
			mas = NULL;
		}

		if (mas)
		{
			theMasFac->return_reference_connector_master(mas);
		}
		if (theMasFac)
		{
			allConnections->return_reference_connection_master_factory(theMasFac);
		}
		mas = NULL;
		theMasFac = NULL;
		if (JVX_CHECK_SIZE_UNSELECTED(mas_id))
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			if (!foundbutbusy)
			{
				errtxt = "Master <" + master_name + "> related to master factory <" + master_factory_name + "> " +
					jvxComponentIdentification_txt(master_factory_tp) + ", could not be found.";
			}
			else
			{
				res = JVX_ERROR_ALREADY_IN_USE;
				errtxt = "Master <" + master_name + "> related to master factory <" + master_factory_name + "> " +
					jvxComponentIdentification_txt(master_factory_tp) + ", was found but is already in use.";
			}
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), res);

			return res;
		}

		theDropzone.jvx_add_master(mas_fac_uid, mas_id, true);
	}

	// Master clear

	std::list<CjvxDescriptBridge>::iterator elm = connectors.begin();
	for (; elm != connectors.end(); elm++)
	{
		//==========================================================
		//lst_elm_bidges newBridge;
		//newBridge.identify_in.fac_uid = JVX_SIZE_UNSELECTED;
		//newBridge.identify_in.icon_id = JVX_SIZE_UNSELECTED;
		//newBridge.identify_out.fac_uid = JVX_SIZE_UNSELECTED;
		//newBridge.identify_out.ocon_id = JVX_SIZE_UNSELECTED;
		jvxSize in_fac_uid = JVX_SIZE_UNSELECTED;
		jvxSize in_icon_id = JVX_SIZE_UNSELECTED;
		jvxSize out_fac_uid = JVX_SIZE_UNSELECTED;
		jvxSize out_ocon_id = JVX_SIZE_UNSELECTED;

		jvxSize numConFac = 0;
		IjvxConnectorFactory* theConFac = NULL;
		IjvxOutputConnectorSelect* ocons = NULL;
		IjvxInputConnectorSelect* icons = NULL;
		jvxSize numOc = 0;
		jvxSize numIc = 0;

		// ======================================================================
		if (!(JVX_CHECK_SIZE_UNSELECTED(elm->from.uid)))
		{
			allConnections->number_connection_factories(&numConFac);
			for (i = 0; i < numConFac; i++)
			{
				allConnections->reference_connection_factory(i, &theConFac, &uId);
				ttmp.clear();
				jvx_request_interfaceToObject(theConFac, NULL, &tpIdMaFac, &ttmp, NULL);
				if (
					(ttmp.std_str() == elm->from.connector_factory_name) &&
					(tpIdMaFac == elm->from.tp))
				{
					out_fac_uid = uId;
					break;
				}
				allConnections->return_reference_connection_factory(theConFac);
				theConFac = NULL;
			}

			if (JVX_CHECK_SIZE_UNSELECTED(out_fac_uid))
			{
				errtxt = "Bridge <" + elm->bridge_name + ">, reference from, connection factory <" + elm->from.connector_factory_name + "> " +
					jvxComponentIdentification_txt(elm->from.tp) + ", could not be found.";
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);

				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}

			foundbutbusy = false;
			theConFac->number_output_connectors(&numOc);
			for (i = 0; i < numOc; i++)
			{
				theConFac->reference_output_connector(i, &ocons);
				ocons->descriptor_connector(&ttmp);
				if (ttmp.std_str() == elm->from.connector_name)
				{
					IjvxDataConnectionCommon* refC = NULL;	
					IjvxOutputConnector* ocon = ocons->reference_ocon();
					if (ocon)
					{
						ocon->associated_common_ocon(&refC);
					}
					if (refC == NULL)
					{
						out_ocon_id = i;
						break;
					}
					else
					{
						foundbutbusy = true;
					}
				}
				theConFac->return_reference_output_connector(ocons);
				ocons = NULL;
			}

			if (ocons)
			{
				theConFac->return_reference_output_connector(ocons);
			}
			if (theConFac)
			{
				allConnections->return_reference_connection_factory(theConFac);
			}
			ocons = NULL;
			theConFac = NULL;

			if (JVX_CHECK_SIZE_UNSELECTED(out_ocon_id))
			{
				if (!foundbutbusy)
				{
					errtxt = "Ref from connector <" + elm->from.connector_name + "> related to connection factory <" + elm->from.connector_factory_name + "> " +
						jvxComponentIdentification_txt(elm->from.tp) + ", could not be found.";
				}
				else
				{
					errtxt = "Ref from connector <" + elm->from.connector_name + "> related to connection factory <" + elm->from.connector_factory_name + "> " +
						jvxComponentIdentification_txt(elm->from.tp) + ", could be found but is already in use";
				}
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);

				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
		else
		{
			if (isProcess)
			{
				errtxt = "Ref from connector is specified as an open input connector, this is only valid when creating a group.";
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		//==========================================================

		numConFac = 0;
		theConFac = NULL;
		ocons = NULL;
		icons = NULL;
		numOc = 0;
		numIc = 0;
		if (!JVX_CHECK_SIZE_UNSELECTED(elm->to.uid))
		{
			allConnections->number_connection_factories(&numConFac);
			for (i = 0; i < numConFac; i++)
			{
				allConnections->reference_connection_factory(i, &theConFac, &uId);
				ttmp.clear();
				jvx_request_interfaceToObject(theConFac, NULL, &tpIdMaFac, &ttmp, NULL);
				if (
					(ttmp.std_str() == elm->to.connector_factory_name) &&
					(tpIdMaFac == elm->to.tp))
				{
					in_fac_uid = uId;
					break;
				}
				allConnections->return_reference_connection_factory(theConFac);
				theConFac = NULL;
			}

			if (JVX_CHECK_SIZE_UNSELECTED(in_fac_uid))
			{
				errtxt = "Bridge <" + elm->bridge_name + ">, reference from, connection factory <" + elm->to.connector_factory_name + "> " +
					jvxComponentIdentification_txt(elm->to.tp) + ", could not be found.";
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);
				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}

			foundbutbusy = false;
			theConFac->number_input_connectors(&numIc);
			for (i = 0; i < numIc; i++)
			{
				theConFac->reference_input_connector(i, &icons);
				icons->descriptor_connector(&ttmp);
				if (ttmp.std_str() == elm->to.connector_name)
				{
					IjvxDataConnectionCommon* refC = NULL;
					IjvxInputConnector* icon = icons->reference_icon();
					if (icon)
					{
						icon->associated_common_icon(&refC);
					}
					if (refC == NULL)
					{
						in_icon_id = i;
						break;
					}
					else
					{
						foundbutbusy = true;
					}
				}
				theConFac->return_reference_input_connector(icons);
				icons = NULL;
			}

			if (icons)
			{
				theConFac->return_reference_input_connector(icons);
			}
			if (theConFac)
			{
				allConnections->return_reference_connection_factory(theConFac);
			}
			icons = NULL;
			theConFac = NULL;

			if (JVX_CHECK_SIZE_UNSELECTED(in_icon_id))
			{
				if (!foundbutbusy)
				{
					errtxt = "Ref to connector <" + elm->from.connector_name + "> related to connection factory <" + elm->to.connector_factory_name + "> " +
						jvxComponentIdentification_txt(elm->to.tp) + ", could not be found.";
				}
				else
				{
					errtxt = "Ref to connector <" + elm->from.connector_name + "> related to connection factory <" + elm->to.connector_factory_name + "> " +
						jvxComponentIdentification_txt(elm->to.tp) + ", could be found but is already in use";
				}
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ALREADY_IN_USE);
				return JVX_ERROR_ELEMENT_NOT_FOUND;
			}
		}
		else
		{
			if (isProcess)
			{
				errtxt = "Ref from connector is specified as an open input connector, this is only valid when creating a group.";
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), JVX_ERROR_ELEMENT_NOT_FOUND);
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		// Seems to be ok, adding bridge to list
		theDropzone.jvx_add_bridge(elm->bridge_name.c_str(), out_fac_uid, out_ocon_id, in_fac_uid, in_icon_id, 
			elm->ded_thread, elm->boost_thread, elm->from.oconTriggerId, elm->to.iconTriggerId);
	}
	if (isProcess)
	{
		jvxSize cnt = 0;
		IjvxDataConnectionProcess* theProcess = NULL;
		jvxErrorType resL = JVX_NO_ERROR;
		while (1)
		{
			std::string name_with_id = process_name;
			if (cnt != 0)
			{
				name_with_id += "<";
				name_with_id += jvx_size2String(cnt);
				name_with_id += ">";
			}

			// Here, we have all information prepared, go and connect the description
			jvxErrorType resL = allConnections->create_connection_process(name_with_id.c_str(), 
				&uId, interceptors, essential_for_start, verbose_mode, report_global);
			if (resL == JVX_ERROR_ALREADY_IN_USE)
			{
				cnt++;
			}
			else
			{
				if (resL != JVX_NO_ERROR)
				{
					errtxt = "Failed to create new connection with name <" +
						process_name + ", error reason: " + jvxErrorType_txt(resL);
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
					return resL;
				}
				break;
			}
		}

		resL = allConnections->reference_connection_process_uid(uId, &theProcess);
		if (resL == JVX_NO_ERROR)
		{
			// If the ruleId is set or the preventStoreConfig is set there will be no entry in the config file.
			// Connection Processes that are created based on a rule should prevent to store the connection in the config file!
			theProcess->set_misc_connection_parameters(ruleId, preventStoreConfig);

			if (JVX_CHECK_SIZE_SELECTED(procCatId))
			{
				// Add the category id before connecting to be available in sync ui updates
				theProcess->set_category_id(procCatId);
			}
		}
		else
		{
			errtxt = "Failed to get reference to new connection with name <" +
				process_name + ", error reason: " + jvxErrorType_txt(resL);
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
			return resL;
		}

		resL = theDropzone.connect_process_from_dropzone(allConnections, theProcess JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (resL != JVX_NO_ERROR)
		{
			jvxErrorType resErr = JVX_NO_ERROR;
			errtxt = "Failed to connect new connection with name <" +
				process_name + ", error reason: " + jvxErrorType_txt(resL);

			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
			resErr = allConnections->return_reference_connection_process(theProcess);
			theProcess = NULL;

			resErr = allConnections->remove_connection_process(uId);

			theDropzone.jvx_reset_dropzone();

			return resL;
		}

		// Run the test function. It sets the error status and description internally, therefore, error code is ignored
		resL = theProcess->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A_NULL);

		if (uidCreated)
			*uidCreated = uId;

		resL = allConnections->return_reference_connection_process(theProcess);
		theProcess = NULL;
	}
	else
	{
		IjvxDataConnectionGroup* theGroup = NULL;

		// Here, we have all information prepared, go and connect the description
		jvxErrorType resL = allConnections->create_connection_group(process_name.c_str(), &uId, process_description.c_str(), verbose_mode);
		if (resL != JVX_NO_ERROR)
		{
			errtxt = "Failed to create new connection group with name <" +
				process_name + ", error reason: " + jvxErrorType_txt(resL);
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
			return resL;
		}

		resL = allConnections->reference_connection_group_uid(uId, &theGroup);
		if (resL == JVX_NO_ERROR)
		{
			theGroup->set_misc_connection_parameters(ruleId, false);
		}
		else
		{
			errtxt = "Failed to get reference to new connection group with name <" +
				process_name + ", error reason: " + jvxErrorType_txt(resL);
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
			return resL;
		}

		resL = theDropzone.connect_group_from_dropzone(allConnections, theGroup JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (resL != JVX_NO_ERROR)
		{
			errtxt = "Failed to connect new connection group with name <" +
				process_name + ", error reason: " + jvxErrorType_txt(resL);

			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, errtxt.c_str(), resL);
			resL = allConnections->return_reference_connection_group(theGroup);
			theGroup = NULL;

			theDropzone.jvx_reset_dropzone();
			return resL;
		}

		if (uidCreated)
			*uidCreated = uId;

		resL = allConnections->return_reference_connection_group(theGroup);
		theGroup = NULL;
	}
	return JVX_NO_ERROR;
}