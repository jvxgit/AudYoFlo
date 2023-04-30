#include "HjvxMicroConnection.h"

HjvxMicroConnection::HjvxMicroConnection(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	theConnections = NULL;
	/*
	theConFac = NULL;
	theTarget = NULL;
	iconr = NULL;
	oconr = NULL;
	*/
	master = NULL;
	mocon = NULL;
	micon = NULL;
	refHandleSimple = nullptr;
	refHandleFwd = nullptr;
	useBuffersInPlaceInput = true;
	useBuffersInPlaceOutput = true;
	theData_inlnk = NULL;
	theData_outlnk = NULL;
	uId_process = JVX_SIZE_UNSELECTED;
	uId_bridge_dev_node = JVX_SIZE_UNSELECTED;
	uId_bridge_node_dev = JVX_SIZE_UNSELECTED;

	_common_set.thisisme = static_cast<IjvxObject*>(this);

}
HjvxMicroConnection::~HjvxMicroConnection()
{
	theConnections = NULL;
}

jvxErrorType 
HjvxMicroConnection::activate_connection(
	IjvxHiddenInterface* href,
	IjvxHiddenInterface* target,
	const std::string& iconn,
	const std::string& oconn,
	const std::string& mastern,
	const std::string& mioconn,
	const std::string& nmprocess,
	jvxBool interceptors,
	IjvxObject* theOwner, 
	jvxSize idProcDepends
	)
{
	jvxSize num, i;
	jvxApiString str;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool preventStore = true;

	if (!href)
		return JVX_ERROR_INVALID_ARGUMENT;
	if (!target)
		return JVX_ERROR_INVALID_ARGUMENT;
	
	if (theConnections == NULL)
	{
		dbg_hint = "MicroConnection-" + nmprocess;
		oneInvolvedObject newElm;
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), mioconn,
			static_cast<IjvxConnectionMaster*>(this), dbg_hint.c_str());
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), mastern);

		CjvxObject::_initialize(href);
		CjvxObject::_select();
		CjvxObject::_activate();

		_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
		newElm.theTarget = target;

		newElm.theTarget->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&newElm.theConFac));

		newElm.theConFac->number_input_connectors(&num);
		for (i = 0; i < num; i++)
		{
			str.clear();
			newElm.theConFac->reference_input_connector(i, &newElm.iconr);
			newElm.iconr->descriptor_connector(&str);
			if (str.std_str() == iconn)
			{
				break;
			}
			newElm.theConFac->return_reference_input_connector(newElm.iconr);
			newElm.iconr = NULL;
		}

		newElm.theConFac->number_output_connectors(&num);
		for (i = 0; i < num; i++)
		{
			str.clear();
			newElm.theConFac->reference_output_connector(i, &newElm.oconr);
			newElm.oconr->descriptor_connector(&str);
			if (str.std_str() == oconn)
			{
				break;
			}
			newElm.theConFac->return_reference_output_connector(newElm.oconr);
			newElm.oconr = NULL;
		}

		if (newElm.iconr && newElm.oconr)
		{
			this->number_connector_masters(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_connector_master(i, &master);
				master->descriptor_master(&str);
				if (str.std_str() == mastern)
				{
					break;
				}
				this->return_reference_connector_master(master);
				master = NULL;
			}

			this->number_output_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_output_connector(i, &mocon);
				mocon->descriptor_connector(&str);
				if (str.std_str() == mioconn)
				{
					break;
				}
				this->return_reference_output_connector(mocon);
				mocon = NULL;
			}

			this->number_input_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_input_connector(i, &micon);
				micon->descriptor_connector(&str);
				if (str.std_str() == mioconn)
				{
					break;
				}
				this->return_reference_input_connector(micon);
				micon = NULL;
			}
			
			if (master && mocon && micon)
			{
				IjvxDataConnectionProcess* theProc = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
				res = theConnections->create_connection_process(nmprocess.c_str(), &uId_process, interceptors, false, false, false, idProcDepends);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theConnections->reference_connection_process_uid(uId_process, &theProc);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}
				
				res = theProc->set_connection_context(theConnections);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				// Do not create an entry in the config file by actively preventing it!
				res = theProc->set_misc_connection_parameters(JVX_SIZE_UNSELECTED, preventStore);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				// We need to specify this object as the owener, otherwise, the connection will
				// be removed before the "deactivate" member function is called
				res = theProc->associate_master(master, theOwner);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}
				
				res = theProc->create_bridge(mocon, newElm.iconr, "Device to node bridge", &uId_bridge_dev_node);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}
				
				res = theProc->create_bridge(newElm.oconr, micon, "Node to device bridge", &uId_bridge_node_dev);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}
				
				theConnections->return_reference_connection_process(theProc);

				involvedObjects.push_back(newElm);

			}
		}
		else
		{
			goto exit_error;

			return JVX_ERROR_INVALID_SETTING;
		}

		return JVX_NO_ERROR;
	}
	
	return JVX_ERROR_WRONG_STATE;

exit_error:
	deactivate_connection();

	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
HjvxMicroConnection::activate_connection(
	IjvxHiddenInterface* href,
	IjvxHiddenInterface** target,
	const char** iconn,
	const char** oconn,
	jvxSize num_objs,
	const std::string& mastern,
	const std::string& mioconn,
	const std::string& nmprocess,
	jvxBool interceptors,
	IjvxObject* theOwner,
	jvxSize idProcDepends, 
	jvxBool fwdTestToDependent)
{
	jvxSize num, i, k;
	jvxApiString str;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool preventStore = true;
	last_error = "";

	if (!href)
	{
		last_error = __FUNCTION__;
		last_error += ": Call fail due to a nullptr host interface reference.";
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	if (!target)
		return JVX_ERROR_INVALID_ARGUMENT;


	if (theConnections == NULL)
	{
		dbg_hint = "MicroConnection-" + nmprocess;

		oneInvolvedObject newElm;
		jvxBool all_connectors_ok = true;

		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), mioconn,
			static_cast<IjvxConnectionMaster*>(this), dbg_hint.c_str());
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), mioconn);

		CjvxObject::_initialize(href);
		CjvxObject::_select();
		CjvxObject::_activate();

		_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
		
		for (k = 0; k < num_objs; k++)
		{
			newElm.theTarget = target[k];
			newElm.theTarget->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&newElm.theConFac));

			newElm.theConFac->number_input_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				newElm.theConFac->reference_input_connector(i, &newElm.iconr);
				newElm.iconr->descriptor_connector(&str);
				if (str.std_str() == iconn[k])
				{
					break;
				}
				newElm.theConFac->return_reference_input_connector(newElm.iconr);
				newElm.iconr = NULL;
			}

			newElm.theConFac->number_output_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				newElm.theConFac->reference_output_connector(i, &newElm.oconr);
				newElm.oconr->descriptor_connector(&str);
				if (str.std_str() == oconn[k])
				{
					break;
				}
				newElm.theConFac->return_reference_output_connector(newElm.oconr);
				newElm.oconr = NULL;
			}
			if (newElm.iconr && newElm.oconr)
			{
				involvedObjects.push_back(newElm);
			}
			else
			{
				all_connectors_ok = false;
				break;
			}
		}

		if (all_connectors_ok && involvedObjects.size())
		{
			this->number_connector_masters(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_connector_master(i, &master);
				master->descriptor_master(&str);
				if (str.std_str() == mastern)
				{
					break;
				}
				this->return_reference_connector_master(master);
				master = NULL;
			}

			this->number_output_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_output_connector(i, &mocon);
				mocon->descriptor_connector(&str);
				if (str.std_str() == mioconn)
				{
					break;
				}
				this->return_reference_output_connector(mocon);
				mocon = NULL;
			}

			this->number_input_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_input_connector(i, &micon);
				micon->descriptor_connector(&str);
				if (str.std_str() == mioconn)
				{
					break;
				}
				this->return_reference_input_connector(micon);
				micon = NULL;
			}

			if (master && mocon && micon)
			{
				IjvxDataConnectionProcess* theProc = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
				res = theConnections->create_connection_process(nmprocess.c_str(), &uId_process, interceptors, false, false, false, idProcDepends);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theConnections->reference_connection_process_uid(uId_process, &theProc);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}
				
				res = theProc->set_connection_context(theConnections);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				// Set the fwd on test function in case desired
				theProc->set_forward_test_depend(fwdTestToDependent);
				
				// Do not create an entry in the config file by actively preventing it!
				res = theProc->set_misc_connection_parameters(JVX_SIZE_UNSELECTED, preventStore);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theProc->associate_master(master, static_cast<IjvxObject*>(this));
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				jvxSize cnt = 0;
				res = theProc->create_bridge(mocon, involvedObjects[cnt].iconr, "Device to node bridge", &uId_bridge_dev_node);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				for (i = 0; i < involvedObjects.size() - 1; i++)
				{
					res = theProc->create_bridge(involvedObjects[cnt].oconr, involvedObjects[cnt+1].iconr,
						("Node #" + jvx_size2String(cnt) + " to node#" + jvx_size2String(cnt) + " bridge").c_str(),
						&involvedObjects[cnt].uId_bridge_node_node);
					if (res != JVX_NO_ERROR)
					{
						goto exit_error;
					}
					cnt++;
				}
				res = theProc->create_bridge(involvedObjects[cnt].oconr, micon, "Node to device bridge", &uId_bridge_node_dev);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				theConnections->return_reference_connection_process(theProc);

				

			}
		}
		else
		{
			goto exit_error;
		}

		return JVX_NO_ERROR;
	}

	return JVX_ERROR_WRONG_STATE;

exit_error:

	deactivate_connection();
	return res;
}
/*
jvxErrorType
HjvxMicroConnection::activate_connection(
	IjvxHiddenInterface* href,
	IjvxHiddenInterface** target,
	std::string* connectors,
	jvxSize num_objs,
	const std::string& mastern,
	const std::string& moconn,
	const std::string& miconn,
	const std::string& nmprocess,
	jvxBool interceptors
)
{
	jvxSize num, i;
	jvxApiString str;
	jvxErrorType res = JVX_NO_ERROR;

	if (!href)
		return JVX_ERROR_INVALID_ARGUMENT;
	if (!target)
		return JVX_ERROR_INVALID_ARGUMENT;

	if (theConnections == NULL)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", static_cast<IjvxConnectionMaster*>(this));
		JVX_ACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER(static_cast<CjvxObject*>(this), "default");

		CjvxObject::_initialize(href);
		CjvxObject::_select();
		CjvxObject::_activate();

		_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
		theTarget = target;

		theTarget->request_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle**>(&theConFac));

		theConFac->number_input_connectors(&num);
		for (i = 0; i < num; i++)
		{
			str.clear();
			theConFac->reference_input_connector(i, &iconr);
			iconr->descriptor_connector(&str);
			if (str.std_str() == iconn)
			{
				break;
			}
			theConFac->return_reference_input_connector(iconr);
			iconr = NULL;
		}

		theConFac->number_output_connectors(&num);
		for (i = 0; i < num; i++)
		{
			str.clear();
			theConFac->reference_output_connector(i, &oconr);
			oconr->descriptor_connector(&str);
			if (str.std_str() == oconn)
			{
				break;
			}
			theConFac->return_reference_output_connector(oconr);
			oconr = NULL;
		}

		if (iconr && oconr)
		{
			this->number_connector_masters(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_connector_master(i, &master);
				master->descriptor_master(&str);
				if (str.std_str() == mastern)
				{
					break;
				}
				this->return_reference_connector_master(master);
				master = NULL;
			}

			this->number_output_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_output_connector(i, &mocon);
				mocon->descriptor_connector(&str);
				if (str.std_str() == moconn)
				{
					break;
				}
				this->return_reference_output_connector(mocon);
				mocon = NULL;
			}

			this->number_input_connectors(&num);
			for (i = 0; i < num; i++)
			{
				str.clear();
				this->reference_input_connector(i, &micon);
				micon->descriptor_connector(&str);
				if (str.std_str() == miconn)
				{
					break;
				}
				this->return_reference_input_connector(micon);
				micon = NULL;
			}

			if (master && mocon && micon)
			{
				IjvxDataConnectionProcess* theProc = NULL;
				JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
				res = theConnections->create_connection_process(nmprocess.c_str(), &uId_process, interceptors);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theConnections->reference_connection_process_uid(uId_process, &theProc);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theProc->set_context(theConnections);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theProc->associate_master(master);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theProc->create_bridge(mocon, iconr, "Device to node bridge", &uId_bridge_dev_node);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				res = theProc->create_bridge(oconr, micon, "Node to device bridge", &uId_bridge_node_dev);
				if (res != JVX_NO_ERROR)
				{
					goto exit_error;
				}

				theConnections->return_reference_connection_process(theProc);
			}
		}
		else
		{
			goto exit_error;

			return JVX_ERROR_INVALID_SETTING;
		}

		return JVX_NO_ERROR;
	}

	return JVX_ERROR_WRONG_STATE;

exit_error:
	deactivate_connection();

	return JVX_ERROR_INVALID_SETTING;
}
*/

jvxErrorType
HjvxMicroConnection::connect_connection(
	jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxConnectionType connTypeArg,
	HjvxMicroConnection_hooks_simple* refSimple,
	HjvxMicroConnection_hooks_fwd* refExt,	
	jvxLinkDataDescriptor* theData_propArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;
	if (theConnections)
	{
		typeConnection = connTypeArg;
		theData_inlnk = theData_in;
		theData_outlnk = theData_out;

		theData_proposeInput = theData_propArg;

		refHandleSimple = refSimple;
		refHandleFwd = refExt;

		switch (typeConnection)
		{
		case jvxConnectionType::JVX_MICROCONNECTION_ENGAGE:
			
			// This is for a kind of validation
			assert(refHandleSimple);
			assert(refHandleFwd);
			break;
		case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_ADAPT:
		case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED:
			assert(refHandleSimple == nullptr);
			assert(refHandleFwd == nullptr);
			break;
		default: 
			break;
		}
		res = theConnections->reference_connection_process_uid(uId_process, &theProc);
		assert(res == JVX_NO_ERROR);

		_common_set_ocon.theData_out.con_params = theData_inlnk->con_params;
		theProc->set_test_on_connect(false, &_common_set_ocon.theData_out.con_params);

		res = theProc->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			goto exit_error;
		}

		theConnections->return_reference_connection_process(theProc);
		theProc = NULL;
		
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;

exit_error:

	theConnections->return_reference_connection_process(theProc);
	theProc = NULL;

	return res;
}

jvxErrorType
HjvxMicroConnection::disconnect_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;
	if (theConnections)
	{
		// The process may already have been removed if the overall system is being shut down
		res = theConnections->reference_connection_process_uid(uId_process, &theProc);
		if (res == JVX_NO_ERROR)
		{
			res = theProc->disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res != JVX_NO_ERROR)
			{
				goto exit_error;
			}

			theConnections->return_reference_connection_process(theProc);
			theProc = NULL;
		}
		theData_inlnk = NULL;
		theData_outlnk = NULL;
		return res;
	}
	return JVX_ERROR_WRONG_STATE;

exit_error:

	theConnections->return_reference_connection_process(theProc);
	theProc = NULL;

	return res;
}

/*
jvxErrorType
HjvxMicroConnection::sync_processing_parameters(
	jvxLinkDataDescriptor_con_params* con_params)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	if(con_params == &theData_inlnk->con_params)
	{ 
		theData_inlnk->con_params = *con_params;
		res = JVX_NO_ERROR;
	}
	else
	{
		if (con_params == &theData_outlnk->con_params)
		{
			theData_outlnk->con_params = *con_params;
			res = JVX_NO_ERROR;
		}
	}
	return res;
}
*/

jvxErrorType
HjvxMicroConnection::test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnectionProcess* theProc = NULL;

	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str(),
		"Starting part of micro connection subbranch.");
	if (theConnections)
	{
		_common_set_ocon.theData_out.con_params = theData_inlnk->con_params; 	

		res = theConnections->reference_connection_process_uid(uId_process, &theProc);
		assert(res == JVX_NO_ERROR);

		// Store info related to micro connection output parameters
		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O_TAG(fdb, (&_common_set_ocon.theData_out), "micro-connection-param-out");

		res = theProc->test_chain(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			goto exit_error;
		}

		theConnections->return_reference_connection_process(theProc);
		theProc = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;

exit_error:

	theConnections->return_reference_connection_process(theProc);
	theProc = NULL;

	return res;
}

jvxErrorType 
HjvxMicroConnection::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		theData_inlnk->con_params = _common_set_ocon.theData_out.con_params;
	}
	return res;
}

jvxErrorType 
HjvxMicroConnection::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	// This is the endpint of the micro connection
	jvxBool forward = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor ld_con;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str(),
		"Stopping part of micro connection subbranch.");

	if (refHandleSimple)
	{
		res = refHandleSimple->hook_test_accept(_common_set_icon.theData_in 
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		switch (typeConnection)
		{
		case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED:
			ld_con.con_params.buffersize = theData_outlnk->con_params.buffersize;
			ld_con.con_params.rate = theData_outlnk->con_params.rate;
			ld_con.con_params.format = theData_outlnk->con_params.format;
			ld_con.con_params.number_channels = theData_outlnk->con_params.number_channels;

			jvx_bitFClear(ld_con.con_params.additional_flags);
			ld_con.con_params.segmentation.x = theData_outlnk->con_params.buffersize;
			ld_con.con_params.segmentation.y = 1;
			ld_con.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

			if (
				(_common_set_icon.theData_in->con_params.buffersize != theData_outlnk->con_params.buffersize) ||
				(_common_set_icon.theData_in->con_params.rate != theData_outlnk->con_params.rate) ||
				(_common_set_icon.theData_in->con_params.format != theData_outlnk->con_params.format) ||
				(_common_set_icon.theData_in->con_params.number_channels != theData_outlnk->con_params.number_channels) ||

				(_common_set_icon.theData_in->con_params.segmentation.x != theData_outlnk->con_params.segmentation.x) ||
				(_common_set_icon.theData_in->con_params.segmentation.y != theData_outlnk->con_params.segmentation.y) ||
				(_common_set_icon.theData_in->con_params.format_group != theData_outlnk->con_params.format_group))
			{
				res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS,
					&ld_con JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}

			switch (res)
			{
			case JVX_NO_ERROR:
				res = _test_connect_icon(false JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				break;

			case JVX_ERROR_COMPROMISE:
				if (
					(_common_set_icon.theData_in->con_params.buffersize != theData_outlnk->con_params.buffersize) ||
					(_common_set_icon.theData_in->con_params.rate != theData_outlnk->con_params.rate) ||
					(_common_set_icon.theData_in->con_params.segmentation.x != theData_outlnk->con_params.segmentation.x) ||
					(_common_set_icon.theData_in->con_params.segmentation.y != theData_outlnk->con_params.segmentation.y) ||
					(_common_set_icon.theData_in->con_params.format_group != theData_outlnk->con_params.format_group) ||
					(_common_set_icon.theData_in->con_params.format != theData_outlnk->con_params.format))
				{
					std::string err = "";
					if (_common_set_icon.theData_in->con_params.buffersize != theData_outlnk->con_params.buffersize)
					{
						err = "Buffersize: " + jvx_size2String(_common_set_icon.theData_in->con_params.buffersize) + " vs " +
							jvx_size2String(theData_outlnk->con_params.buffersize);
					}
					if (_common_set_icon.theData_in->con_params.rate != theData_outlnk->con_params.rate)
					{
						err = "Samplerate: " + jvx_size2String(_common_set_icon.theData_in->con_params.rate) + " vs " +
							jvx_size2String(theData_outlnk->con_params.rate);
					}
					if (_common_set_icon.theData_in->con_params.format != theData_outlnk->con_params.format)
					{
						err = "Format: ";
						err += jvxDataFormat_txt(_common_set_icon.theData_in->con_params.format);
						err += " vs ";
						err += jvxDataFormat_txt(theData_outlnk->con_params.format);
					}
					JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, ("Could not find compromise with previous audio node: " + err).c_str(), JVX_ERROR_INVALID_SETTING);
					res = JVX_ERROR_INVALID_SETTING;
				}
				else
				{
					// There might be a chance to create a mapping but at the moment, it is not supported
					assert(0);
				}
				break;
			default:
				JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb,
					"Unable to find a compromise, reason: ", JVX_ERROR_INVALID_SETTING);
				res = JVX_ERROR_INVALID_SETTING;
				break;
			}
			break;
		case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_ADAPT:

			// The output parameters are modified
			theData_outlnk->con_params = _common_set_icon.theData_in->con_params;
			break;
		default:
			// What are we going to do here?
			assert(0);
			break;
		}
	}
	return res;
};

/**
 * On prepare, we copy the parameters as specified in the linked data container
 */
jvxErrorType 
HjvxMicroConnection::prepare_connection(jvxBool buffersInPlaceIn, jvxBool buffersInPlaceOut, jvxBool copyAttachedDataArg)
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;
	
	res = CjvxObject::_prepare();
	if (res != JVX_NO_ERROR)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	assert(theConnections);

	useBuffersInPlaceInput = buffersInPlaceIn;
	useBuffersInPlaceOutput = buffersInPlaceOut;

	// Copy the processing parameters in passed container
	_common_set_ocon.theData_out.con_params = theData_inlnk->con_params; // Inportant: do not touch "link" subfield, otherwise, linkage will be destroyed
	_common_set_ocon.theData_out.con_data = theData_inlnk->con_data; // Copy the number of buffers and the allocation flags

	copyAttachedData = copyAttachedDataArg;

	// Forwarding the attached data does not make sense in a flexible connection but makes sense in a fixed connection
	if (copyAttachedData)
	{
		_common_set_ocon.theData_out.con_link.attached_chain_single_pass = theData_inlnk->con_link.attached_chain_single_pass;
	}

	if (useBuffersInPlaceInput)
	{
		_common_set_ocon.theData_out.con_pipeline = theData_inlnk->con_pipeline;
		_common_set_ocon.theData_out.con_data = theData_inlnk->con_data;
		jvx_bitZSet(_common_set_ocon.theData_out.con_data.alloc_flags, 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT); // Use the field which was passed in the struct
	}

	res = theConnections->reference_connection_process_uid(uId_process, &theProc);
	assert(res == JVX_NO_ERROR);

	res = theProc->prepare_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		goto exit_error;
	}

	theConnections->return_reference_connection_process(theProc);

	if (!useBuffersInPlaceInput)
	{
		theData_inlnk->con_pipeline = _common_set_ocon.theData_out.con_pipeline;
		theData_inlnk->con_data.buffers = _common_set_ocon.theData_out.con_data.buffers;
		theData_inlnk->con_data.fHeights = _common_set_ocon.theData_out.con_data.fHeights;
		if (jvx_bitTest(_common_set_ocon.theData_out.con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_THREAD_INIT_PRE_RUN))
		{
			jvx_bitSet(theData_inlnk->con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_THREAD_INIT_PRE_RUN);
		}
	}

	return res;

exit_error:

	theProc->postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvx_neutralDataLinkDescriptor(&_common_set_ocon.theData_out, true);
	if (theProc)
	{
		theConnections->return_reference_connection_process(theProc);
	}
	theProc = NULL;

	return res;
}


jvxErrorType
HjvxMicroConnection::start_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;

	res = CjvxObject::_start();
	if (res != JVX_NO_ERROR)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	assert(theConnections);

	res = theConnections->reference_connection_process_uid(uId_process, &theProc);
	assert(res == JVX_NO_ERROR);

	res = theProc->start_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		goto exit_error;
	}

	theConnections->return_reference_connection_process(theProc);
	theProc = NULL;
	return res;


exit_error:

	if (theProc)
	{
		theConnections->return_reference_connection_process(theProc);
	}
	theProc = NULL;

	return res;
}

jvxErrorType 
HjvxMicroConnection::postprocess_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;

	res = CjvxObject::_postprocess();
	if (res != JVX_NO_ERROR)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	assert(theConnections);

	res = theConnections->reference_connection_process_uid(uId_process, &theProc);
	assert(res == JVX_NO_ERROR);
	theProc->postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	theConnections->return_reference_connection_process(theProc);

	if (!useBuffersInPlaceInput)
	{
		//theData_inlnk->pipeline. = NULL;
		theData_inlnk->con_data = _common_set_ocon.theData_out.con_data;
	}

	return JVX_NO_ERROR;
}

jvxErrorType
HjvxMicroConnection::stop_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnectionProcess* theProc = NULL;

	res = CjvxObject::_stop();
	if (res != JVX_NO_ERROR)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	assert(theConnections);

	res = theConnections->reference_connection_process_uid(uId_process, &theProc);
	assert(res == JVX_NO_ERROR);

	theProc->stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));

	theConnections->return_reference_connection_process(theProc);

	return JVX_NO_ERROR;
}

jvxErrorType 
HjvxMicroConnection::deactivate_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	if (theConnections)
	{
		if (JVX_CHECK_SIZE_SELECTED(uId_process))
		{
			// Removing the connection may fail if the overall system is in shutdown
			res = theConnections->remove_connection_process(uId_process);			

			uId_process = JVX_SIZE_UNSELECTED;
			uId_bridge_dev_node = JVX_SIZE_UNSELECTED;
			uId_bridge_node_dev = JVX_SIZE_UNSELECTED;
		}
		if (master)
		{
			res = this->return_reference_connector_master(master);
		}
		master = NULL;

		if (mocon)
		{
			res = this->return_reference_output_connector(mocon);
		}
		mocon = NULL;

		if (micon)
		{
			res = this->return_reference_input_connector(micon);
		}
		micon = NULL;

		for (i = 0; i < involvedObjects.size(); i++)
		{
			if (involvedObjects[i].iconr)
			{
				res = involvedObjects[i].theConFac->return_reference_input_connector(involvedObjects[i].iconr);
			}
			involvedObjects[i].iconr = NULL;

			if (involvedObjects[i].oconr)
			{
				res = involvedObjects[i].theConFac->return_reference_output_connector(involvedObjects[i].oconr);
			}
			involvedObjects[i].oconr = NULL;

			if (involvedObjects[i].theConFac)
			{
				res = involvedObjects[i].theTarget->return_hidden_interface(JVX_INTERFACE_CONNECTOR_FACTORY, reinterpret_cast<jvxHandle*>(involvedObjects[i].theConFac));
			}
			involvedObjects[i].theConFac = NULL;
		}
		involvedObjects.clear();
		theConnections = NULL;

		CjvxObject::_deactivate();
		CjvxObject::_unselect();
		CjvxObject::_terminate();

		JVX_DEACTIVATE_DEFAULT_ONE_CONNECTOR_MASTER();
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
HjvxMicroConnection::transfer_forward_connection(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	IjvxDataConnectionProcess* theProc = nullptr;
	jvxErrorType res = theConnections->reference_connection_process_uid(uId_process, &theProc);
	assert(res == JVX_NO_ERROR);

	res = theProc->transfer_forward_chain(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	
	theConnections->return_reference_connection_process(theProc);
	return res;
}

jvxErrorType
HjvxMicroConnection::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (refHandleSimple)
	{
		res = refHandleSimple->hook_forward(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		// There is no reason to forward this call - we are at the end of the micro connection
		// res = transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb)); <- this will crash as it calls itself
	}
	return res;
}

jvxErrorType 
HjvxMicroConnection::transfer_backward_connection(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if ((res == JVX_NO_ERROR) || (res == JVX_ERROR_COMPROMISE))
	{
		// The update of the output may be required for both conditions since the setting may have changed even in case of the COMPROMISE
		if (refHandleSimple)
		{
			// Another return value here which should always be NO_ERROR - but we must return the original version of res!!
			jvxErrorType resL = refHandleSimple->hook_test_update(_common_set_icon.theData_in JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
	}
	return res;
}

jvxErrorType
HjvxMicroConnection::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (refHandleFwd)
	{
		// Copy input parameters number of buffers and allocation type
		theData_outlnk->con_data = _common_set_icon.theData_in->con_data;
		theData_outlnk->con_pipeline = _common_set_icon.theData_in->con_pipeline;

		if (copyAttachedData)
		{
			theData_outlnk->con_link.attached_chain_single_pass = _common_set_icon.theData_in->con_link.attached_chain_single_pass;
		}

		res = refHandleFwd->hook_prepare(JVX_CONNECTION_FEEDBACK_CALL(fdb));

		_common_set_icon.theData_in->con_data = theData_outlnk->con_data;
		_common_set_icon.theData_in->con_pipeline = theData_outlnk->con_pipeline;

	}
	else
	{
		// Link
		if (useBuffersInPlaceOutput)
		{
			_common_set_icon.theData_in->con_data = theData_outlnk->con_data;
			_common_set_icon.theData_in->con_pipeline = theData_outlnk->con_pipeline;
			res = JVX_NO_ERROR;
		}
		else
		{
			// Connect the output side and start this link
			res = allocate_pipeline_and_buffers_prepare_to();

			// Do not attach any user hint into backward direction
			_common_set_icon.theData_in->con_compat.user_hints = NULL;
			*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;

			theData_outlnk->con_data.buffers =
				_common_set_icon.theData_in->con_data.buffers;
			theData_outlnk->con_pipeline = _common_set_icon.theData_in->con_pipeline;

		}
	}
	return res;
}


jvxErrorType
HjvxMicroConnection::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;

	if (refHandleFwd)
	{
		res = refHandleFwd->hook_postprocess(JVX_CONNECTION_FEEDBACK_CALL(fdb));

		// Reset this to null
		_common_set_icon.theData_in->con_data = theData_outlnk->con_data;
		_common_set_icon.theData_in->con_pipeline = theData_outlnk->con_pipeline;
	}
	else
	{

		if (useBuffersInPlaceOutput)
		{
			jvx_neutralDataLinkDescriptor_mem(_common_set_icon.theData_in, true);
		}
		else
		{
			res = deallocate_pipeline_and_buffers_postprocess_to();

			theData_outlnk->con_data.buffers = NULL;
			// theData_outlnk->pipeline = NULL;
		}
	}
	return res;
}

jvxErrorType
HjvxMicroConnection::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (refHandleFwd)
	{
		res = refHandleFwd->hook_start(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		res = __start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
HjvxMicroConnection::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (refHandleFwd)
	{
		res = refHandleFwd->hook_stop(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	else
	{
		res = __stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType 
HjvxMicroConnection::prepare_process_connection(jvxLinkDataDescriptor** lkDataOnReturn,
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock cb,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!useBuffersInPlaceInput)
	{
		if (lkDataOnReturn)
			*lkDataOnReturn = &_common_set_ocon.theData_out;
		res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon(
			pipeline_offset,
			idx_stage,
			tobeAccessedByStage,
			cb,
			priv_ptr);
		assert(res == JVX_NO_ERROR);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
HjvxMicroConnection::postprocess_process_connection(
	jvxSize idx_stage ,
	jvxBool operate_first_call ,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock cb,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (!useBuffersInPlaceInput)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon(
			idx_stage,
			operate_first_call,
			tobeAccessedByStage,
			cb,
			priv_ptr);
		assert(res == JVX_NO_ERROR);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
HjvxMicroConnection::process_connection(jvxLinkDataDescriptor** lkDataOnReturn)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
	assert(res == JVX_NO_ERROR);

	if (lkDataOnReturn)
	{
		*lkDataOnReturn = _common_set_icon.theData_in;
	}

	return res;
}

jvxErrorType
HjvxMicroConnection::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		// What to do here?
		if (refHandleSimple)
		{
			res = refHandleSimple->hook_test_negotiate((jvxLinkDataDescriptor*)data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_NO_ERROR)
			{
				_common_set_ocon.theData_out.con_params = theData_inlnk->con_params;
			}
		}
		else
		{
			jvxLinkDataDescriptor* proposed = (jvxLinkDataDescriptor*)data;
			if (theData_proposeInput)
			{
				theData_proposeInput->con_params = proposed->con_params;
			}

			switch (typeConnection)
			{
			case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED:
			case jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_ADAPT:
				res = JVX_ERROR_UNSUPPORTED;
				break;
			default:
				assert(0);
			}
			
			//
			// This old code does not make any sense to me
			// 
			// _common_set_ocon.theData_out.con_params = proposed->con_params;
			// theData_outlnk->con_params = proposed->con_params;			
		}
		break;
	default:

		// We should stop here!
		res = _transfer_backward_ocon(false, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
HjvxMicroConnection::transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		if (refHandleSimple)
		{
			jvxErrorType res = JVX_NO_ERROR;
			jvxBool isReady = true;
			jvxApiString* astr = (jvxApiString*)data;
			if (refHandleSimple)
			{
				res = refHandleSimple->hook_check_is_ready(&isReady, astr);
				assert(res == JVX_NO_ERROR);
				if (isReady == false)
				{
					return JVX_ERROR_NOT_READY;
				}
			}
		}
	}
	return _transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
HjvxMicroConnection::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	if (refHandleFwd)
	{
		return refHandleFwd->hook_process_start(
			pipeline_offset,
			idx_stage,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}

	return __process_start_icon(
		pipeline_offset,
		idx_stage,
		tobeAccessedByStage,
		clbk,
		priv_ptr);
}

jvxErrorType
HjvxMicroConnection::process_stop_icon(jvxSize idx_stage,
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	if (refHandleFwd)
	{
		return refHandleFwd->hook_process_stop(
			idx_stage,
			shift_fwd,
			tobeAccessedByStage,
			clbk,
			priv_ptr);
	}

	return __process_stop_icon(
		idx_stage,
		shift_fwd,
		tobeAccessedByStage,
		clbk,
		priv_ptr);
}

jvxErrorType
HjvxMicroConnection::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	if (refHandleFwd)
	{
		return refHandleFwd->hook_process(mt_mask, idx_stage);
	}
	return __process_buffers_icon(mt_mask, idx_stage);
}


jvxErrorType
HjvxMicroConnection::connect_chain_master(const jvxChainConnectArguments& args,
	const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate_master(_common_set_ocon.ocon, static_cast<IjvxConnectionMaster*>(this));
	assert(res == JVX_NO_ERROR);

	res = _connect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		jvxErrorType resL = _deactivate_master(_common_set_ocon.ocon, static_cast<IjvxConnectionMaster*>(this));
		assert(resL == JVX_NO_ERROR);
	}

	// Prepare a following test
	if (init_params)
	{
		_common_set_ocon.theData_out.con_params = *init_params;
	}
	return res;
}
/*
jvxErrorType 
HjvxMicroConnection::process_start_icon()
{
	return JVX_NO_ERROR;
}
*/