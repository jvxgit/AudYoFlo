	virtual jvxErrorType JVX_CALLINGCONVENTION number_can_play_component_role(jvxSize* num) override
	{
		return(_number_can_play_component_role(num));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION can_play_component_role(jvxComponentType* iActAsComponent, jvxSize idExposure) override
	{
		return(_can_play_component_role(iActAsComponent, idExposure));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_ports_sender_to_receiver(jvxSize* szOnReturn) override
	{
		return(_number_ports_sender_to_receiver(szOnReturn));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION port_setting_sender_to_receiver(jvxSize id_connection, jvxPortConnectionState* portState, jvxString* port_decription, jvxString* connection_description, jvxBool doGet) override
	{
		return(_port_setting_sender_to_receiver(id_connection, portState, port_decription, connection_description, doGet));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION number_ports_receiver_to_sender(jvxSize* szOnReturn) override
	{
		return(_number_ports_receiver_to_sender(jvxSize* szOnReturn));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION port_setting_receiver_to_sender(jvxSize id_connection, jvxPortConnectionState* portState, jvxString* port_decription, jvxString* connection_description, jvxBool doGet) override
	{
		return(_port_setting_receiver_to_sender(id_connection, portState, port_decription, connection_description, doGet));
	};