	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_component(IjvxObject* theObj, IjvxGlobalInstance* theGlob, const char* locationDescription, 
		jvxBool allowMultipleInstance, jvxInitObject_tp funcInit, jvxTerminateObject_tp funcTerm) override
	{
		return this->_add_external_component(static_cast<CjvxObject*>(this), theObj, theGlob, locationDescription, allowMultipleInstance, funcInit, funcTerm);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_component(IjvxObject* theObj) override
	{
		return this->_remove_external_component(static_cast<CjvxObject*>(this), theObj);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_external_report_target(IjvxReport* hdl) override
	{
		return this->_set_external_report_target(static_cast<CjvxObject*>(this), hdl);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_external_report_on_config(IjvxReportOnConfig* callbackStruct) override
	{
		return this->_set_external_report_on_config(static_cast<CjvxObject*>(this), callbackStruct);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_report_state_switch(IjvxReportStateSwitch* callbackStruct, const char* tag) override
	{
		return this->_add_external_report_state_switch(callbackStruct, tag);
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_report_state_switch(IjvxReportStateSwitch* callbackStruct) override
	{
		return this->_remove_external_report_state_switch(callbackStruct);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_external_interface(jvxHandle* theHdl, jvxInterfaceType theIFacetype) override
	{
		return this->_add_external_interface(static_cast<CjvxObject*>(this), theHdl, theIFacetype);
	};
	 
	virtual jvxErrorType JVX_CALLINGCONVENTION remove_external_interface(jvxHandle* theHdl, jvxInterfaceType theIFacetype) override
	{
		return this->_remove_external_interface(static_cast<CjvxObject*>(this), theHdl, theIFacetype);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION add_component_load_blacklist(jvxComponentType theTp, jvxBool targetBlacklist) override
	{
		return this->_add_component_load_blacklist(static_cast<CjvxObject*>(this), theTp, targetBlacklist);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_component_load_blacklist(jvxComponentType theTp, jvxBool targetBlacklist) override
	{
		return this->_remove_component_load_blacklist(static_cast<CjvxObject*>(this), theTp, targetBlacklist);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_component_load_filter_function(jvxLoadModuleFilterCallback regme, jvxHandle* priv) override
	{
		return this->_set_component_load_filter_function(regme, priv);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION store_config(const char* token, const char* cfgToken, jvxBool overwrite_old = true) override
	{
		return this->_store_config(token, cfgToken, overwrite_old);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION copy_config(const char* token, jvxApiString* strReturn) override
	{
		return this->_copy_config(token, strReturn);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION clear_config(const char* token) override
	{
		return this->_clear_config(token);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION  request_id_main_thread(JVX_THREAD_ID* thread_id)override
	{
		return this->_request_id_main_thread(thread_id);
	}

	jvxErrorType add_external_factory(IjvxExternalModuleFactory* oneModFactory)override
	{
		return this->_add_external_factory(oneModFactory);
	}
	
	jvxErrorType remove_external_factory(IjvxExternalModuleFactory* oneModFactory)override
	{
		return this->_remove_external_factory(oneModFactory);
	}
		
	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION report_boot_complete(jvxBool isComplete) override
	{
		return this->_report_boot_complete(isComplete);
	}
	*/