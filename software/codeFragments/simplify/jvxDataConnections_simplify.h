	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionProcess* ref, jvxSize* uid) override
	{
		return this->_uid_for_reference(ref, uid);
	}; 

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_connection() override
	{
		return this->_remove_all_connection();
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unique_descriptor(jvxApiString* str) override
	{
		return this->_unique_descriptor(str);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connections_process(jvxSize* num) override
	{
		return this->_number_connections_process(num);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_process(jvxSize idx, IjvxDataConnectionProcess** connected) override
	{
		return this->_reference_connection_process(idx, connected);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_process_uid(jvxSize uid, IjvxDataConnectionProcess** connected) override
	{
		return this->_reference_connection_process_uid(uid, connected);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_process(IjvxDataConnectionProcess* connected) override
	{
		return this->_return_reference_connection_process(connected);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_process(
		const char* nm, 
		jvxSize* unique_id, 
		jvxBool interceptors, 
		jvxBool essential_for_start,
		jvxBool verbose_out,
		jvxBool report_global,
		jvxSize idProcDepends) override
	{
		return this->_create_connection_process( nm, unique_id, interceptors, essential_for_start, verbose_out, report_global, idProcDepends);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_process(jvxSize unique_id) override
	{
		return this->_remove_connection_process(unique_id);
	};

	// ==========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionGroup* ref, jvxSize* uid) override
	{
		return this->_uid_for_reference(ref, uid);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connections_group(jvxSize* num) override
	{
		return this->_number_connections_group(num);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_group(jvxSize idx, IjvxDataConnectionGroup** connected) override
	{
		return this->_reference_connection_group(idx, connected);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_group_uid(jvxSize uid, IjvxDataConnectionGroup** connected) override
	{
		return this->_reference_connection_group_uid(uid, connected);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_group(IjvxDataConnectionGroup* connected) override
	{
		return  _return_reference_connection_group(connected);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_group(
		const char* nm, 
		jvxSize* unique_id, 
		const char* descr, 
		jvxBool verbose_out) override
	{
		return this->_create_connection_group( nm,  unique_id, descr, verbose_out);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_group(jvxSize unique_id) override
	{
		return this->_remove_connection_group(unique_id);
	};

	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION register_connection_factory(IjvxConnectorFactory* theFac) override
	{
		return this->_register_connection_factory(  theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_connection_factory(
		IjvxConnectorFactory* theFac) override
	{
		return this->_unregister_connection_factory( theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_factories(jvxSize* num) override
	{
		return this->_number_connection_factories( num);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_factory(jvxSize idx, IjvxConnectorFactory** theFac, jvxSize* unique_id) override
	{
		return this->_reference_connection_factory( idx, theFac, unique_id);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_factory_uid(jvxSize unique_id, IjvxConnectorFactory** theFac) override
	{
		return this->_reference_connection_factory_uid(unique_id, theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_factory(IjvxConnectorFactory* theFac) override
	{
		return this->_return_reference_connection_factory(theFac);
	};

	// ============================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION register_connection_master_factory(IjvxConnectionMasterFactory* theFac) override
	{
		return this->_register_connection_master_factory(theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_connection_master_factory(
		IjvxConnectionMasterFactory* theFac) override
	{
		return this->_unregister_connection_master_factory(theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_master_factories(jvxSize* num) override
	{
		return this->_number_connection_master_factories(num);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_master_factory(jvxSize idx, IjvxConnectionMasterFactory** theFac, jvxSize* unique_id) override
	{
		return this->_reference_connection_master_factory(idx, theFac, unique_id);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_master_factory_uid(jvxSize unique_id, IjvxConnectionMasterFactory** theFac ) override
	{
		return this->_reference_connection_master_factory_uid(unique_id, theFac);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_master_factory(IjvxConnectionMasterFactory* theFac) override
	{
		return this->_return_reference_connection_master_factory(theFac);
	};
	
	// ==========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_connection_rules(jvxSize* num)override
	{
		return this->_number_connection_rules(num);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_rule(jvxSize idx, IjvxDataConnectionRule** rule_on_return)override
	{
		return this->_reference_connection_rule(idx, rule_on_return);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connection_rule_uid(jvxSize uId, IjvxDataConnectionRule** rule_on_return)override
	{
		return this->_reference_connection_rule_uid(uId, rule_on_return);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionRule* ref, jvxSize* uid) override
	{
		return this->_uid_for_reference(ref, uid);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connection_rule(IjvxDataConnectionRule* rule_to_return)override
	{
		return this->_return_reference_connection_rule(rule_to_return);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION create_connection_rule(const char* rule_name, jvxSize* uId, jvxDataConnectionRuleParameters* params, jvxSize catId) override
	{
		return this->_create_connection_rule(rule_name, uId, params, catId);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection_rule(jvxSize uId) override
	{
		return this->_remove_connection_rule(uId);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_connection_rules() override
	{
		return this->_remove_all_connection_rules();
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_process_test(jvxSize uIdProcess, jvxSize* numTested, jvxBool immediatetest) override
	{
		return this->_add_process_test(uIdProcess, numTested, immediatetest);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION trigger_process_test_all(jvxSize* numTested) override
	{
		return this->_trigger_process_test_all(numTested);
	}

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION set_reference_report(IjvxDataConnection_report* hdlArg) override
	{
		return this->_set_reference_report(hdlArg);
	}
	*/

	virtual jvxErrorType JVX_CALLINGCONVENTION uid_for_reference(IjvxDataConnectionCommon* ref, jvxSize* uid, jvxBool* isProcess) override
	{
		return this->_uid_for_reference(ref,  uid, isProcess);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start(jvxApiString* reason_if_not) override
	{
		return this->_ready_for_start(reason_if_not);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION ready_for_start(jvxSize uid, jvxApiString* reason_if_not)override
	{
		return this->_ready_for_start(uid, reason_if_not);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION set_ready_handler(IjvxDataConnections_checkready* ptr) override
	{
		return this->_set_ready_handler(ptr);
	}
