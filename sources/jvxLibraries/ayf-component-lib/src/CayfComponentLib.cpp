#include "ayf-connection-host.h"
#include "CayfComponentLib.h"

/*
extern jvxErrorType register_module_host(const char* nm, jvxApiString& nmAsRegistered, IjvxObject* regMe, IjvxMinHost** hostOnReturn);
extern jvxErrorType unregister_module_host(IjvxObject* regMe);
*/

CayfComponentLib::CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
}

CayfComponentLib::~CayfComponentLib()
{

}

jvxErrorType
CayfComponentLib::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize(hostRef);
	if (res == JVX_NO_ERROR)
	{
		jvxApiString astr;
		ayf_register_module_host(regName.c_str(), astr, this, &this->host, &this->confProcHdl);
		this->hostRef = static_cast<IjvxHiddenInterface*>(this->host);
		_common_set_min.theHostRef = this->hostRef;		

		ayf_load_config_content(this, &cfgDataInit, nullptr);
	}
	return res;
}

jvxErrorType
CayfComponentLib::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;	
	if (_common_set_min.theHostRef)
	{
		if (cfgDataInit)
		{
			ayf_release_config_content(this, cfgDataInit);
		}
		ayf_unregister_module_host(this);
	}
	this->hostRef = nullptr;
	_common_set_min.theHostRef = nullptr;
	CjvxObject::_terminate();
	return res;
}

jvxErrorType
CayfComponentLib::activate()
{
	jvxErrorType resC = JVX_NO_ERROR;
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionProcess* theProc = NULL;
	IjvxProperties* props = nullptr;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	jvxBool errorDetected = false;

	resC = _activate();
	if (resC == JVX_NO_ERROR)
	{
		JVX_PROPERTIES_ALL_START(genComponentLib);

		// Need this link
		_common_set_ldslave.ocon = static_cast<IjvxOutputConnector*>(this);
		_common_set_ldslave.icon = static_cast<IjvxInputConnector*>(this);
		_common_set_ldslave.theMaster = static_cast<IjvxConnectionMaster*>(this);

		resC = allocateMainNode(); 
		if ((resC == JVX_NO_ERROR) && this->mainObj)
		{
			this->mainNode = castFromObject<IjvxNode>(this->mainObj);
			resC = this->mainNode->initialize(this->hostRef);
			if (resC == JVX_NO_ERROR)
			{
				this->mainNode->set_location_info(jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, 0));
				
				resC = this->mainNode->select(nullptr);
				passConfigSection(this->mainNode, regName);

			}
			if (resC == JVX_NO_ERROR)
			{
				// Add the sub components
				props = reqInterfaceObj<IjvxProperties>(this->mainNode);
				if (props)
				{
					IjvxPropertyExtender* pExt = nullptr;
					jvxCallManagerProperties callMan;
					jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> rPtr(&pExt);
					// rPtr.atype(jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL);
					jvxErrorType resL = props->get_property(callMan, rPtr, jPAD("/iface/ex_interface"), jPD(true));
					if (pExt)
					{
						IjvxPropertyExtenderChainControl* propExtSpec = castPropIfExtender< IjvxPropertyExtenderChainControl>(pExt);
						if (propExtSpec)
						{
							propExtSpec->set_stateswitch_node_handler(this);
							attachAllSubModules(propExtSpec);
							
						}
					}

					retInterfaceObj<IjvxProperties>(this->mainNode, props);
				}
			}

			resC = this->mainNode->activate();
			if (resC == JVX_NO_ERROR)
			{
				passConfigSection(this->mainNode, regName);

				// Attach main node to host control
				attach_component_module(mainNodeName, this, this->mainObj);

				// Create simple connection
				theConnections = reqInterface<IjvxDataConnections>(this->host);
				if (theConnections)
				{
					std::string nmprocess = chainName;
					jvxSize cnt = 1;
					jvxBool interceptors = false;
					jvxSize idProcDepends = JVX_SIZE_UNSELECTED;
					jvxBool preventStore = true;
					IjvxConnectorFactory* iFac = nullptr;
					IjvxInputConnector* icon = nullptr;
					IjvxOutputConnector* ocon = nullptr;
					jvxSize uId_bridge_dev_node = JVX_SIZE_UNSELECTED;
					jvxSize uId_bridge_node_dev = JVX_SIZE_UNSELECTED;
					iFac = reqInterfaceObj< IjvxConnectorFactory>(this->mainNode);
					if (iFac)
					{
						jvxSize numIcons = 0;
						jvxSize numOcons = 0;
						iFac->number_input_connectors(&numIcons);
						iFac->number_output_connectors(&numOcons);
						if ((numIcons > 0) && (numOcons > 0))
						{
							iFac->reference_input_connector(0, &icon);
							iFac->reference_output_connector(0, &ocon);
							if (icon && ocon)
							{
								resC = JVX_ERROR_DUPLICATE_ENTRY;
								while (1)
								{

									resC = theConnections->create_connection_process(nmprocess.c_str(), &uId_process, interceptors, false, false, false, idProcDepends);
									if (resC == JVX_NO_ERROR)
									{
										break;
									}
									nmprocess = chainName + "(" + jvx_size2String(cnt) + ")";
									cnt++;

									if (cnt >= 10)
									{
										break;
									}
								}

								if (resC == JVX_NO_ERROR)
								{
									resC = theConnections->reference_connection_process_uid(uId_process, &theProc);
								}

								if (resC == JVX_NO_ERROR)
								{
									resC = theProc->set_connection_context(theConnections);
								}

								if (resC == JVX_NO_ERROR)
								{
									// Do not create an entry in the config file by actively preventing it!
									resC = theProc->set_misc_connection_parameters(JVX_SIZE_UNSELECTED, preventStore);
								}

								if (resC == JVX_NO_ERROR)
								{
									// We need to specify this object as the owener, otherwise, the connection will
									// be removed before the "deactivate" member function is called
									resC = theProc->associate_master(static_cast<IjvxConnectionMaster*>(this), nullptr);
								}

								if (resC == JVX_NO_ERROR)
								{
									resC = theProc->create_bridge(static_cast<IjvxOutputConnector*>(this), icon, "Device to node bridge", &uId_bridge_dev_node);
								}

								if (resC == JVX_NO_ERROR)
								{
									resC = theProc->create_bridge(ocon, static_cast<IjvxInputConnector*>(this), "Node to device bridge", &uId_bridge_node_dev);
								}

								// Prevent that we run the test function immediately after connect
								theProc->set_test_on_connect(false);

								if (resC == JVX_NO_ERROR)
								{
									resC = theProc->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
								}

								theConnections->return_reference_connection_process(theProc);
								theProc = nullptr;
							}
							retInterfaceObj< IjvxConnectorFactory>(this->mainNode, iFac);
							iFac = nullptr;
						}
					}
					retInterface<IjvxDataConnections>(this->host, theConnections);
					theConnections = nullptr;
				}
			} // resC = this->mainNode->activate(); if (resC == JVX_NO_ERROR)
		}
	}
	if (resC != JVX_NO_ERROR)
	{
		this->deactivate();
	}
	return resC;

	/*
exit_error:
	if (JVX_CHECK_SIZE_SELECTED(uId_process))
	{
		theConnections->remove_connection_process(uId_process);
	}
	if (theConnections)
	{
		retInterface<IjvxDataConnections>(this->host, theConnections);
		theConnections = nullptr;
	}
	return JVX_NO_ERROR;
	*/
}

jvxErrorType 
CayfComponentLib::deactivate()
{
	jvxErrorType resC = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionProcess* theProc = NULL;
	IjvxProperties* props = nullptr;
	theConnections = reqInterface<IjvxDataConnections>(this->host);
	resC = _pre_check_deactivate();
	if (resC == JVX_NO_ERROR)
	{
		if (theConnections)
		{
			resC = theConnections->reference_connection_process_uid(uId_process, &theProc);
			assert(resC == JVX_NO_ERROR);

			resC = theProc->disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resC == JVX_NO_ERROR);

			theConnections->return_reference_connection_process(theProc);

			if (JVX_CHECK_SIZE_SELECTED(uId_process))
			{
				resC = theConnections->remove_connection_process(uId_process);
				assert(resC == JVX_NO_ERROR);
			}
			uId_process = JVX_SIZE_UNSELECTED;
			retInterface<IjvxDataConnections>(this->host, theConnections);
		}
		theConnections = nullptr;

		resC = detach_component_module(mainNodeName, this);
		assert(resC == JVX_NO_ERROR);

		resC = this->mainNode->deactivate();
		assert(resC == JVX_NO_ERROR);

		// Add the sub components
		props = reqInterfaceObj<IjvxProperties>(this->mainNode);
		if (props)
		{
			IjvxPropertyExtender* pExt = nullptr;
			jvxCallManagerProperties callMan;
			jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> rPtr(&pExt);
			// rPtr.atype(jvx::propertyRawPointerType::apiType::JVX_RAW_POINTER_API_TYPE_DLL);
			jvxErrorType resL = props->get_property(callMan,
				rPtr, jPAD("/iface/ex_interface"), jPD(true));
			if (pExt)
			{
				IjvxPropertyExtenderChainControl* propExtSpec = castPropIfExtender< IjvxPropertyExtenderChainControl>(pExt);
				if (propExtSpec)
				{
					this->detachAllSubModules(propExtSpec);
					propExtSpec->set_stateswitch_node_handler(nullptr);
				}
			}
			retInterfaceObj<IjvxProperties>(this->mainNode, props);
		}

		resC = this->mainNode->unselect();
		assert(resC == JVX_NO_ERROR);

		resC = this->mainNode->terminate();
		assert(resC == JVX_NO_ERROR);

		deallocateMainNode();
		this->mainObj = nullptr;
		this->mainNode = nullptr;
		
		resC = _deactivate();
		assert(resC == JVX_NO_ERROR);

		JVX_PROPERTIES_ALL_STOP(genComponentLib);
	}
	return resC;
}

jvxErrorType 
CayfComponentLib::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = neg_input._negotiate_connect_icon(_common_set_ldslave.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_ldslave.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
	}
	return res;
}

jvxErrorType
CayfComponentLib::add_text_message_token(const std::string& txtIn)
{
	if (txtMessageStatus == ayfTextIoStatus::AYF_TEXT_IO_STATUS_COLLECTING)
	{
		txtMessageCollect += txtIn;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CayfComponentLib::new_text_message_status(int value, char* fldRespond, jvxSize szRespond, int* newStatOnReturn)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	switch (value)
	{
	case 0:

		// Message reset
		txtMessageCollect.clear();
		txtMessageResponse.clear();
		txtMessageStatus = ayfTextIoStatus::AYF_TEXT_IO_STATUS_COLLECTING;
		res = JVX_NO_ERROR;
		
		break;
	case 1:
		// Process txtMessageCollect
		if (txtMessageStatus == ayfTextIoStatus::AYF_TEXT_IO_STATUS_COLLECTING)
		{
			// txtMessageResponse = txtMessageCollect;
			jvxApiString astr;
			ayf_forward_text_command(txtMessageCollect.c_str(), this, astr);
			txtMessageResponse = astr.std_str();
			txtMessageStatus = ayfTextIoStatus::AYF_TEXT_IO_STATUS_RESPONDING;
			res = JVX_NO_ERROR;
		}
		break;
	case 2: 
		if (txtMessageStatus == ayfTextIoStatus::AYF_TEXT_IO_STATUS_RESPONDING)
		{
			jvxSize copyNum = JVX_MIN(txtMessageResponse.size(), szRespond-1);
			memset(fldRespond, 0, szRespond);
			memcpy(fldRespond, txtMessageResponse.c_str(), copyNum);
			txtMessageResponse = txtMessageResponse.substr(copyNum);

			// Here we can copy the chunks
			if (txtMessageResponse.empty())
			{
				txtMessageStatus = ayfTextIoStatus::AYF_TEXT_IO_STATUS_DONE;
				txtMessageCollect.clear();
			}
			res = JVX_NO_ERROR;
		}
		break;
	}
	if (newStatOnReturn) *newStatOnReturn = (int)txtMessageStatus;
	return res;
}

jvxErrorType 
CayfComponentLib::attach_component_module(const std::string& nm, IjvxObject* priObj, IjvxObject* attachMe)
{
	return ayf_attach_component_module(nm.c_str(), priObj, attachMe);
}

jvxErrorType 
CayfComponentLib::detach_component_module(const std::string& nm, IjvxObject* priObj)
{
	return ayf_detach_component_module(nm.c_str(), priObj);
}

jvxErrorType
CayfComponentLib::runStateSwitch(jvxStateSwitch ss, IjvxNode* node, const char* moduleName, IjvxObject* theOwner)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (ss)
	{
	case JVX_STATE_SWITCH_SELECT:
		res = node->select(theOwner);
		passConfigSection(node, moduleName);
		break;
	case JVX_STATE_SWITCH_ACTIVATE:
		res = node->activate();
		passConfigSection(node, moduleName);
		break;
	case JVX_STATE_SWITCH_DEACTIVATE:
		res = node->deactivate();
		break;
	case JVX_STATE_SWITCH_UNSELECT:
		res = node->unselect();
		break;
	default:
		assert(0);
	}
	return res;
}

jvxErrorType
CayfComponentLib::componentsAboutToConnect()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLib::passConfigSection(IjvxNode* node, const std::string& moduleName)
{
	jvxConfigData* datSec = nullptr;
	if (confProcHdl && cfgDataInit)
	{
		confProcHdl->getReferenceEntryCurrentSection_name(cfgDataInit, &datSec, moduleName.c_str());
		if (datSec)
		{
			IjvxConfiguration* cfg = reqInterfaceObj<IjvxConfiguration>(node);
			if (cfg)
			{
				jvxCallManagerConfiguration callMan;
				cfg->put_configuration(&callMan, confProcHdl, datSec);
				retInterfaceObj<IjvxConfiguration>(node,cfg);
			}
		}
	}
	return JVX_NO_ERROR;
}

// Callback to report that the system is ready now
jvxErrorType 
CayfComponentLib::system_ready()  
{
	return JVX_NO_ERROR;
}

// Callback to report that the system is about to shutdown
jvxErrorType 
CayfComponentLib::system_about_to_shutdown() 
{
	return JVX_NO_ERROR;
}

