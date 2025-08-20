#include "common/CjvxSingleMaster.h"

#ifndef JVX_SKIP_EVENT_JSON
#include "CjvxJson.h"
#endif

CjvxSingleMaster::CjvxSingleMaster()
{
}

jvxErrorType 
CjvxSingleMaster::name_master(jvxApiString* str) 
{
	str->assign(_common_set_ma_common.name);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::data_delivery_type(jvxCBitField* delTp) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::select_master(IjvxDataConnectionProcess* ref) 
{
	if (runtime.proc == nullptr)
	{
		runtime.proc = ref;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType 
CjvxSingleMaster::unselect_master() 
{
	if (runtime.proc != nullptr)
	{
		runtime.proc = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType 
CjvxSingleMaster::available_master(jvxBool* isAvail)
{
	if (isAvail)
	{
		*isAvail = (runtime.proc == nullptr);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::associated_process(IjvxDataConnectionProcess** ref)
{
	if (ref)
	{
		*ref = runtime.proc;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::set_connection_context(IjvxDataConnectionCommon* ctxt)
{
	runtime.connCtx = ctxt;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::connection_context(IjvxDataConnectionCommon** ctxt)
{
	if (ctxt) *ctxt = runtime.connCtx;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::connect_chain_master(const jvxChainConnectArguments& params, const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_INIT)
	{
		if (_common_set_ma_common.conn_out)
		{
			if (_common_set_ma_common.cbRef)
			{
				_common_set_ma_common.cbRef->before_connect_chain();
			}
			res = _common_set_ma_common.conn_out->connect_connect_ocon(
				jvxChainConnectArguments(params.uIdConnection, this) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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
			runtime.stat = JVX_STATE_ACTIVE;
		}
		return res;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType 
CjvxSingleMaster::disconnect_chain_master(const jvxChainConnectArguments& params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if(runtime.stat == JVX_STATE_ACTIVE)
	{
		if (_common_set_ma_common.conn_out)
		{
			res = _common_set_ma_common.conn_out->disconnect_connect_ocon(
				jvxChainConnectArguments(params.uIdConnection, this) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		else
		{
			std::cout << "Master does not provide any output connector!" << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			runtime.stat = JVX_STATE_INIT;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_MASTER_UPDATE_EVENT_NONE:
		assert(0);
		break;
	case JVX_MASTER_UPDATE_EVENT_STARTED:
		res = this->connect_chain_master(jvxChainConnectArguments(JVX_SIZE_UNSELECTED, this), nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	case JVX_MASTER_UPDATE_EVENT_ABOUT_TO_STOP:
		res = this->disconnect_chain_master(jvxChainConnectArguments(JVX_SIZE_UNSELECTED, this) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	case JVX_MASTER_UPDATE_EVENT_LAYOUT_CHANGED:
		res = this->disconnect_chain_master(jvxChainConnectArguments(JVX_SIZE_UNSELECTED, this) JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			res = this->connect_chain_master(jvxChainConnectArguments(JVX_SIZE_UNSELECTED, this), nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	}
	return res;
}

jvxErrorType 
CjvxSingleMaster::transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string locTxt;
	jvxApiString* str = (jvxApiString*)data;

#ifndef JVX_SKIP_EVENT_JSON
	CjvxJsonElementList* jsonLst = (CjvxJsonElementList*)data;
	CjvxJsonElement jsonElm;
#endif

	if (runtime.stat == JVX_STATE_ACTIVE)
	{
		switch (tp)
		{
		case JVX_LINKDATA_TRANSFER_COLLECT_LINK_STRING:
			if (!str)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			locTxt = str->std_str() + "master <" + _common_set_ma_common.descriptor + "> ";
			locTxt += JVX_DISPLAY_MASTER(this);
			locTxt += "-chain entry";
			if (_common_set_ma_common.conn_out)
			{
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				locTxt += "-<int>->";
				locTxt += JVX_TEXT_NEW_SEGMENT_CHAR_START;
				str->assign(locTxt);
				res = _common_set_ma_common.conn_out->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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

#ifndef JVX_SKIP_EVENT_JSON
			if (!jsonLst)
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			jsonElm.makeAssignmentString("master", JVX_DISPLAY_MASTER(this));
			jsonLst->addConsumeElement(jsonElm);
			if (_common_set_ma_common.conn_out)
			{
				CjvxJsonElementList jsonLstRet;
				res = _common_set_ma_common.conn_out->transfer_forward_ocon(tp, &jsonLstRet JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
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
#else
			assert(0);
#endif
			return JVX_NO_ERROR;
			break;
		default:
			if (_common_set_ma_common.conn_out)
			{
				return _common_set_ma_common.conn_out->transfer_forward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			}
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxSingleMaster::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_ACTIVE)
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_FAC_MAS(fdb, _common_set_ma_common.myParent);
		if (_common_set_ma_common.conn_out)
		{
			if (_common_set_ma_common.cbRef)
			{
				_common_set_ma_common.cbRef->before_test_chain(this);
			}

			jvxSize cnt = 0;
			while (cnt < _common_set_ma_common.postpone_attempts_max)
			{
				res = _common_set_ma_common.conn_out->test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				if (res != JVX_ERROR_POSTPONE)
				{
					break;
				}
				cnt++;
			}
			if (res == JVX_ERROR_POSTPONE)
			{
				std::cout << __FUNCTION__ << ": WARNING: Test of chain " << _common_set_ma_common.descriptor <<
					" failed due to a number of postpone requests exceeding the maximum value of <" <<
					_common_set_ma_common.postpone_attempts_max << ">." << std::endl;
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
}

jvxErrorType 
CjvxSingleMaster::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_ACTIVE)
	{
		if (_common_set_ma_common.conn_out)
		{
			res = _common_set_ma_common.conn_out->prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			std::cout << "Master does not provide any output connector!" << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			runtime.stat = JVX_STATE_PREPARED;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_PREPARED)
	{
		if (_common_set_ma_common.conn_out)
		{
			res = _common_set_ma_common.conn_out->postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			std::cout << "Master does not provide any output connector!" << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			runtime.stat = JVX_STATE_ACTIVE;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_PREPARED)
	{
		if (_common_set_ma_common.conn_out)
		{
			res = _common_set_ma_common.conn_out->start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			std::cout << "Master does not provide any output connector!" << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			runtime.stat = JVX_STATE_PROCESSING;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	if (runtime.stat == JVX_STATE_PROCESSING)
	{
		if (_common_set_ma_common.conn_out)
		{
			res = _common_set_ma_common.conn_out->stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
		else
		{
			std::cout << "Master does not provide any output connector!" << std::endl;
		}
		if (res == JVX_NO_ERROR)
		{
			runtime.stat = JVX_STATE_PREPARED;
		}
		return res;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxSingleMaster::parent_master_factory(IjvxConnectionMasterFactory** my_parent) 
{
	if (my_parent) *my_parent = _common_set_ma_common.myParent;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::descriptor_master(jvxApiString* name) 
{
	name->assign(_common_set_ma_common.descriptor);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::iterator_chain(IjvxConnectionIterator** it) 
{
	if (it) *it = this;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::connection_association(jvxApiString* astr, jvxComponentIdentification* cpId)
{
	if (astr)
	{
		// astr->assign(_common_set_ld_master.tagAss);
	}
	if (cpId)
	{
		// *cpId = _common_set_ld_master.cpTpAss;
	}

	return JVX_NO_ERROR;
}

// ==========================================================================================================
jvxErrorType CjvxSingleMaster::number_next(jvxSize* num)
{
	if (num) *num = 0;
	if (runtime.stat >= JVX_STATE_ACTIVE)
	{
		if (_common_set_ma_common.conn_out)
		{
			if (num) *num = 1;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSingleMaster::reference_next(jvxSize idx, IjvxConnectionIterator** next)
{
	jvxErrorType res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	if(next) *next = nullptr;

	if (idx == 0)
	{
		res = JVX_ERROR_WRONG_STATE;
		if (runtime.stat >= JVX_STATE_ACTIVE)
		{
			if (_common_set_ma_common.conn_out)
			{
				IjvxInputConnector* icon = nullptr;
				_common_set_ma_common.conn_out->connected_icon(&icon);
				if (icon)
				{
					*next = static_cast<IjvxConnectionIterator*>(icon);
					res = JVX_NO_ERROR;
				}
			}
		}
	}
	return res;
}
	
jvxErrorType
CjvxSingleMaster::reference_component(
	jvxComponentIdentification* cpTp,
	jvxApiString* modName,
	jvxApiString* description,
	jvxApiString* lContext)
{
	if (cpTp) *cpTp = JVX_COMPONENT_UNKNOWN;
	if (_common_set_ma_common.object)
	{
		_common_set_ma_common.object->module_reference(modName, nullptr);
		_common_set_ma_common.object->description(description);
		if(lContext) lContext->assign(_common_set_ma_common.descriptor);
	}
	return JVX_NO_ERROR;
}

