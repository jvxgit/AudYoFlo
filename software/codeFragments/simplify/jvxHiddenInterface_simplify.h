	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;

		switch(tp)
		{
#ifdef JVX_INTERFACE_SUPPORT_PROPERTIES
		case JVX_INTERFACE_PROPERTIES:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
			break;

#endif

#ifdef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
		case JVX_INTERFACE_SEQUENCERCONTROL:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxSequencerControl*>(this));
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_CONFIGURATION
		case JVX_INTERFACE_CONFIGURATION:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
		case JVX_INTERFACE_CONFIGURATION_DONE:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationDone*>(this));
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
		case JVX_INTERFACE_CONNECTOR_FACTORY:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(this));
			break;
#endif
#ifdef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
		case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(this));
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_SCHEDULE
		case JVX_INTERFACE_SCHEDULE:
			*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxSchedule*>(this));
			break;
#endif

		default:
#ifdef JVX_INTERFACE_SUPPORT_BASE_CLASS
			res = JVX_INTERFACE_SUPPORT_BASE_CLASS::request_hidden_interface(tp, hdl);
#else
			res = _request_hidden_interface(tp, hdl);
#endif
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override
	{
		jvxErrorType res = JVX_NO_ERROR;

		switch(tp)
		{
#ifdef JVX_INTERFACE_SUPPORT_PROPERTIES
		case JVX_INTERFACE_PROPERTIES:
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
		case JVX_INTERFACE_SEQUENCERCONTROL:
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxSequencerControl*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;

#endif

#ifdef JVX_INTERFACE_SUPPORT_CONFIGURATION
		case JVX_INTERFACE_CONFIGURATION:
			if(hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_CONFIGURATION_DONE
		case JVX_INTERFACE_CONFIGURATION_DONE:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfigurationDone*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
		case JVX_INTERFACE_CONNECTOR_FACTORY:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectorFactory*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
#endif
#ifdef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
		case JVX_INTERFACE_CONNECTOR_MASTER_FACTORY:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConnectionMasterFactory*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;
#endif

#ifdef JVX_INTERFACE_SUPPORT_SCHEDULE
		case JVX_INTERFACE_SCHEDULE:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxSchedule*>(this)))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
		}
			break;
#endif

		default:
#ifdef JVX_INTERFACE_SUPPORT_BASE_CLASS
			res = JVX_INTERFACE_SUPPORT_BASE_CLASS::return_hidden_interface(tp, hdl);
#else
			res = _return_hidden_interface(tp, hdl);
#endif
		}
		return(res);
	}
	
	jvxErrorType
	object_hidden_interface(IjvxObject** objRef) override
	{
		if (objRef)
		{
			*objRef = static_cast<IjvxObject*>(this);
		}
		return JVX_NO_ERROR;
	}
