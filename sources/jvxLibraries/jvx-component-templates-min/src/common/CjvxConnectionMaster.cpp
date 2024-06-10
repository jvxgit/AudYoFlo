#include "common/CjvxConnectionMaster.h"
#include "common/CjvxObject.h"

CjvxConnectionMaster::CjvxConnectionMaster()
	{
		_common_set_ld_master.object = NULL;
		_common_set_ld_master.state = JVX_STATE_INIT;
		_common_set_ld_master.isConnected = false;
		_common_set_ld_master.refProc = NULL;
		_common_set_ld_master.myParent = NULL;

		_common_set_ld_master.oconn = NULL;
		_common_set_ld_master.mas = NULL;

		jvx_bitZSet(_common_set_ld_master.supported_delivery_type, JVX_MASTER_DELIVER_ACTIVE_SHIFT);

		_common_set_ld_master.postpone_attempts_max = JVX_DEFAULT_NUMBER_OF_POSTPONE_ATTEMPTS_TEST;
		theContext = NULL;

		localIterator.set_master(this);
	}
	
CjvxConnectionMaster::~CjvxConnectionMaster()
	{
		_common_set_ld_master.object = NULL;
		_common_set_ld_master.state = JVX_STATE_INIT;
		_common_set_ld_master.refProc = NULL;
		localIterator.set_master(nullptr);
	}
	
	// ===========================================================================

	jvxErrorType 
		CjvxConnectionMaster::_descriptor_master(jvxApiString* nm)
	{
		if (nm)
			nm->assign(_common_set_ld_master.name);
		return JVX_NO_ERROR;
	}

	// ============================================================================
	// INIT
	// ============================================================================

	jvxErrorType 
		CjvxConnectionMaster::_init_master(CjvxObject* object, std::string name, IjvxConnectionMasterFactory* myParent)
	{
		if (_common_set_ld_master.object == NULL)
		{
			jvxApiString astr;
			_common_set_ld_master.object = object;
			_common_set_ld_master.name = name;
			_common_set_ld_master.myParent = myParent;
			object->_requestComponentSpecialization(nullptr, &_common_set_ld_master.cpTpAss, nullptr, nullptr);
			object->_module_reference(&astr, nullptr);
			_common_set_ld_master.tagAss = astr.std_str();
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	};

	// ============================================================================
	// TERMINATE
	// ============================================================================

	jvxErrorType 
		CjvxConnectionMaster::_terminate_master()
	{
		if (_common_set_ld_master.object)
		{
			_common_set_ld_master.object = NULL;
			_common_set_ld_master.name = "";
			_common_set_ld_master.myParent = NULL;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_name_master(jvxApiString* str)
	{
		if (str)
			str->assign(_common_set_ld_master.name);
		return JVX_NO_ERROR;
	}

	// ============================================================================
	// NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME NAME
	// ============================================================================

	jvxErrorType 
		CjvxConnectionMaster::_data_delivery_type(jvxCBitField* deltp)
	{
		if (deltp)
		{
			*deltp = _common_set_ld_master.supported_delivery_type;
		}
		return JVX_NO_ERROR;
	}

	// ============================================================================
	// SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT SELECT
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_select_master(IjvxDataConnectionProcess* ref)
	{
		if(_common_set_ld_master.refProc == NULL)
		{
			assert(_common_set_ld_master.state == JVX_STATE_INIT);
			_common_set_ld_master.state = JVX_STATE_SELECTED;
			_common_set_ld_master.refProc = ref;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// UNSELECT
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_unselect_master()
	{
		if (_common_set_ld_master.state == JVX_STATE_SELECTED)
		{
			_common_set_ld_master.refProc = NULL;
			_common_set_ld_master.oconn = NULL;
			_common_set_ld_master.state = JVX_STATE_INIT;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;		
	}
	
	// ============================================================================
	// AVAILABLE
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_available_master(jvxBool* isAvail)
	{
		if (isAvail)
		{
			*isAvail = (_common_set_ld_master.state == JVX_STATE_INIT);
		}
		return JVX_NO_ERROR;
	}

	// ============================================================================
	// PROCESS REFERENCE
	// ============================================================================
	jvxErrorType
		CjvxConnectionMaster::_associated_process(IjvxDataConnectionProcess** ref)
	{
		if (ref)
		{
			*ref = _common_set_ld_master.refProc;
		}
		return JVX_NO_ERROR;
	}
	// ============================================================================
	// ACTIVATE
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_activate_master(IjvxOutputConnector* conn_out, IjvxConnectionMaster* mas)
	{
		if(_common_set_ld_master.state == JVX_STATE_SELECTED)
		{
			assert(_common_set_ld_master.oconn == NULL);
			_common_set_ld_master.oconn = conn_out;
			_common_set_ld_master.mas = mas;
			_common_set_ld_master.state = JVX_STATE_ACTIVE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// DEACTIVATE
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_deactivate_master(IjvxOutputConnector* conn_out, IjvxConnectionMaster* mas)
	{
		if (_common_set_ld_master.state == JVX_STATE_ACTIVE)
		{
			assert(_common_set_ld_master.oconn == conn_out);
			_common_set_ld_master.oconn = NULL;
			_common_set_ld_master.mas = NULL;
			_common_set_ld_master.state = JVX_STATE_SELECTED;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// CONNECT
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_connect_chain_master(const jvxChainConnectArguments& args
			 JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_ACTIVE) && (!_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->connect_connect_ocon(
					jvxChainConnectArguments(args.uIdConnection, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res != JVX_NO_ERROR)
				{
					// Nothing to do here
				}
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.isConnected = true;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// DISCONNECT
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_disconnect_chain_master(const jvxChainConnectArguments& args, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_ACTIVE) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->disconnect_connect_ocon(
					jvxChainConnectArguments(args.uIdConnection, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.isConnected = false;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE PREPARE
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_ACTIVE) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.state = JVX_STATE_PREPARED;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectionMaster::_prepare_chain_master_no_link()
	{
		if ((_common_set_ld_master.state == JVX_STATE_ACTIVE) && (_common_set_ld_master.isConnected))
		{
			_common_set_ld_master.state = JVX_STATE_PREPARED;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	// ============================================================================
	// POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS POSTPROCESS
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.state = JVX_STATE_ACTIVE;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE; 
	}

	jvxErrorType
		CjvxConnectionMaster::_postprocess_chain_master_no_link()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected))
		{
			_common_set_ld_master.state = JVX_STATE_ACTIVE;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE; 
	}

	jvxBool
		CjvxConnectionMaster::_check_postprocess_chain_master()
	{
		return ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected));
	}

	// ============================================================================
	// START START START START START START START START START START START START START
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.state = JVX_STATE_PROCESSING;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectionMaster::_start_chain_master_no_link()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PREPARED) && (_common_set_ld_master.isConnected))
		{
			_common_set_ld_master.state = JVX_STATE_PROCESSING;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}
	// ============================================================================
	// STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP
	// ============================================================================

	jvxErrorType
		CjvxConnectionMaster::_stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PROCESSING) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			if (res == JVX_NO_ERROR)
			{
				_common_set_ld_master.state = JVX_STATE_PREPARED;
			}
			return res;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxErrorType
		CjvxConnectionMaster::_stop_chain_master_no_link()
	{
		jvxErrorType res = JVX_NO_ERROR;
		if ((_common_set_ld_master.state == JVX_STATE_PROCESSING) && (_common_set_ld_master.isConnected))
		{
			_common_set_ld_master.state = JVX_STATE_PREPARED;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}

	jvxBool 
		CjvxConnectionMaster::_check_stop_chain_master()
	{
		return ((_common_set_ld_master.state == JVX_STATE_PROCESSING) && (_common_set_ld_master.isConnected));
	}

	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================
	// ============================================================================

	/*
	jvxErrorType _ldm_prepare()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		_common_set_ld_master.object->_lock_state();
		if(_common_set_ld_master.state == JVX_STATE_ACTIVE)
		{			
			_common_set_ld_master.state = JVX_STATE_PREPARED;
			res = JVX_NO_ERROR;			
		}
		_common_set_ld_master.object->_unlock_state();
		return res;
	};

	jvxErrorType _ldm_start()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		_common_set_ld_master.object->_lock_state();
		if(_common_set_ld_master.state == JVX_STATE_PREPARED)
		{			
			_common_set_ld_master.state = JVX_STATE_PROCESSING;
			res = JVX_NO_ERROR;
		}
		_common_set_ld_master.object->_unlock_state();
		return res;
	};
	
	jvxErrorType _ldm_stop()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		_common_set_ld_master.object->_lock_state();
		if(_common_set_ld_master.state == JVX_STATE_PROCESSING)
		{			
			_common_set_ld_master.state = JVX_STATE_PREPARED;
			res = JVX_NO_ERROR;
		}
		_common_set_ld_master.object->_unlock_state();
		return res;
	};
	
	jvxErrorType _ldm_postprocess()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		_common_set_ld_master.object->_lock_state();
		if(_common_set_ld_master.state == JVX_STATE_PREPARED)
		{			
			_common_set_ld_master.state = JVX_STATE_ACTIVE;
			res = JVX_NO_ERROR;
		}
		_common_set_ld_master.object->_unlock_state();
		return res;
	};
	*/
	jvxErrorType
		CjvxConnectionMaster::_parent_master_factory(IjvxConnectionMasterFactory** my_parent)
	{
		if(my_parent)
			*my_parent = _common_set_ld_master.myParent;
		return JVX_NO_ERROR;		
	}
	// ===========================================================================

	jvxErrorType
		CjvxConnectionMaster::_inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		switch (tp)
		{
		case JVX_MASTER_UPDATE_EVENT_NONE:
			assert(0);
			break;
		case JVX_MASTER_UPDATE_EVENT_STARTED:
			if (_common_set_ld_master.oconn)
			{
				return _common_set_ld_master.oconn->connect_connect_ocon(
					jvxChainConnectArguments( JVX_SIZE_UNSELECTED, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			break;
		case JVX_MASTER_UPDATE_EVENT_ABOUT_TO_STOP:
			if (_common_set_ld_master.oconn)
			{
				return _common_set_ld_master.oconn->disconnect_connect_ocon(
					jvxChainConnectArguments( JVX_SIZE_UNSELECTED, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			break;
		case JVX_MASTER_UPDATE_EVENT_LAYOUT_CHANGED:
			if (_common_set_ld_master.oconn)
			{
				res = _common_set_ld_master.oconn->disconnect_connect_ocon(
					jvxChainConnectArguments( JVX_SIZE_UNSELECTED, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res)
				{
					res = _common_set_ld_master.oconn->connect_connect_ocon(
						jvxChainConnectArguments( JVX_SIZE_UNSELECTED, _common_set_ld_master.mas) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				return res;
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			break;
		}
		return res;
	}

	// ===========================================================================

	jvxErrorType
		CjvxConnectionMaster::_transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
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
			locTxt = str->std_str() + "master <" + _common_set_ld_master.name + "> ";
			locTxt += JVX_DISPLAY_MASTER(_common_set_ld_master.mas);
			locTxt += "-chain entry";
			if (_common_set_ld_master.oconn)
			{
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<int>->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);
				res = _common_set_ld_master.oconn->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				locTxt = str->std_str();
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				locTxt += "<-<tni>-";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_STOP;
				str->assign(locTxt);
				return res;
			}
			std::cout << "<end>" << std::endl;
			return JVX_NO_ERROR;
			break;
		case  JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON:
			if (!jsonLst)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			jsonElm.makeAssignmentString("master", JVX_DISPLAY_MASTER(_common_set_ld_master.mas));
			jsonLst->addConsumeElement(jsonElm);
			if (_common_set_ld_master.oconn)
			{
				CjvxJsonElementList jsonLstRet;
				res = _common_set_ld_master.oconn->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					jsonElm.makeSection("master_connects_from", jsonLstRet);
				}
				else
				{
					jsonElm.makeAssignmentString("connects_error", jvxErrorType_txt(res));
				}
				jsonLst->addConsumeElement(jsonElm);
			}
			return JVX_NO_ERROR;
			break;
		default:
			if (_common_set_ld_master.oconn)
			{
				return _common_set_ld_master.oconn->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	// ===========================================================================

	jvxErrorType
		CjvxConnectionMaster::_transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		return JVX_ERROR_UNSUPPORTED;
	};

	// ===========================================================================

	jvxErrorType
		CjvxConnectionMaster::_test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_NO_ERROR;
		JVX_CONNECTION_FEEDBACK_ON_ENTER_FAC_MAS(fdb, _common_set_ld_master.myParent);
		if ((_common_set_ld_master.state == JVX_STATE_ACTIVE) && (_common_set_ld_master.isConnected))
		{
			if (_common_set_ld_master.oconn)
			{
				jvxSize cnt = 0;
				while (cnt < _common_set_ld_master.postpone_attempts_max)
				{
					res = _common_set_ld_master.oconn->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
					if (res != JVX_ERROR_POSTPONE)
					{
						break;
					}
					cnt++;
				}
				if (res == JVX_ERROR_POSTPONE)
				{
					std::cout << __FUNCTION__ << ": WARNING: Test of chain " << _common_set_ld_master.name <<
						" failed due to a number of postpone requests exceeding the maximum value of <" <<
						_common_set_ld_master.postpone_attempts_max << ">." << std::endl;
					res = JVX_ERROR_UNSUPPORTED;
				}
			}
			else
			{
				std::cout << "Master does not provide any output connector!" << std::endl;
			}
			return res;
		}
		return JVX_ERROR_ABORT;
	};

	jvxErrorType
		CjvxConnectionMaster::_set_connection_context(IjvxDataConnectionCommon* ctxt)
	{
		if (ctxt)
		{
			if(theContext == NULL)
			{
				theContext = ctxt;
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_ALREADY_IN_USE;
		}
		if (theContext)
		{
			theContext = ctxt;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	};

	jvxErrorType
		CjvxConnectionMaster::_connection_context(IjvxDataConnectionCommon** ctxt)
	{
		if (ctxt)
			*ctxt = theContext;
		return JVX_NO_ERROR;
	};

	jvxErrorType
		CjvxConnectionMaster::_iterator_chain(IjvxConnectionIterator** it)
	{
		if (it)
		{
			*it = &localIterator;
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CjvxConnectionMaster::_connection_association(jvxApiString* astr, jvxComponentIdentification* cpId)
	{
		if (astr)
		{
			astr->assign(_common_set_ld_master.tagAss);
		}
		if (cpId)
		{
			*cpId = _common_set_ld_master.cpTpAss;
		}

		return JVX_NO_ERROR;
	}
	// =====================================================================================
	/*
	jvxErrorType _ldm_activate(CjvxObject* my_object)
	{
	if(_common_set_ld_master.state == JVX_STATE_INIT)
	{
	_common_set_ld_master.state = JVX_STATE_ACTIVE;
	_common_set_ld_master.object = my_object;
	return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
	};

	jvxErrorType _ldm_deactivate()
	{
	if(_common_set_ld_master.state == JVX_STATE_ACTIVE)
	{
	_common_set_ld_master.state = JVX_STATE_INIT;
	_common_set_ld_master.object = NULL;
	return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
	};
	*/
	jvxErrorType 
		CjvxConnectionMasterIterator::number_next(jvxSize* num)
	{
		if (num)
		{
			*num = 1;
		}
		return JVX_NO_ERROR;
	}
	
	jvxErrorType
		CjvxConnectionMasterIterator::reference_next(jvxSize idx, IjvxConnectionIterator** next) 
	{
		if (idx == 0)
		{
			if (next)
			{
				*next = nullptr;
				if (parentMaster)
				{
					if (parentMaster->_common_set_ld_master.oconn)
					{
						IjvxInputConnector* icon = nullptr;
						parentMaster->_common_set_ld_master.oconn->connected_icon(&icon);
						if (icon)
						{
							*next = static_cast<IjvxConnectionIterator*>(icon);
						}
					}
				}
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	};

	jvxErrorType
		CjvxConnectionMasterIterator::reference_component(
			jvxComponentIdentification* cpTp, 
			jvxApiString* modName,			
			jvxApiString* description,
			jvxApiString* lContext)
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxComponentIdentification cpId;

		if (parentMaster)
		{
			IjvxObject* objRef = nullptr;
			parentMaster->_common_set_ld_master.myParent->request_reference_object(&objRef);
			if (objRef)
			{
				res = objRef->request_specialization(nullptr, &cpId, nullptr);
				objRef->module_reference(modName, nullptr);
				objRef->description(description);
				parentMaster->_common_set_ld_master.myParent->return_reference_object(objRef);
				if (lContext)
					lContext->assign(parentMaster->_common_set_ld_master.name);
			}
		}
		
		if (cpTp)
		{
			*cpTp = cpId;
		}
		return JVX_NO_ERROR;
	};

	void
		CjvxConnectionMasterIterator::set_master(CjvxConnectionMaster* master)
	{
		parentMaster = master;
	}

	