virtual jvxErrorType JVX_CALLINGCONVENTION name_master(jvxApiString* str)override
{
	return _name_master(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION data_delivery_type(jvxCBitField* deltp)override
{
	return _data_delivery_type(deltp);
}

virtual jvxErrorType JVX_CALLINGCONVENTION select_master(IjvxDataConnectionProcess* ref)override
{
	return _select_master(ref);
}

virtual jvxErrorType JVX_CALLINGCONVENTION unselect_master()override
{
	return _unselect_master();
}

virtual jvxErrorType JVX_CALLINGCONVENTION available_master(jvxBool* avail)override
{
	return _available_master(avail);
}

virtual jvxErrorType JVX_CALLINGCONVENTION associated_process(IjvxDataConnectionProcess **ref)override
{
	return _associated_process(ref);
}

virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_context(IjvxDataConnectionCommon* ctxt)override
{
	return _set_connection_context(ctxt);
}

virtual jvxErrorType JVX_CALLINGCONVENTION connection_context(IjvxDataConnectionCommon** ctxt)override
{
	return _connection_context(ctxt);
}

#ifndef JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR
#define JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR static_cast<IjvxOutputConnector*>(this)
#define UNDEFINE_JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_CONNECT
virtual jvxErrorType JVX_CALLINGCONVENTION connect_chain_master(const jvxChainConnectArguments& args,
	const jvxLinkDataDescriptor_con_params* init_params JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _activate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
	assert(res == JVX_NO_ERROR);

	res = _connect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		jvxErrorType resL = _deactivate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
		assert(resL == JVX_NO_ERROR);
	}
	return res;
}
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_DISCONNECT
virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_chain_master(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _disconnect_chain_master(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	res = _deactivate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
	return res;
}
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION inform_changed_master(jvxMasterChangedEventType tp JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_MASTER_UPDATE_EVENT_STARTED:
		_activate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
		res = _inform_changed_master(tp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;
	case JVX_MASTER_UPDATE_EVENT_ABOUT_TO_STOP:
		res = _inform_changed_master(tp JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		_deactivate_master(JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR, static_cast<IjvxConnectionMaster*>(this));
		break;
	}
	return res;
}

#ifdef UNDEFINE_JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR
#undef JVX_CONNECTION_MASTER_OUTPUT_CONNECTOR
#endif

#ifndef JVX_SUPPRESS_AUTO_READY_CHECK_MASTER
jvxErrorType transfer_chain_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{

	jvxErrorType res = JVX_NO_ERROR;
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		jvxBool is_ready = true;
		jvxApiString* astr = (jvxApiString*)data;
		res = this->is_ready(&is_ready, astr);
		if (is_ready == false)
		{
			res = JVX_ERROR_NOT_READY;
		}
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return _transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif

jvxErrorType transfer_chain_backward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
{
	return _transfer_chain_backward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

#ifndef JVX_CONNECTION_MASTER_SKIP_TEST
jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
{
	return _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_PREPARE
virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_POSTPROCESS
virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_START
virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
#endif

#ifndef JVX_CONNECTION_MASTER_SKIP_STOP
virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
{
	return _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION parent_master_factory(IjvxConnectionMasterFactory** my_parent)override
{
	return _parent_master_factory(my_parent);
}

virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_master(jvxApiString* str) override
{
	return _descriptor_master(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION iterator_chain(IjvxConnectionIterator** it) override
{
	return _iterator_chain(it);
}

virtual jvxErrorType JVX_CALLINGCONVENTION connection_association(jvxApiString* astr, jvxComponentIdentification* cpId) override
{
	return _connection_association(astr, cpId);
}
