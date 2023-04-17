#include "common/CjvxInputOutputConnector.h"

CjvxInputOutputConnector::CjvxInputOutputConnector()
	{
		_common_set_ldslave.data_processor = NULL;
		_common_set_ldslave.object = NULL;
		_common_set_ldslave.icon = NULL;
		_common_set_ldslave.ocon = NULL;
		_common_set_ldslave.descriptor = "none";

		_common_set_ldslave.myParent = NULL;
		_common_set_ldslave.theMaster = NULL;
		_common_set_ldslave.num_min_buffers_in = 1;
		_common_set_ldslave.num_min_buffers_out = 1;
		_common_set_ldslave.num_additional_pipleline_stages = 0;
		_common_set_ldslave.zeroCopyBuffering_cfg = false;
		_common_set_ldslave.myRuntimeId = JVX_SIZE_UNSELECTED;
		_common_set_ldslave.allows_termination = true;
		_common_set_ldslave.setup_for_termination = false;
		_common_set_ldslave.copy_timestamp_inout = true;

		_common_set_ldslave.detectLoop = false;

		// _common_set_ldslave.caps_in with default settings
		// _common_set_ldslave.caps_out with default settings

#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		_common_set_ldslave.dbg_hint = NULL;
#endif
}

CjvxInputOutputConnector::~CjvxInputOutputConnector()
	{
	}

	jvxErrorType 
		CjvxInputOutputConnector::CjvxInputOutputConnector::_available_connector(jvxBool* isAvail)
	{
		if (isAvail)
		{
			*isAvail = false;
			if (
				(_common_set_icon.conn_in == NULL) &&
				(_common_set_icon.conn_in == NULL)) // <-
			{
				*isAvail = true;
			}
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_associated_common_icon(IjvxDataConnectionCommon** ref)
	{
		if (ref)
		{
			*ref = _common_set_icon.theCommon_to;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxInputOutputConnector::_connected_ocon(IjvxOutputConnector** ocon)
	{
		if (ocon)
		{
			*ocon = nullptr;
			if (_common_set_icon.theData_in)
			{
				*ocon = _common_set_icon.theData_in->con_link.connect_from;
			}
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxInputOutputConnector::_connected_icon(IjvxInputConnector** ocon)
	{
		if (ocon)
		{
			*ocon = nullptr;
			*ocon = _common_set_ocon.theData_out.con_link.connect_to;
		}
		return JVX_NO_ERROR;
	}
	
	jvxErrorType
		CjvxInputOutputConnector::_associated_common_ocon(IjvxDataConnectionCommon** ref)
	{
		if (ref)
		{
			*ref = _common_set_ocon.theCommon_from;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_descriptor_connector(jvxApiString* str)
	{
		if (str)
		{
			str->assign(_common_set_ldslave.descriptor);
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::lds_activate(IjvxDataProcessor* theDataProc, IjvxObject* theObjRef,
		IjvxConnectorFactory* myPar, IjvxConnectionMaster* master, std::string descror,
		IjvxInputConnector* icon, IjvxOutputConnector* ocon
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, const char* dbg_hint
#endif
		)
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = theDataProc;
		_common_set_ldslave.object = theObjRef;
		_common_set_ldslave.myParent = myPar;
		_common_set_ldslave.myParent = myPar;
		_common_set_ldslave.descriptor = descror;
		_common_set_ldslave.icon = icon;
		_common_set_ldslave.ocon = ocon;
		_common_set_ldslave.detectLoop = false;
		_common_set_ldslave.theMaster = master;
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		_common_set_ldslave.dbg_hint = dbg_hint;
#endif
		return res;
	}

	/*
	jvxErrorType
		CjvxInputOutputConnector::_outputs_data_format_group(jvxDataFormatGroup grp)
	{
		if (grp == _common_set_ldslave.format_group_out)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	*/

	jvxErrorType 
		CjvxInputOutputConnector::_supports_connector_class_ocon(
			jvxDataFormatGroup format_group,
			jvxDataflow data_flow)
	{		
		if (_common_set_ocon.caps_out.format_group != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (_common_set_ocon.caps_out.format_group != format_group)
			{
				return JVX_ERROR_UNSUPPORTED;
			}
		}
		if (_common_set_ocon.caps_out.data_flow!= JVX_DATAFLOW_NONE)
		{
			if (_common_set_ocon.caps_out.data_flow != data_flow)
			{
				return JVX_ERROR_UNSUPPORTED;
			}
		}
		return JVX_NO_ERROR;
	}
	jvxErrorType 
		CjvxInputOutputConnector::_supports_connector_class_icon(
			jvxDataFormatGroup format_group,
			jvxDataflow data_flow)
	{
		if (_common_set_icon.caps_in.format_group != JVX_DATAFORMAT_GROUP_NONE)
		{
			if (_common_set_icon.caps_in.format_group != format_group)
			{
				return JVX_ERROR_UNSUPPORTED;
			}
		}
		if (_common_set_icon.caps_in.data_flow != JVX_DATAFLOW_NONE)
		{
			if (_common_set_icon.caps_in.data_flow != data_flow)
			{
				return JVX_ERROR_UNSUPPORTED;
			}
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::lds_deactivate()
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_ldslave.data_processor = NULL;
		_common_set_ldslave.object = NULL;
		_common_set_ldslave.icon = NULL;
		_common_set_ldslave.ocon = NULL;
		_common_set_ldslave.myParent = NULL;
		_common_set_ldslave.theMaster = NULL;
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_set_parent_factory(IjvxConnectorFactory* my_parent)
	{
		_common_set_ldslave.myParent = my_parent;
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_parent_factory(IjvxConnectorFactory** my_parent)
	{
		if (my_parent)
			*my_parent = _common_set_ldslave.myParent;
		return JVX_NO_ERROR;
	}

	// ==============================================================
	// SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_select_connect_icon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master, 
			IjvxDataConnectionCommon* ass_connection_common,
			IjvxInputConnector** replace_connector)
	{
		// To and from conector must be associated in the same process/group
		if (_common_set_icon.theCommon_to != NULL)
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}

		if (_common_set_ocon.theCommon_from != NULL)
		{
			if (ass_connection_common != _common_set_ocon.theCommon_from)
			{
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		// If we are linked with a master since we are a master with connectors, passed master must match
		if (master && _common_set_ldslave.theMaster)
		{
			if (master != _common_set_ldslave.theMaster)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		if (_common_set_icon.conn_in == NULL)
		{
			_common_set_icon.conn_in = bri;
			_common_set_icon.theCommon_to = ass_connection_common;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_select_connect_ocon(IjvxConnectorBridge* bri, IjvxConnectionMaster* master, 
			IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector)
	{
		// To and from conector must be associated in the same process/group
		if (_common_set_ocon.theCommon_from != NULL)
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}

		if (_common_set_icon.theCommon_to != NULL)
		{
			if (ass_connection_common != _common_set_icon.theCommon_to)
			{
				return JVX_ERROR_INVALID_SETTING;
			}
		}

		// If we are linked with a master since we are a master with connectors, passed master must match
		if (master && _common_set_ldslave.theMaster)
		{
			if (master != _common_set_ldslave.theMaster)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
		}

		// If no bridge was set before
		if (_common_set_ocon.conn_out == NULL)
		{
			_common_set_ocon.conn_out = bri;
			_common_set_ocon.theCommon_from = ass_connection_common;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_COMPONENT_BUSY;
	}

	// ==============================================================
	// UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT UNSELECT
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_unselect_connect_icon(IjvxConnectorBridge* bri, IjvxInputConnector* replace_connector)
	{
		if (_common_set_icon.conn_in == bri)
		{
			_common_set_icon.conn_in = NULL;
			_common_set_icon.theCommon_to = NULL;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_unselect_connect_ocon(IjvxConnectorBridge* bri, IjvxOutputConnector* replace_connector)
	{
		if (_common_set_ocon.conn_out)
		{
			_common_set_ocon.conn_out = NULL;
			_common_set_ocon.theCommon_from = NULL;

			// What else to do here?
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ==============================================================
	// CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT CONNECT
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxBool forward)
	{
		jvxErrorType res = JVX_NO_ERROR;
		_common_set_icon.theData_in = theData;

		if (forward)
		{
			assert(_common_set_ldslave.detectLoop == false);

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
			if (_common_set_ldslave.ocon)
			{
				_common_set_ldslave.detectLoop = true;
				res = _common_set_ldslave.ocon->connect_connect_ocon(
					jvxChainConnectArguments(theData->con_link.uIdConn, theData->con_link.master) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				_common_set_ldslave.detectLoop = false;
				if (res != JVX_NO_ERROR)
				{
					_common_set_icon.theData_in = NULL;
				}
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		else
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb), jvxDataProcessorHintDescriptor* add_me)
	{
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		if (_common_set_ocon.conn_out)
		{
			jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
			assert(_common_set_ocon.theData_out.con_link.connect_to == NULL);
			_common_set_ocon.conn_out->reference_connect_to(&_common_set_ocon.theData_out.con_link.connect_to);
			if (_common_set_ocon.theData_out.con_link.connect_to)
			{
				_common_set_ocon.theData_out.con_link.master = args.theMaster;
				_common_set_ocon.theData_out.con_link.uIdConn = args.uIdConnection;
				_common_set_ocon.theData_out.con_link.connect_from = _common_set_ldslave.ocon;
				_common_set_ocon.theData_out.con_user.chain_spec_user_hints = add_me;

				res = _common_set_ocon.theData_out.con_link.connect_to->connect_connect_icon(&_common_set_ocon.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res != JVX_NO_ERROR)
				{
					_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
					_common_set_ocon.theData_out.con_link.connect_to = NULL;
					jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
				}
			}
			else
			{
				assert(0);
				if (!_common_set_ldslave.allows_termination)
				{
					std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
					res = JVX_ERROR_NOT_READY;
				}
			}
		}
		else
		{
			if (!_common_set_ldslave.allows_termination)
			{
				std::cout << "This output connector does not allow termination but is not connected to a connected bridge (no input connector in bridge)." << std::endl;
				res = JVX_ERROR_NOT_READY;
			}
		}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		return res;
	}

	// ==============================================================
	// DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT DISCONNECT
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_disconnect_connect_icon(jvxLinkDataDescriptor* theData, jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (forward)
		{
			if (_common_set_ldslave.ocon)
			{
				res = _common_set_ldslave.ocon->disconnect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn), 
					JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
		}
		_common_set_icon.theData_in = NULL;
		return res;
	}

	jvxErrorType
		CjvxInputOutputConnector::_disconnect_connect_ocon(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE_F(fdb) jvxDataProcessorHintDescriptor** pointer_remove)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (_common_set_ocon.conn_out)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->disconnect_connect_icon(&_common_set_ocon.theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (pointer_remove)
			{
				*pointer_remove = _common_set_ocon.theData_out.con_user.chain_spec_user_hints;
			}
			_common_set_ocon.conn_out->return_reference_connect_to(_common_set_ocon.theData_out.con_link.connect_to);
			_common_set_ocon.theData_out.con_link.connect_to = nullptr;
			jvx_terminateDataLinkDescriptor(&_common_set_ocon.theData_out);
		}
		else
		{
			std::cout << "Not connected" << std::endl;
		}
		return res;
	}


	// ==============================================================
	// PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		// Content of link descriptor has been set before...
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

		return res;
	};



	jvxErrorType 
		CjvxInputOutputConnector::_prepare_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (forward)
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
			assert(_common_set_ldslave.detectLoop == false);
			if (_common_set_ldslave.ocon)
			{
				_common_set_ldslave.detectLoop = true;
				res = _common_set_ldslave.ocon->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				_common_set_ldslave.detectLoop = false;
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		else
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		return res;
	}

	// ==============================================================
	// POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		// Content of link descriptor has been set before...
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
			
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_postprocess_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (forward)
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
			assert(_common_set_ldslave.detectLoop == false);
			if (_common_set_ldslave.ocon)
			{
				_common_set_ldslave.detectLoop = true;
				_common_set_ldslave.ocon->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				_common_set_ldslave.detectLoop = false;
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		else
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		return res;
	}

	// ==============================================================
	// START START START START START START START START START START START
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_start_connect_icon(jvxBool forward, jvxSize myUniquePipelineId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		_common_set_ldslave.myRuntimeId = myUniquePipelineId;
		if (forward)
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
			assert(_common_set_ldslave.detectLoop == false);
			if (_common_set_ldslave.ocon)
			{
				_common_set_ldslave.detectLoop = true;
				_common_set_ldslave.ocon->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				_common_set_ldslave.detectLoop = false;
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		else
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		// Content of link descriptor has been set before...
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

		return res;
	}

	// ==============================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_stop_connect_icon(jvxBool forward, jvxSize * uId JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (forward)
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
			assert(_common_set_ldslave.detectLoop == false);
			if (_common_set_ldslave.ocon)
			{
				_common_set_ldslave.detectLoop = true;
				_common_set_ldslave.ocon->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				_common_set_ldslave.detectLoop = false;
			}
			else
			{
				std::cout << "Not connected" << std::endl;
			}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif
		}
		else
		{
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
			std::cout << "Entering & Leaving " << __FUNCTION__ << std::endl;
#endif
		}

		if (uId)
			*uId = _common_set_ldslave.myRuntimeId;

		_common_set_ldslave.myRuntimeId = JVX_SIZE_UNSELECTED;

		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		// Content of link descriptor has been set before...
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Entering " << __FUNCTION__ << std::endl;
#endif
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
#ifdef JVX_VERBOSE_CJVXINPUTCONNECTOR_H
		std::cout << "Leaving " << __FUNCTION__ << std::endl;
#endif

		return res;
	}

	// =========================================================================
	// =========================================================================
	// =========================================================================
	// =========================================================================

	// ==============================================================
	// PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART PROCESSSTART
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_start_icon(jvxSize pipeline_offset , jvxSize* idx_stage, jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk, jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Propagate to next object
		if (_common_set_ldslave.ocon)
		{
			// Input / output connector implementations are never zerocopy
			// We start a regular output connector here
			res = _common_set_ldslave.ocon->process_start_ocon(0, NULL, 0, NULL, NULL);
		}

		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_start(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage, jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk,
			jvxHandle* priv_ptr )
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Propagate to next object
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage,
				clbk, priv_ptr);
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}

		return res;
	}

	// ==============================================================
	// PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER PROCESSBUFFER
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (_common_set_ldslave.ocon)
		{
			// Copy the admin data
			_common_set_ocon.theData_out.admin = _common_set_icon.theData_in->admin;
			_common_set_ldslave.ocon->process_buffers_ocon(mt_mask, idx_stage);

			//theData->link.descriptor_out_linked->admin = theData->admin;
			//res = theData->link.next->process_buffers_icon();
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon(mt_mask, idx_stage);
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		return res;
	}

	// ==============================================================
	// PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP PROCESSSTOP
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::_process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock clbk,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;


		if (_common_set_ldslave.ocon)
		{
			// Input / output connector implementations are never zerocopy
			// We start a regular output connector here
			res = _common_set_ldslave.ocon->process_stop_ocon(JVX_SIZE_UNSELECTED, true, tobeAccessedByStage , clbk, priv_ptr);
		}

		if (res == JVX_NO_ERROR)
		{
			res = shift_buffer_pipeline_idx_on_stop(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
		}
		return res;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
			jvxSize tobeAccessedByStage ,
			callback_process_stop_in_lock clbk,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon(
				idx_stage, shift_fwd, 
				tobeAccessedByStage,
				clbk, priv_ptr);
		}
		else
		{
			if (_common_set_ldslave.allows_termination)
			{
				// JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "Input / output connector with allowed termination.", JVX_ERROR_NOT_READY);
				// Accepted as chain terminator
			}
			else
			{
				std::cout << "This output connector is not connected to any bridge." << std::endl;
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		return res;
	}

	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================

	// ==============================================================
	// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::_test_connect_icon(jvxBool forward JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_ldslave.object),
			_common_set_ldslave.descriptor.c_str(), "Entering single default CjvxInputOutputConnector input connector");

		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, _common_set_icon.theData_in);
		if (forward)
		{
			if (_common_set_ldslave.ocon)
			{
				return _common_set_ldslave.ocon->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			return JVX_ERROR_UNSUPPORTED;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::_test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, static_cast<IjvxObject*>(_common_set_ldslave.object), 
			_common_set_ldslave.descriptor.c_str(), "Leaving single default CjvxInputOutputConnector output connector");
		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, (&_common_set_ocon.theData_out));

		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			_common_set_ldslave.setup_for_termination = false;
			return _common_set_ocon.theData_out.con_link.connect_to->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

		if (_common_set_ldslave.allows_termination)
		{
			JVX_CONNECTION_FEEDBACK_SET_COMMENT_STRING(fdb, "Input / output connector with allowed termination.");
			_common_set_ldslave.setup_for_termination = true;
			return JVX_NO_ERROR;
		}
		
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING_TEST(fdb, "No output connection.", JVX_ERROR_NOT_READY);
		return JVX_ERROR_NOT_READY;
	}

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_forward_icon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string locTxt;
		jvxApiString* str = (jvxApiString*)data;
		CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
		CjvxJsonElement jsonElm;

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

			locTxt = str->std_str();
			if (_common_set_ldslave.icon)
			{
				locTxt += "object input connector " + JVX_DISPLAY_CONNECTOR(_common_set_ldslave.icon);
			}
			if (forward)
			{
				if (_common_set_ldslave.ocon)
				{
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
					locTxt += "-<int>->";
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
					str->assign(locTxt);
					res = _common_set_ldslave.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					locTxt = str->std_str();
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
					locTxt += "<-<tni>-";
					locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				}
				else
				{
					locTxt += "-<ncon>";
				}
			}
			else
			{
				locTxt += "<end>";
			}
			str->assign(locTxt);
			return res;
			break;

		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

			if (_common_set_ldslave.icon)
			{
				jsonElm.makeAssignmentString("icon", JVX_DISPLAY_CONNECTOR(_common_set_ldslave.icon));
				jsonLst->addConsumeElement(jsonElm);
			}
			if (forward)
			{
				if (_common_set_ldslave.ocon)
				{
					CjvxJsonElementList jsonLstRet;
					res = _common_set_ldslave.ocon->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						jsonElm.makeSection("connects_from", jsonLstRet);
					}
					else
					{
						jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
					}
					jsonLst->addConsumeElement(jsonElm);
				}
			}
			else
			{
				CjvxJsonElementList jsonLstRet;
				jsonElm.makeAssignmentString("connects_end", "here");
				jsonLst->addConsumeElement(jsonElm);
			}
			return JVX_NO_ERROR;
			break;

		case JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY:

			// This is supported
			if (forward)
			{
				if (_common_set_ldslave.ocon)
				{
					res = _common_set_ldslave.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
			}
			return res;
			break;

		default:
			if (forward)
			{
				if (_common_set_ldslave.ocon)
				{
					return _common_set_ldslave.ocon->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				return JVX_ERROR_UNSUPPORTED;
			}
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType 
	CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{

		jvxErrorType res = JVX_NO_ERROR;
		std::string locTxt;
		jvxApiString* str = (jvxApiString*)data;
		CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
		CjvxJsonElement jsonElm;

		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:

			if (!str)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			locTxt = str->std_str();
			if (_common_set_ldslave.ocon)
			{
				locTxt += "object output connector " + JVX_DISPLAY_CONNECTOR(_common_set_ldslave.ocon);
			}
			if (_common_set_ocon.theData_out.con_link.connect_to)
			{
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<ext>->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);
				res = _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, str JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				locTxt = str->std_str();
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locTxt += "<-<txe>-";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				str->assign(locTxt);
				return res;
			}
			locTxt += "-<not_connected>";
			str->assign(locTxt);
			return JVX_NO_ERROR;
			break;
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:

			if (!jsonLst)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			if (_common_set_ldslave.ocon)
			{
				jsonElm.makeAssignmentString("ocon", JVX_DISPLAY_CONNECTOR(_common_set_ldslave.ocon));
				jsonLst->addConsumeElement(jsonElm);
				if (_common_set_ocon.theData_out.con_link.connect_to)
				{
					CjvxJsonElementList jsonLstRet;
					res = _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						jsonElm.makeSection("connects_to", jsonLstRet);
					}
					else
					{
						jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
					}
					jsonLst->addConsumeElement(jsonElm);
				}
			}
			return JVX_NO_ERROR;
			break;
		default:
			if (_common_set_ocon.theData_out.con_link.connect_to)
			{
				return _common_set_ocon.theData_out.con_link.connect_to->transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	// ==============================================================
	// TRANSFER FORWARD
	// ==============================================================

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_backward_ocon(jvxBool forward, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_ldslave.object,
			_common_set_ldslave.descriptor.c_str(), "Entering transfer_backward on output onnector");

		if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
		{
			if (_common_set_ocon.theData_out.con_user.chain_spec_user_hints)
			{
				jvxDataProcessorHintDescriptorSearch* ptr = (jvxDataProcessorHintDescriptorSearch*)data;
				if (data)
				{
					jvxErrorType res = jvx_hintDesriptor_find(_common_set_ocon.theData_out.con_user.chain_spec_user_hints,
						&ptr->reference, ptr->hintDescriptorId, ptr->assocHint);
					if (res == JVX_NO_ERROR)
					{
						return res;
					}
				}
			}
		}

		if (forward)
		{
			if (_common_set_ldslave.icon)
			{
				return _common_set_ldslave.icon->transfer_backward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			return JVX_ERROR_UNSUPPORTED;
		}
		if (tp == JVX_LINKDATA_TRANSFER_REQUEST_CHAIN_SPECIFIC_DATA_HINT)
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION _transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, _common_set_ldslave.object,
			_common_set_ldslave.descriptor.c_str(), "Entering transfer_backward on input onnector");
		if (_common_set_icon.theData_in->con_link.connect_from)
		{
			return _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	// ==============================================================

	jvxErrorType
		CjvxInputOutputConnector::_number_next(jvxSize* num)
	{
		if (num)
		{
			*num = 1;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxInputOutputConnector::_reference_next(jvxSize idx, IjvxConnectionIterator** next)
	{
		if(idx == 0)
		{
			if (next)
			{
				*next = nullptr;
				IjvxInputConnector* icon = nullptr;
				this->_connected_icon(&icon);
				if (icon)
				{
					*next = static_cast<IjvxConnectionIterator*>(icon);
				}
			}
			return JVX_NO_ERROR;

		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}

	jvxErrorType
		CjvxInputOutputConnector::_reference_component(
			jvxComponentIdentification* cpTp, 
			jvxApiString* modName,
			jvxApiString* lContext)
	{
		jvxComponentIdentification cpId;
		IjvxObject* objRef = nullptr;
		if (_common_set_ldslave.myParent)
		{
			_common_set_ldslave.myParent->request_reference_object(&objRef);
			if (objRef)
			{
				objRef->request_specialization(nullptr, &cpId, nullptr);
				objRef->module_reference(modName, nullptr);
				_common_set_ldslave.myParent->return_reference_object(objRef);
				if (lContext)
					lContext->assign(_common_set_ldslave.descriptor);
			}
		}
		
		if (cpTp)
		{
			*cpTp = cpId;
		}
		return JVX_NO_ERROR;
	}
	

	// ==============================================================
	// HELPER FUNCTIONS
	// ==============================================================
	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_start(jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock clbk = NULL,
			jvxHandle* priv_ptr = NULL)
	{
		return  jvx_shift_buffer_pipeline_idx_on_start(_common_set_icon.theData_in, _common_set_ldslave.myRuntimeId,
			pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION shift_buffer_pipeline_idx_on_stop(jvxSize idx_stage, 
			jvxBool shift_fwd, jvxSize tobeAccessed,
			callback_process_stop_in_lock clbk, jvxHandle* priv_ptr)
	{
		return jvx_shift_buffer_pipeline_idx_on_stop(
			_common_set_icon.theData_in, idx_stage,
			shift_fwd, tobeAccessed, 
			clbk, priv_ptr);
	}

	jvxErrorType CjvxInputOutputConnector::JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to()
	{ 

		return jvx_allocate_pipeline_and_buffers_prepare_to(_common_set_icon.theData_in
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		, _common_set_ldslave.dbg_hint
#endif
		);
	};

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION allocate_pipeline_and_buffers_prepare_to_zerocopy()
	{
		return jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(_common_set_icon.theData_in, &_common_set_ocon.theData_out);
	}

	jvxErrorType 
		CjvxInputOutputConnector::JVX_CALLINGCONVENTION deallocate_pipeline_and_buffers_postprocess_to()
	{ 
		return jvx_deallocate_pipeline_and_buffers_postprocess_to(_common_set_icon.theData_in);
	}

	jvxErrorType 
		JVX_CALLINGCONVENTION CjvxInputOutputConnector::deallocate_pipeline_and_buffers_postprocess_to_zerocopy()
	{
		return jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(_common_set_icon.theData_in);
	}

	void
		CjvxInputOutputConnector::request_test_chain(IjvxReport* theRep)
	{
		jvxCBitField req = 0;
		IjvxDataConnectionCommon* ctxt = NULL;
		jvxSize idConn = JVX_SIZE_UNSELECTED;

		jvx_bitSet(req, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT);
		if (_common_set_icon.theData_in)
		{
			if (_common_set_icon.theData_in->con_link.master)
			{
				_common_set_icon.theData_in->con_link.master->connection_context(&ctxt);
				if (ctxt)
				{
					ctxt->unique_id_connections(&idConn);
					intptr_t idMaster = (intptr_t)idConn;
					theRep->report_command_request(req, (jvxHandle*)idMaster, 0);
				}
			}
		}
	}

jvxErrorType
CjvxInputOutputConnector::request_process_id_chain(jvxSize* procId)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	IjvxDataConnectionProcess* theProc = NULL;
	if (_common_set_icon.theData_in)
	{
		if (_common_set_icon.theData_in->con_link.master)
		{
			_common_set_icon.theData_in->con_link.master->associated_process(&theProc);
			if (theProc)
			{
				theProc->unique_id_connections(procId);
				res = JVX_NO_ERROR;
			}
		}
	}
	return res;
}

	