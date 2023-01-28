virtual jvxErrorType JVX_CALLINGCONVENTION unique_id_connections(jvxSize* uid) override
{
	return _unique_id_connections(uid);
}

virtual jvxErrorType JVX_CALLINGCONVENTION interceptors_active(jvxBool* interc_active) override
{
	return _interceptors_active(interc_active);
}

virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connection(jvxApiString* str) override
{
	return _descriptor_connection(str);
}

virtual jvxErrorType JVX_CALLINGCONVENTION misc_connection_parameters(jvxSize* connRuleId, jvxBool* preventStoredInConfig) override
{
	return _misc_connection_parameters(connRuleId, preventStoredInConfig);
}

virtual jvxErrorType JVX_CALLINGCONVENTION set_misc_connection_parameters(jvxSize connRuleIdArg, jvxBool preventStoredInConfigArg) override
{
	return _set_misc_connection_parameters(connRuleIdArg, preventStoredInConfigArg);
}

virtual jvxErrorType JVX_CALLINGCONVENTION number_bridges(jvxSize* num) override
{
	return _number_bridges(num);
}

#ifndef JVX_DATACONNECTION_SUPPRESS_CONNECTOR_FACTORY_INVOLVED
virtual jvxErrorType connector_factory_is_involved(IjvxConnectorFactory* rem_this)override
{
	return _connector_factory_is_involved(rem_this);
}
#endif

virtual jvxErrorType JVX_CALLINGCONVENTION reference_bridge(jvxSize idx, IjvxConnectorBridge** theBridge) override
{
	return _reference_bridge(idx, theBridge);
}
virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_bridge(IjvxConnectorBridge* theBridge) override
{
	return _return_reference_bridge(theBridge);
}

virtual jvxErrorType JVX_CALLINGCONVENTION remove_bridge(jvxSize unique_id) override
{
	return remove_bridge_local(unique_id);
}
virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_bridges() override
{
	return remove_all_bridges_local();
}
virtual jvxErrorType JVX_CALLINGCONVENTION remove_connection()override
{
	return _remove_connection();
}
/*
virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section, jvxFlagTag flagtag)
{
	return _get_configuration(theWriter, add_to_this_section, flagtag);
}
*/
virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_context(IjvxDataConnections* context) override
{
	return _set_connection_context(context);
}
virtual jvxErrorType JVX_CALLINGCONVENTION connection_context(IjvxDataConnections** context) override
{
	return _connection_context(context);
}

virtual jvxErrorType JVX_CALLINGCONVENTION set_connection_association(const char* tag, jvxComponentIdentification cpTp) override
{
	return _set_connection_association(tag, cpTp);
}

virtual jvxErrorType JVX_CALLINGCONVENTION connection_association(jvxApiString* tagOnRet, jvxComponentIdentification* cpTp) override
{
	return _connection_association(tagOnRet, cpTp);
}

virtual jvxErrorType JVX_CALLINGCONVENTION status(jvxState* stat) override
{
	return _status(stat);
}

virtual jvxErrorType JVX_CALLINGCONVENTION add_dependency(IjvxDataConnectionCommon* depedent) override
{
	return _add_dependency( depedent);
}

virtual jvxErrorType JVX_CALLINGCONVENTION remove_dependency(IjvxDataConnectionCommon* depedent) override
{
	return _remove_dependency(depedent);
}