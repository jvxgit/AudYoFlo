#include "CayfComponentLib.h"
#include "common/CjvxConnectorFactory.h"

#include "ayf-embedding-proxy-entries.h"

#include "jvxHosts/CjvxDataConnections.h"

#define Q(x) #x
#define Quotes(x) Q(x)
#define SET_DLL_REFERENCE(ret, entry) ret. ## entry ## _call = (entry) JVX_GETPROCADDRESS(proxyLibHandle, Quotes(entry))

// Create a local data connections object if required
class myLocalHost : public IjvxDataConnections, public CjvxDataConnections, public IjvxHiddenInterface
{
public:

	jvxErrorType request_hidden_interface(jvxInterfaceType ifTp, jvxHandle** retPtr) override
	{
		switch (ifTp)
		{
		case JVX_INTERFACE_DATA_CONNECTIONS:
			if (retPtr)
			{
				*retPtr = reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this));
			}
			return JVX_NO_ERROR;
			break;
		default:
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType return_hidden_interface(jvxInterfaceType ifTp, jvxHandle* ptrRet)
	{
		switch (ifTp)
		{
		case JVX_INTERFACE_DATA_CONNECTIONS:
			if (ptrRet == reinterpret_cast<jvxHandle*>(static_cast<IjvxDataConnections*>(this)))
			{
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_INVALID_ARGUMENT;
			break;
		default:
			break;
		}
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType object_hidden_interface(IjvxObject** objRef)
	{
		if (objRef) *objRef = nullptr;
		return JVX_NO_ERROR;
	}

#include "codeFragments/simplify/jvxDataConnections_simplify.h"
};

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
CayfComponentLib::populateBindingRefs()
{
	std::string fNameDll = "ayf-embedding-proxy_import.dll";
	proxyLibHandle = JVX_LOADLIBRARY(fNameDll.c_str());
	if (proxyLibHandle != JVX_HMODULE_INVALID)
	{
		SET_DLL_REFERENCE(proxyReferences, ayf_embedding_proxy_init);
		SET_DLL_REFERENCE(proxyReferences, ayf_embedding_proxy_terminate);
	}

	binding.bindType = ayfHostBindingType::AYF_HOST_BINDING_NONE;
	if (proxyReferences.ayf_embedding_proxy_init_call)
	{
		proxyReferences.ayf_embedding_proxy_init_call("MyRegisteredName", &idRegister, &binding, rootPath.c_str());
	}
	
	// Currently just alocal assignemnt
	/*
	binding.bindType = ayfHostBindingType::AYF_HOST_BINDING_MIN;
	binding.ayf_register_module_host_call = ayf_register_module_host;
	binding.ayf_unregister_module_host_call = ayf_unregister_module_host;
	binding.ayf_load_config_content_call = ayf_load_config_content;
	binding.ayf_release_config_content_call = ayf_release_config_content;
	binding.ayf_attach_component_module_call = ayf_attach_component_module;
	binding.ayf_detach_component_module_call = ayf_detach_component_module;
	binding.ayf_forward_text_command_call = ayf_forward_text_command;
	*/
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLib::unpopulateBindingRefs()
{
	if(proxyReferences.ayf_embedding_proxy_terminate_call)
	{
		proxyReferences.ayf_embedding_proxy_terminate_call(idRegister);
		JVX_UNLOADLIBRARY(proxyLibHandle);
		proxyLibHandle = JVX_HMODULE_INVALID;
	}

	return JVX_NO_ERROR;
}

void
CayfComponentLib::setExecutableRootPath(const std::string& rootPathArg)
{
	rootPath = rootPathArg;
}

jvxErrorType
CayfComponentLib::initialize(IjvxHiddenInterface* hostRef)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize(hostRef);
	if (res == JVX_NO_ERROR)
	{
		populateBindingRefs();

		jvxApiString astr;
		if (binding.ayf_register_module_host_call)
		{
			binding.ayf_register_module_host_call(regName.c_str(), astr, this, &this->minHostRef, &this->confProcHdl);
		}
		this->hostRef = static_cast<IjvxHiddenInterface*>(this->minHostRef);

		// If there is no host embedding available, open this tiny local host implementation
		if (this->hostRef == nullptr)
		{
			JVX_DSP_SAFE_ALLOCATE_OBJECT(locHost, myLocalHost);
			this->hostRef = locHost;
		}

		_common_set_min.theHostRef = this->hostRef;		

		// If we received a config processor pointer, we load the global configure tokens
		if (this->confProcHdl)
		{
			binding.ayf_load_config_content_call(this, &cfgDataInit, nullptr);
		}
	}
	return res;
}

jvxErrorType
CayfComponentLib::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;	

	if (cfgDataInit)
	{
		if (this->confProcHdl)
		{
			binding.ayf_release_config_content_call(this, cfgDataInit);
		}
	}
	
	if (locHost)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(locHost);
	}
	else
	{
		if (binding.ayf_unregister_module_host_call)
		{
			binding.ayf_unregister_module_host_call(this);
		}
		this->minHostRef = nullptr;
	}

	this->hostRef = nullptr;
	_common_set_min.theHostRef = nullptr;

	this->confProcHdl = nullptr;
	CjvxObject::_terminate();

	unpopulateBindingRefs();
	return res;
}

jvxErrorType
CayfComponentLib::activate()
{
	jvxApiString astr;
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
		/*
		_common_set_ocon.ocon = static_cast<IjvxOutputConnector*>(this);
		_common_set_icon.icon = static_cast<IjvxInputConnector*>(this);
		_common_set_io_common.theMaster = static_cast<IjvxConnectionMaster*>(this);
		*/
		CjvxInputOutputConnector::lds_activate(nullptr, static_cast<IjvxObject*>(this),
			nullptr, static_cast<IjvxConnectionMaster*>(this), "default", 
			static_cast<IjvxInputConnector*>(this), static_cast<IjvxOutputConnector*>(this));

		// Allocate the single main node for processing 
		resC = allocate_main_node(); 
		if ((resC == JVX_NO_ERROR) && this->mainObj)
		{
			this->mainNode = castFromObject<IjvxNode>(this->mainObj);
			resC = this->mainNode->initialize(this->hostRef);

			if (resC == JVX_NO_ERROR)
			{
				this->mainNode->set_location_info(jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM, 0));
				
				resC = this->mainNode->select(nullptr);

				this->mainNode->module_reference(&astr, nullptr);

				passConfigSection(this->mainNode, astr.std_str());

			}
			if (resC == JVX_NO_ERROR)
			{
				resC = on_main_node_selected();
			}

			resC = this->mainNode->activate();
			if (resC == JVX_NO_ERROR)
			{
				passConfigSection(this->mainNode, astr.std_str());

				// Attach main node to host control
				if (binding.ayf_attach_component_module_call)
				{
					resC = binding.ayf_attach_component_module_call(mainNodeName.c_str(), this, this->mainObj);
				}

				// Create simple connection
				theConnections = reqInterface<IjvxDataConnections>(this->hostRef);
				assert(theConnections);

				std::string nmprocess = chainName;
				jvxSize cnt = 1;
				jvxBool interceptors = false;
				jvxSize idProcDepends = JVX_SIZE_UNSELECTED;
				jvxBool preventStore = true;
				IjvxConnectorFactory* iFac = nullptr;
				IjvxInputConnectorSelect* icon = nullptr;
				IjvxOutputConnectorSelect* ocon = nullptr;
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
								resC = theProc->create_bridge(static_cast<IjvxOutputConnector*>(this), icon, "Device to node bridge", &uId_bridge_dev_node, false, false);
							}

							if (resC == JVX_NO_ERROR)
							{
								resC = theProc->create_bridge(ocon, static_cast<IjvxInputConnector*>(this), "Node to device bridge", &uId_bridge_node_dev, false, false);
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
				retInterface<IjvxDataConnections>(this->hostRef, theConnections);
				theConnections = nullptr;

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
	theConnections = reqInterface<IjvxDataConnections>(this->hostRef);
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
			retInterface<IjvxDataConnections>(this->hostRef, theConnections);
		}
		theConnections = nullptr;

		if (binding.ayf_detach_component_module_call)
		{
			resC = binding.ayf_detach_component_module_call(mainNodeName.c_str(), this);
		}
		assert(resC == JVX_NO_ERROR);

		resC = this->mainNode->deactivate();
		assert(resC == JVX_NO_ERROR);

		resC = before_main_node_unselect();
		assert(resC == JVX_NO_ERROR);

		resC = this->mainNode->unselect();
		assert(resC == JVX_NO_ERROR);

		resC = this->mainNode->terminate();
		assert(resC == JVX_NO_ERROR);

		deallocate_main_node();
		this->mainObj = nullptr;
		this->mainNode = nullptr;
		
		CjvxInputOutputConnector::lds_deactivate();
		resC = _deactivate();
		assert(resC == JVX_NO_ERROR);

		JVX_PROPERTIES_ALL_STOP(genComponentLib);
	}
	return resC;
}

jvxErrorType 
CayfComponentLib::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str()
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
			if (binding.ayf_forward_text_command_call)
			{
				binding.ayf_forward_text_command_call(txtMessageCollect.c_str(), this, astr);
			}
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
CayfComponentLib::passConfigSection(IjvxSimpleNode* node, const std::string& moduleName)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datSec = nullptr;
	if (confProcHdl && cfgDataInit)
	{
		confProcHdl->getReferenceEntryCurrentSection_name(cfgDataInit, &datSec, moduleName.c_str());
		if (datSec)
		{

			// The passed configuration is a section with the name as it fits. We need not search for any 
			// sub section to match it properly
			IjvxConfiguration* cfg = reqInterfaceObj<IjvxConfiguration>(node);
			if (cfg)
			{
				jvxCallManagerConfiguration callMan;
				res = cfg->put_configuration(&callMan, confProcHdl, datSec);
				retInterfaceObj<IjvxConfiguration>(node, cfg);
			}
		}
	}
	return res;
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

