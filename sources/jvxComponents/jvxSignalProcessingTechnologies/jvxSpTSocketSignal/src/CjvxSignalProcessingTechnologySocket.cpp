#include "CjvxSignalProcessingTechnologySocket.h"

CjvxSignalProcessingTechnologySocket::CjvxSignalProcessingTechnologySocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxTemplateTechnologyVD(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	this->numberDevicesInit = 0;
	this->deviceNamePrefix = "SockDev";
	JVX_INITIALIZE_MUTEX(safeAccessConnections);
}


CjvxSignalProcessingTechnologySocket::~CjvxSignalProcessingTechnologySocket()
{
	JVX_TERMINATE_MUTEX(safeAccessConnections);
}

jvxErrorType 
CjvxSignalProcessingTechnologySocket::activate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = _activate();
	if (res == JVX_NO_ERROR)
	{		
		std::string deskPath = JVX_GET_USER_DESKTOP_PATH();
		std::string socketPath = deskPath + JVX_SEPARATOR_DIR + "jvxrt.sock.server";

		std::cout << __FUNCTION__ << ": Starting to listen for connections on UNIX socket at <" << socketPath << ">." << std::endl;

		// socketPath = " jvxrt.sock.server";
		theDeviceSocket.start(static_cast<IjvxSocketsServer_report*>(this),
			true, socketPath.c_str());

#if 0
		// Do whatever is required
		T* newDevice = new T(deviceNamePrefix.c_str(), false,
			_common_set.theDescriptor.c_str(),
			_common_set.theFeatureClass,
			_common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			(jvxComponentType)(_common_set.theComponentType.tp + 1),
			"", NULL);

		// Whatever to be done for initialization
		oneDeviceWrapper elm;
		elm.hdlDev = static_cast<IjvxDevice*>(newDevice);

		_common_tech_set.lstDevices.push_back(elm);
#endif
	}
	return(res);
};

jvxErrorType 
CjvxSignalProcessingTechnologySocket::deactivate()
{
	// "Jump" over the base class implementation 
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		// Accept no more incoming
		theDeviceSocket.stop();
		//theDeviceSocket.lock_incoming(true);
		
		assert(pendig_activate.size() == 0);
		/*
		JVX_LOCK_MUTEX(safeAccessConnections);
		auto elm = pendig_activate.begin();
		for (; elm != pendig_activate.end(); elm++)
		{
			if (!elm->second->shutdowTriggered)
			{
				elm->second->connection->disconnect();
			}
		}
		JVX_UNLOCK_MUTEX(safeAccessConnections);

		while (1)
		{
			if (pendig_activate.size() == 0)
			{
				break;
			}
			else
			{
				JVX_SLEEP_MS(100);
			}
		}
		*/

		auto elmD = _common_tech_set.lstDevices.begin();
		for (; elmD != _common_tech_set.lstDevices.end(); elmD++)
		{
			delete elmD->hdlDev;
		}
		_common_tech_set.lstDevices.clear();
		assert(lookupDevice.size() == 0);

		_deactivate();
	}
	return res;
}

jvxErrorType
CjvxSignalProcessingTechnologySocket::return_device(IjvxDevice* dev)
{
	jvxErrorType res = CjvxTemplateTechnology<CjvxSignalProcessingDeviceSocket>::return_device(dev);
	if(res == JVX_NO_ERROR)
	{
		jvxBool stillWaiting = true;
		while (stillWaiting)
		{
			jvxBool stillAlive = false;

			JVX_LOCK_MUTEX(safeAccessConnections);
			auto elm = pendig_activate.begin();
			for (; elm != pendig_activate.end(); elm++)
			{
				if (static_cast<IjvxDevice*>(elm->second) == dev)
				{
					stillAlive = true;
					break;
				}
			}
			JVX_UNLOCK_MUTEX(safeAccessConnections);
			if (stillAlive)
			{
				elm->second->connection->disconnect();
				JVX_SLEEP_MS(200);
			}
			else
			{
				stillWaiting = false;
			}
		}
		
		auto elm = _common_tech_set.lstDevices.begin();
		for (; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			if (static_cast<IjvxDevice*>(elm->hdlDev) == dev)
			{
				break;
			}
		}
		if (elm != _common_tech_set.lstDevices.end())
		{
			delete elm->hdlDev;
			_common_tech_set.lstDevices.erase(elm);
		}
	}
	return res;
};
// ==============================================================================================
jvxErrorType 
CjvxSignalProcessingTechnologySocket::report_server_error(jvxSocketsErrorType err, const char* description)
{
	std::cout << __FUNCTION__ << " -> " << description << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingTechnologySocket::report_server_denied(const char* description)
{
	std::cout << __FUNCTION__ << " -> " << description << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingTechnologySocket::report_server_connect(const char* description, IjvxSocketsConnection* newConnection)
{
	jvxCBitField prio = 0;
	std::string nm = deviceNamePrefix + "#" + jvx_size2String(uId);
	uId++;
	CjvxSignalProcessingDeviceSocket* newDevice =
		new CjvxSignalProcessingDeviceSocket(
			nm.c_str(), false,
			description,
			_common_set.theFeatureClass,
			_common_set.theModuleName.c_str(),
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			(jvxComponentType)(_common_set.theComponentType.tp + 1),
			"", NULL);
	newDevice->init_connection(newConnection, this);

	JVX_LOCK_MUTEX(safeAccessConnections);
	pendig_activate[newConnection] = newDevice;
	JVX_UNLOCK_MUTEX(safeAccessConnections);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingTechnologySocket::report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection)
{
	std::string ident;
	JVX_LOCK_MUTEX(safeAccessConnections);
	auto elm = pendig_activate.find(newConnection);
	if (elm != pendig_activate.end())
	{
		assert(elm->second->connection);
		ident = elm->second->_common_set_min.theDescription;
		
		// Here, we unlink all connection referfences.
		// The connection, however, lives a little longer.
		// It should disappear shortly after this position. In the worst case,
		// the connection thread keeps on running and will be waited for
		// once the server is shutdown. In this case, this is once 
		// the technology is deactivated
		elm->second->terminate_connection();		
		pendig_activate.erase(elm);
	}
	JVX_UNLOCK_MUTEX(safeAccessConnections);

	/*
	CjvxReportCommandRequest_rm theRequest(_common_set.theComponentType, JVX_SCHEDULE_ID_CONNECTION_DROPPED);
	_request_command(static_cast<IjvxReportCommandRequest*>(&theRequest));
	*/

	report_device_died(ident);

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingTechnologySocket::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	if (tp == JVX_INTERFACE_SCHEDULE)
	{
		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxSchedule*>(this));
		return JVX_NO_ERROR;
	}
	return CjvxTemplateTechnology<CjvxSignalProcessingDeviceSocket>::request_hidden_interface(tp, hdl);
};

jvxErrorType 
CjvxSignalProcessingTechnologySocket::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	if (tp == JVX_INTERFACE_SCHEDULE)
	{
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxSchedule*>(this)))
		{
			return JVX_NO_ERROR;
		}
		else
		{
			return JVX_ERROR_INVALID_ARGUMENT;
		}
	}

	return CjvxTemplateTechnology<CjvxSignalProcessingDeviceSocket>::return_hidden_interface(tp, hdl);
};

jvxErrorType 
CjvxSignalProcessingTechnologySocket::schedule_main_loop(jvxSize rescheduleId, jvxHandle* user_data)
{
	std::string ident;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	switch (rescheduleId)
	{
	case JVX_SCHEDULE_ID_NEW_CONNECTION:
	{
		// New Connection established
		JVX_LOCK_MUTEX(safeAccessConnections);
		auto elm = pendig_activate.begin();
		for (; elm != pendig_activate.end();elm++)
		{
			if (elm->second->theSMachine.localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT)
			{
				jvxBool nmSet = false;
				jvxSize cnt = 0;
				std::string uniqueName;
				jvxAudioSocketParameters params;
				elm->second->data_slave(&params);
				uniqueName = params.nameConnection;
				while (1)
				{
					uniqueName = params.nameConnection;
					if (cnt != 0)
					{
						uniqueName += "<" + jvx_size2String(cnt) + ">";					
					}
					cnt++;
					jvxBool foundIt = false;
					auto elmD = _common_tech_set.lstDevices.begin();
					for (;elmD != _common_tech_set.lstDevices.end(); elmD++)
					{
						jvxApiString astr;
						elmD->hdlDev->description(&astr);
						if(astr.std_str() == uniqueName)
						{
							foundIt = true;
							break;
						}
					}
					if (!foundIt)
					{
						params.nameConnection = uniqueName;
						break;
					}
				}

				res = elm->second->accept_slave(&params);
				assert(res == JVX_NO_ERROR); // <- this must be observed :-)

				oneDeviceWrapper elmAdd;
				elmAdd.hdlDev = static_cast<IjvxDevice*>(elm->second);
				ident = elm->second->_common_set_min.theDescription;
				_common_tech_set.lstDevices.push_back(elmAdd);
			}
		}
		JVX_UNLOCK_MUTEX(safeAccessConnections);

		report_device_was_born(ident);

		res = JVX_NO_ERROR;
	}
	break;	
	}
	return res;
}

jvxErrorType
CjvxSignalProcessingTechnologySocket::report_new_connection(IjvxSocketsConnection* connection)
{
	// Reschedule this information into the main thread
	CjvxReportCommandRequest_rm theRequest(_common_set.theComponentType, JVX_SCHEDULE_ID_NEW_CONNECTION, 
		jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
		connection);
	_request_command(theRequest);
	return JVX_NO_ERROR;
}

/*
jvxErrorType
CjvxSignalProcessingTechnologySocket::report_client_error(jvxSocketErrorType err, const char* description)
{
	std::cout << __FUNCTION__ << " -> " << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSignalProcessingTechnologySocket::report_client_connect(IjvxSocketConnection* connection)
{
	std::cout << __FUNCTION__ << " -> " << std::endl;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSignalProcessingTechnologySocket::report_client_disconnect(IjvxSocketConnection* connection)
{
	std::cout << __FUNCTION__ << " -> " << std::endl;
	return JVX_NO_ERROR;
}
*/