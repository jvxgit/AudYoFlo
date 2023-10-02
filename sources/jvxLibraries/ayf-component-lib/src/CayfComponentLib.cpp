#include "CayfComponentLib.h"
#include "CayfComponentLibContainer.h"
#include "common/CjvxConnectorFactory.h"

#include "ayf-embedding-proxy-entries.h"
#include "ayf-embedding-libs.h"

#include "jvxHosts/CjvxDataConnections.h"

#define Q(x) #x
#define Quotes(x) Q(x)
#define SET_DLL_REFERENCE(ret, entry) ret. ## entry ## _call = (entry) JVX_GETPROCADDRESS(proxyLibHandleGlobal, Quotes(entry))

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

// ================================================================================================
// Here, we run the global stuff: opening sub dll once and receiving the binding references
// ================================================================================================

JVX_HMODULE proxyLibHandleGlobal = JVX_HMODULE_INVALID;
ayfEmbeddingProxyReferences proxyReferencesGlobal;
ayfHostBindingReferences* bindingGlobal = nullptr;
jvxSize idRegisterGlobal = 0;
jvxSize refCntGlobal = 0;

jvxErrorType
CayfComponentLib::populateBindingRefs(const std::string &myRegisterName, const std::string& rootPath, ayfHostBindingReferences*& bindOnReturn)
{
	if (proxyLibHandleGlobal == JVX_HMODULE_INVALID)
	{
		std::string fNameDll = AYF_EMBEDDING_PROXY_HOST;
		proxyLibHandleGlobal = JVX_LOADLIBRARY(fNameDll.c_str());
		if (proxyLibHandleGlobal != JVX_HMODULE_INVALID)
		{
			SET_DLL_REFERENCE(proxyReferencesGlobal, ayf_embedding_proxy_init);
			SET_DLL_REFERENCE(proxyReferencesGlobal, ayf_embedding_proxy_terminate);
		}

		if (proxyReferencesGlobal.ayf_embedding_proxy_init_call)
		{
			proxyReferencesGlobal.ayf_embedding_proxy_init_call(myRegisterName.c_str(), &idRegisterGlobal, &bindingGlobal, rootPath.c_str());
		}

		refCntGlobal = 1;
		// Currently just alocal assignemnt
		/*
		binding->bindType = ayfHostBindingType::AYF_HOST_BINDING_MIN;
		binding->ayf_register_module_host_call = ayf_register_module_host;
		binding->ayf_unregister_module_host_call = ayf_unregister_module_host;
		binding->ayf_load_config_content_call = ayf_load_config_content;
		binding->ayf_release_config_content_call = ayf_release_config_content;
		binding->ayf_attach_component_module_call = ayf_attach_component_module;
		binding->ayf_detach_component_module_call = ayf_detach_component_module;
		binding->ayf_forward_text_command_call = ayf_forward_text_command;
		*/

		bindOnReturn = bindingGlobal;
		return JVX_NO_ERROR;
	}
	bindOnReturn = bindingGlobal;
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CayfComponentLib::unpopulateBindingRefs()
{
	if (proxyLibHandleGlobal)
	{
		refCntGlobal--;
		if (refCntGlobal == 0)
		{
			if (proxyReferencesGlobal.ayf_embedding_proxy_terminate_call)
			{
				proxyReferencesGlobal.ayf_embedding_proxy_terminate_call(idRegisterGlobal, bindingGlobal);
				bindingGlobal = nullptr;
				JVX_UNLOADLIBRARY(proxyLibHandleGlobal);
				proxyLibHandleGlobal = JVX_HMODULE_INVALID;				
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

CayfComponentLib::CayfComponentLib(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE, CayfComponentLibContainer* parentArg) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	switch (bindingGlobal->bindType)
	{
	case ayfHostBindingType::AYF_HOST_BINDING_NONE:
		bindingGlobal->request_specialization(reinterpret_cast<jvxHandle**>(&binding), bindingGlobal->bindType);
		break;
	case ayfHostBindingType::AYF_HOST_BINDING_MIN_HOST:
		bindingGlobal->request_specialization(reinterpret_cast<jvxHandle**>(&bindingMinHost), bindingGlobal->bindType);
		break;
	case ayfHostBindingType::AYF_HOST_BINDING_EMBEDDED_HOST:
		bindingGlobal->request_specialization(reinterpret_cast<jvxHandle**>(&bindingEmbHost), bindingGlobal->bindType);
		break;
	default: 
		assert(0);
	}
	parent = parentArg;
}

CayfComponentLib::~CayfComponentLib()
{
	binding = nullptr;
}

jvxErrorType 
CayfComponentLib::initialize(IjvxHiddenInterface* hostRef)
{
	// This entry to be done later..
	jvxErrorType res = JVX_NO_ERROR;
	/*
	res = CjvxObject::_initialize(hostRef);
	if (res == JVX_NO_ERROR)
	{
		// 
	*/
	IjvxToolsHost* toolsHost = reqInterface<IjvxToolsHost>(hostRef);
	if (toolsHost)
	{
		embHost.retCfgProc = reqRefTool<IjvxConfigProcessor>(toolsHost, embHost.tp);
		retInterface<IjvxToolsHost>(hostRef, toolsHost);
	}
	embHost.hostRef = reqInterface<IjvxHost>(hostRef);
	return initialize(embHost.hostRef, embHost.retCfgProc.cpPtr, _common_set.theModuleName);
}

jvxErrorType
CayfComponentLib::initialize(IjvxMinHost* hostRef, IjvxConfigProcessor* confProc, const std::string& realRegName)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize(hostRef);
	if (res == JVX_NO_ERROR)
	{
		this->minHostRef = hostRef;
		this->confProcHdl = confProc;
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
			if (bindingMinHost)
			{
				bindingMinHost->ayf_load_config_content_call(this, &cfgDataInit, nullptr);
			}
			if (bindingEmbHost)
			{
				bindingEmbHost->ayf_load_config_content_factory_host_call(parent, &cfgDataInit, nullptr);
			}
		}
	}
	return res;
}

jvxErrorType
CayfComponentLib::terminate()
{
	// To be done later
	jvxErrorType res = JVX_NO_ERROR;
	ayfHostBindingReferencesMinHost* bind = nullptr;

	IjvxHiddenInterface* hostRefLoc = _common_set_min.theHostRef;

	res = terminate(bind);

	IjvxToolsHost* toolsHost = reqInterface<IjvxToolsHost>(hostRefLoc);
	if (toolsHost)
	{
		retRefTool<IjvxConfigProcessor>(toolsHost, embHost.tp.tp, embHost.retCfgProc);
		retInterface<IjvxToolsHost>(embHost.hostRef, toolsHost);
	}
	retInterface<IjvxHost>(hostRefLoc, embHost.hostRef);

	return res;

}

jvxErrorType
CayfComponentLib::terminate(ayfHostBindingReferencesMinHost*& refOnReturn)
{
	jvxErrorType res = JVX_NO_ERROR;	

	if (cfgDataInit)
	{
		if (this->confProcHdl)
		{
			if (bindingMinHost)
			{
				bindingMinHost->ayf_release_config_content_call(this, cfgDataInit);
			}
			if (bindingEmbHost)
			{
				bindingEmbHost->ayf_release_config_content_factory_host_call(parent, cfgDataInit);
			}
		}
	}
	
	if (locHost)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(locHost);
	}
	else
	{
		refOnReturn = bindingMinHost;
		this->minHostRef = nullptr;
	}

	this->hostRef = nullptr;
	_common_set_min.theHostRef = nullptr;

	this->confProcHdl = nullptr;
	CjvxObject::_terminate();

	return res;
}

#include "common/CjvxConnectorFactory.h"

jvxErrorType
CayfComponentLib::activate()
{
	jvxSize i;
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
		if (exposeFactories)
		{
			JVX_SAFE_ALLOCATE_OBJECT(optConFactory, CjvySimpleConnectorFactory(static_cast<IjvxObject*>(this)));
			JVX_SAFE_ALLOCATE_OBJECT(optMasFactory, CjvxSimpleConnectionMasterFactory(static_cast<IjvxObject*>(this)));
		}

		CjvxInputOutputConnector::lds_activate(nullptr, static_cast<IjvxObject*>(this),
			optConFactory, static_cast<IjvxConnectionMaster*>(this), "default",
			static_cast<IjvxInputConnector*>(this), static_cast<IjvxOutputConnector*>(this));

		// Start the input output connector factory
		if (optConFactory)
		{
			optConFactory->activate(
				static_cast<IjvxInputConnectorSelect*>(this), 
				static_cast<IjvxOutputConnectorSelect*>(this), 
				static_cast<IjvxConnectionMaster*>(this), 
				_common_set.theModuleName.c_str());
		}

		if (optMasFactory)
		{
			_init_master(static_cast<CjvxObject*>(this), "default", optMasFactory);
			optMasFactory->activate("default", static_cast<IjvxConnectionMaster*>(this));
		}

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

				// Attach main node to host control - if embedded in full host
				IjvxComponentHostExt* hostExt = nullptr;
				hostExt = reqInterface<IjvxComponentHostExt>(hostRef);
				if (hostExt)
				{
					hostExt->attach_external_component(mainNode, regName.c_str(), true);
				}
				else
				{
					if (bindingMinHost)
					{
						resC = bindingMinHost->ayf_attach_component_module_call(mainNodeName.c_str(), this, this->mainObj);
					}
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
					for (i = 0; i < numIcons; i++)
					{
						jvxApiString conName;
						iFac->reference_input_connector(i, &icon);
						if (icon)
						{
							icon->descriptor_connector(&conName);
							if (conName.std_str() == conToName)
							{
								break;
							}
							else
							{
								iFac->return_reference_input_connector(icon);
								icon = nullptr;
							}
						}
					}

					for (i = 0; i < numOcons; i++)
					{
						jvxApiString conName;
						iFac->reference_output_connector(i, &ocon);
						if (ocon)
						{
							ocon->descriptor_connector(&conName);
							if (conName.std_str() == conFromName)
							{
								break;
							}
							else
							{
								iFac->return_reference_output_connector(ocon);
								ocon = nullptr;
							}
						}
					}
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
					else
					{
						std::cout << "Unable to find connectors for from and to connection." << std::endl;
					}
					retInterfaceObj< IjvxConnectorFactory>(this->mainNode, iFac);
					iFac = nullptr;
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
			
			// The process might have been removed already if we use the embedded or full host: 
			// As soon as we unregister the connector factory and/or the master factory, the process will be cleared by
			// the host. This is ok, however.
			if (resC == JVX_NO_ERROR)
			{

				resC = theProc->disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
				assert(resC == JVX_NO_ERROR);

				theConnections->return_reference_connection_process(theProc);

				if (JVX_CHECK_SIZE_SELECTED(uId_process))
				{
					resC = theConnections->remove_connection_process(uId_process);
					assert(resC == JVX_NO_ERROR);
				}
			}
			resC = JVX_NO_ERROR;
			uId_process = JVX_SIZE_UNSELECTED;
			retInterface<IjvxDataConnections>(this->hostRef, theConnections);
		}
		theConnections = nullptr;

		IjvxComponentHostExt* hostExt = nullptr;
		hostExt = reqInterface<IjvxComponentHostExt>(hostRef);
		if (hostExt)
		{
			resC = hostExt->detach_external_component(this->mainNode, regName.c_str(), JVX_STATE_ACTIVE);
		}
		else
		{
			if (bindingMinHost)
			{
				resC = bindingMinHost->ayf_detach_component_module_call(mainNodeName.c_str(), this);
			}
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
		
		if (optMasFactory)
		{			
			optMasFactory->deactivate();
			_terminate_master();
			JVX_SAFE_DELETE_OBJECT(optMasFactory);
		}

		if (optConFactory)
		{
			optConFactory->deactivate();
			JVX_SAFE_DELETE_OBJECT(optConFactory);
		}

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
			if (bindingMinHost)
			{
				bindingMinHost->ayf_forward_text_command_call(txtMessageCollect.c_str(), this, astr);
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

