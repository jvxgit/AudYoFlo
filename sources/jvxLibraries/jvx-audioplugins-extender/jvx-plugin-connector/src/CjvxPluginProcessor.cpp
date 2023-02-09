#include "CjvxPluginProcessor.h"
#include "CjvxPluginCommon.h"
#include "CjvxPluginProcessor_specific.h"

#include "jvxFHCon_nd.h"
#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTThreads.h"
#include "jvxTDataLogger.h"

/*
 *  IMPORTANT CHANGE: the host reference is unique within the VST host.
 *  Therefore we may only open it once and then keep track of it within a reference counter,
 * 
 *  The following must be static instances since multiple objects share the access
 */
static IjvxObject* objHost = nullptr; // jvxHMin_init
static IjvxFactoryHost* facHost = nullptr;
static std::vector<oneAddedStaticComponent> addedStaticObjects;

/*******************************************************************/

CjvxPluginProcessor::CjvxPluginProcessor()
{
	JVX_INITIALIZE_MUTEX(safeAccessProcessing);
}

CjvxPluginProcessor::~CjvxPluginProcessor()
{
	JVX_TERMINATE_MUTEX(safeAccessProcessing);
}

jvxErrorType
CjvxPluginProcessor::initialize(IjvxPropertyExtenderStreamAudioPlugin* ext)
{
	parameter_reset();

	// Default configuration user and system path
	host_parameters.userPath.val = JVX_GET_USER_DESKTOP_PATH();
	host_parameters.userPath.set = true;

	host_parameters.systemPath.val = JVX_GET_CURRENT_MODULE_PATH(JVX_FUNCTION_RETURN_ADDRESS);
	host_parameters.systemPath.set = true;
	 
	theExtender = ext;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::terminate()
{
	theExtender = nullptr;
	parameter_reset();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxPluginProcessor::add_component(jvxInitObject_tp init, jvxTerminateObject_tp term, const std::string& nm)
{
	oneComponentDescriptor theRef;
	theRef.name = nm;
	theRef.funcInit = init;
	theRef.funcTerm = term;

	for (const oneComponentDescriptor& elm : registeredComponents)
	{
		if (elm.name == nm)
		{
			return JVX_ERROR_DUPLICATE_ENTRY;
		}
	}
	this->registeredComponents.push_back(theRef);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::clear_components()
{
	this->registeredComponents.clear();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxPluginProcessor::initialize_connection(jvxSize numPluginInChannels, jvxSize numPluginOutChannels)
{
	jvxSize cnt = 0;
	jvxBool allComponentsOk = true;
	jvxComponentIdentification cpTp;
	jvxErrorType res = JVX_ERROR_INTERNAL, resL = JVX_NO_ERROR;
	oneAddedStaticComponent comp;
	
	if(objHost == nullptr)
	{
		jvxFHCon_nd_init(&objHost);
		assert(objHost);
		jvxState stat = JVX_STATE_NONE;

		objHost->request_specialization(reinterpret_cast<jvxHandle**>(&facHost), &cpTp, NULL);
		assert(cpTp.tp == JVX_COMPONENT_FACTORY_HOST);

		facHost->state(&stat);
		if (stat == JVX_STATE_NONE)
		{
			if (facHost)
			{
				resL = facHost->initialize(NULL);
				if (resL == JVX_NO_ERROR)
				{
					resL = facHost->select();
				}
				if (resL == JVX_NO_ERROR)
				{
					configure_host();

					LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init,
						jvxTSystemTextLog_terminate, "Text Log",
						addedStaticObjects,
						facHost);

					LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init,
						jvxTconfigProcessor_terminate, "Config Parser",
						addedStaticObjects,
						facHost);
					LOAD_ONE_MODULE_LIB_FULL(jvxTThreads_init,
						jvxTThreads_terminate, "Threads",
						addedStaticObjects,
						facHost);
					LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init,
						jvxTDataLogger_terminate, "Data Logger",
						addedStaticObjects, facHost);

					JVX_PLUGIN_CONNECTOR_LOAD_MODULES

						resL = facHost->activate();
					refCountHost++;
				}
				if (resL != JVX_NO_ERROR)
				{
					allComponentsOk = false;
				}
			}
			else
			{
				allComponentsOk = false;
			}
		}
	}
	else
	{
		// Host has been initialized before
		facHost->increment_reference();
	}

	facHost->current_reference(&refCountHost);

	for (CjvxPluginProcessor::oneComponentDescriptor& descr : registeredComponents)
	{
		descr.funcInit(&descr.objNode, NULL, NULL);
		if (descr.objNode)
		{
			descr.objNode->request_specialization(reinterpret_cast<jvxHandle**>(&descr.theNode), &cpTp, NULL);
			if (descr.theNode)
			{
				resL = descr.theNode->initialize(facHost);
				if (resL == JVX_NO_ERROR)
				{
					resL = descr.theNode->select();
				}
				if (resL == JVX_NO_ERROR)
				{
					resL = descr.theNode->activate();
				}
				if (resL == JVX_NO_ERROR)
				{
					// Give each implementation a number
					resL = descr.theNode->set_location_info(jvxComponentIdentification(refCountHost, 0));
				}
				if (resL != JVX_NO_ERROR)
				{
					allComponentsOk = false;
					break;
				}
			}
		}
	}

	if (allComponentsOk)
	{
		numNodes = registeredComponents.size();
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(lstNodes, IjvxHiddenInterface*, numNodes);

		cnt = 0;
		for (CjvxPluginProcessor::oneComponentDescriptor& descr : registeredComponents)
		{
			lstNodes[cnt] = descr.theNode;
			icons.add("default");
			ocons.add("default");
		}
		icons.produce();
		ocons.produce();

		theMicroConnection = new HjvxMicroConnection("Micro Connection VST",
			false, "Micro Connection VST",
			0, "local-vst-connection",
			JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
		theMicroConnection->set_location_info(jvxComponentIdentification(refCountHost, JVX_SIZE_SLOT_OFF_SYSTEM));

		if (theMicroConnection)
		{
			std::string pname = "vst-process";
			pname += "_" + jvx_size2String(refCountHost);

			// Create the connection			
			res = theMicroConnection->activate_connection(facHost,
				lstNodes, icons.cchars, ocons.cchars, numNodes,
				"default", "default",
				pname.c_str());

			if (res == JVX_NO_ERROR)
			{
				// Connect the connection
				theMicroConnection->connect_connection(
					&descr_conn_in,
					&descr_conn_out);

				// We keep the format open. Output plugin is input micro connection
				neg_input._set_parameters_fixed(numPluginOutChannels, 512, 48000,
					JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);

				neg_output._set_parameters_fixed(numPluginInChannels, 512, 48000,
					JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
				return res;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxPluginProcessor::terminate_connection()
{
	if (theMicroConnection)
	{
		theMicroConnection->disconnect_connection();

		theMicroConnection->deactivate_connection();
		delete(theMicroConnection);
		theMicroConnection = nullptr;
	}

	icons.clear();
	ocons.clear();
	JVX_DSP_SAFE_DELETE_FIELD(lstNodes);
	lstNodes = nullptr;

	for (CjvxPluginProcessor::oneComponentDescriptor& descr : registeredComponents)
	{
		descr.theNode->deactivate();
		descr.theNode->unselect();
		descr.theNode->terminate();
		descr.funcTerm(descr.objNode);
	}
	if (facHost)
	{
		jvxSize i;
		jvxSize refCnt = 0;
		facHost->current_reference(&refCnt);
		if (refCnt > 0)
		{
			facHost->decrement_reference();
		}
		else
		{
			facHost->deactivate();
			// Remove all library objects
			for (i = 0; i < addedStaticObjects.size(); i++)
			{
				UNLOAD_ONE_MODULE_LIB_FULL(addedStaticObjects[i], facHost);
			}
			addedStaticObjects.clear();
			facHost->unselect();
			facHost->terminate();

			facHost = nullptr;
			if (objHost)
			{
				jvxFHCon_nd_terminate(objHost);
			}
			objHost = nullptr;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::update_plugin_parameters(jvxSize bsize, jvxSize srate)
{
	neg_output._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		bsize,
		srate);
	neg_input._update_parameters_fixed(
		JVX_SIZE_UNSELECTED,
		bsize,
		srate);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::start_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (connection_started)
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	else
	{
		neg_output.derive(&descr_conn_out.con_params);
		neg_input.derive(&descr_conn_in.con_params);

		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(var);
		res = theMicroConnection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(var));
		if (res == JVX_NO_ERROR)
		{
			descr_conn_out.con_data.number_buffers = 1;
			res = theMicroConnection->prepare_connection(false, false);
		}
		if (res == JVX_NO_ERROR)
		{
			res = theMicroConnection->start_connection();
		}
		connection_started = true;
	}
	return res;
}

jvxErrorType
CjvxPluginProcessor::stop_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (connection_started)
	{
		res = theMicroConnection->stop_connection();
		res = theMicroConnection->postprocess_connection();
		connection_started = false;
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType 
CjvxPluginProcessor::request_property_reference(IjvxProperties** theProp, const std::string& name)
{
	jvxErrorType res = JVX_NO_ERROR;
	for (const oneComponentDescriptor& elm : registeredComponents)
	{
		if (elm.name == name)
		{
			if (elm.objNode)
			{
				res = elm.theNode->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(theProp));
				return res;
			}
			res = JVX_ERROR_INVALID_SETTING;
		}
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}

jvxErrorType
CjvxPluginProcessor::return_property_reference(IjvxProperties* theProp, const std::string& name)
{
	jvxErrorType res = JVX_NO_ERROR;
	for (const oneComponentDescriptor& elm : registeredComponents)
	{
		if (elm.name == name)
		{
			if (elm.objNode)
			{
				res = elm.theNode->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProp));
				return res;
			}
			res = JVX_ERROR_INVALID_SETTING;
		}
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return res;
}

// ==========================================================================

jvxErrorType
CjvxPluginProcessor::hook_test_negotiate(jvxLinkDataDescriptor* proposed JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::hook_test_accept(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxPluginProcessor::hook_forward(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::hook_test_update(jvxLinkDataDescriptor* dataIn  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::hook_check_is_ready(jvxBool* is_ready, jvxApiString* astr)
{
	return JVX_NO_ERROR;
}


// ============================================================================

jvxErrorType
CjvxPluginProcessor::configure_host()
{
	jvxApiString theStr;
	jvxApiStringList theStrLst;
	IjvxProperties* theProps = NULL;
	jvxSize id = 0;
	jvxCallManagerProperties callGate;
	jvxErrorType res = facHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	assert((res == JVX_NO_ERROR) && theProps);

	// Set the predefined property for dll path for subcomponents
	if (host_parameters.dllsDirectories.size())
	{
		if (jvx_findPropertyDescriptor("jvx_component_path_str_list", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyStringList(host_parameters.dllsDirectories,
				facHost, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}
	}	

	if (host_parameters.systemPath.set)
	{
		// Set the predefined property for dll path for subcomponents
		if (jvx_findPropertyDescriptor("jvx_system_path_str", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyString(host_parameters.systemPath.val,
				facHost, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}
	}

	if (host_parameters.userPath.set)
	{
		if (jvx_findPropertyDescriptor("jvx_user_path_str", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyString(host_parameters.userPath.val,
				facHost, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}
	}

	res = jvx_set_property(theProps, &host_parameters.out_cout, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/host_output_cout", callGate);
	assert(res == JVX_NO_ERROR);

	res = jvx_set_property(theProps, &host_parameters.verbose_dll, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/host_verbose_dll", callGate);
	assert(res == JVX_NO_ERROR);

	theStr.assign_const(host_parameters.textLog_filename.c_str(), host_parameters.textLog_filename.size());
	res = jvx_set_property(theProps, &theStr, 0, 1, JVX_DATAFORMAT_STRING, false, "/textLog_filename", callGate);
	assert(res == JVX_NO_ERROR);

	res = jvx_set_property(theProps, &host_parameters.textLog_dbglevel, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_dbglevel", callGate);
	assert(res == JVX_NO_ERROR);

	res = jvx_set_property(theProps, &host_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferFile", callGate);
	assert(res == JVX_NO_ERROR);

	res = jvx_set_property(theProps, &host_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeTransferFile", callGate);
	assert(res == JVX_NO_ERROR);

	res = jvx_set_property(theProps, &host_parameters.textLog_sizeInternBufferRW, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferRW", callGate);
	assert(res == JVX_NO_ERROR);

	if (host_parameters.textLog_expressions.size())
	{
		theStrLst.assign(host_parameters.textLog_expressions);
		res = jvx_set_property(theProps, &theStrLst, 0, 1, JVX_DATAFORMAT_STRING_LIST, false, "/textLog_expressions", callGate);
		assert(res == JVX_NO_ERROR);
	}
	res = jvx_set_property(theProps, &host_parameters.textLog_activate, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/textLog_activate", callGate);
	assert(res == JVX_NO_ERROR);

	res = facHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	return res;
}

// =====================================================================================

jvxErrorType 
CjvxPluginProcessor::add_config_text(const std::string& config, const std::string& text)
{
	if (config == "--systemdir")
	{
		host_parameters.systemPath.val = text; 
		host_parameters.systemPath.set = true;
		return JVX_NO_ERROR;
	}
	else if (config == "--userdir")
	{
		host_parameters.userPath.val = text;
		host_parameters.userPath.set = true;
		return JVX_NO_ERROR;
	}
	else if (config == "--textlogfilename")
	{
		host_parameters.textLog_filename = text;
		return JVX_NO_ERROR;
	}
	else if (config == "--textlogexpr")
	{
		host_parameters.textLog_expressions.push_back(text);
		return JVX_NO_ERROR;
	}
	else if (config == "--jvxdir")
	{
		host_parameters.dllsDirectories.push_back(text);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxPluginProcessor::add_config_flag(const std::string& config, jvxBool flag)
{
	if (config == "--textlog")
	{
		host_parameters.textLog_activate = flag;
		return JVX_NO_ERROR;
	}
	else if (config == "--n_out_cout")
	{
		host_parameters.out_cout = flag;
		return JVX_NO_ERROR;
	}
	else if (config == "--verbose_dll")
	{
		host_parameters.verbose_dll = flag;
		return JVX_NO_ERROR;
	}

	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxPluginProcessor::add_config_value(const std::string& config, jvxSize value)
{
	if (config == "--textlogszrw")
	{
		host_parameters.textLog_sizeInternBufferRW = value;
		return JVX_NO_ERROR;
	}
	else if (config == "--textlogszfile")
	{
		host_parameters.textLog_sizeInternBufferFile = value;
		return JVX_NO_ERROR;
	}
	else if (config == "--textloglev")
	{
		host_parameters.textLog_dbglevel = value;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
CjvxPluginProcessor::parameter_reset()
{
	host_parameters.systemPath.val = "";
	host_parameters.systemPath.set = false;

	host_parameters.userPath.val = "";
	host_parameters.userPath.set = false;

	host_parameters.textLog_filename = ".tlog"; // "--textlogfilename"
	host_parameters.textLog_expressions.clear(); // "--textlogexpr"
	host_parameters.textLog_activate = false; // "--textlog"
	host_parameters.textLog_sizeInternBufferFile = 8192; // "--textlogszfile"
	host_parameters.textLog_sizeInternBufferRW = 1024; // "--textlogszrw"
	host_parameters.textLog_dbglevel = 10; // "--textloglev"
	host_parameters.out_cout = false; // "--n_out_cout"
	host_parameters.verbose_dll = false; // "--verbose_dll"
	host_parameters.dllsDirectories.clear();
}

// ===========================================================================


jvxErrorType
CjvxPluginProcessor::getConfiguration(std::string& txtOut)
{	
	
	IjvxToolsHost* tHost = reqInterface<IjvxToolsHost>(facHost);
	refComp<IjvxConfigProcessor> cfg = reqRefTool< IjvxConfigProcessor>(
		tHost, JVX_COMPONENT_CONFIG_PROCESSOR);
	if (cfg.cpPtr)
	{
		jvxConfigData* theMainSec = nullptr;
		jvxSize cnt = 0;
		cfg.cpPtr->createEmptySection(&theMainSec, "JVX_VST_MAIN_SECTION");
		if (theMainSec)
		{
			for (const oneComponentDescriptor& cp: registeredComponents)
			{
				IjvxConfiguration* locCfg = reqInterface<IjvxConfiguration>(cp.theNode);
				if (locCfg)
				{
					jvxConfigData* locSec = nullptr;
					std::string sec_token = "NODE_" + jvx_size2String(cnt);
					cfg.cpPtr->createEmptySection(&locSec, sec_token.c_str());
					if (locSec)
					{
						jvxCallManagerConfiguration callMan;
						jvxErrorType res = locCfg->get_configuration(&callMan, cfg.cpPtr, locSec);
						if (res == JVX_NO_ERROR)
						{
							cfg.cpPtr->addSubsectionToSection(theMainSec, locSec);
						}
					}
				}
			}
			jvxApiString astr;
			cfg.cpPtr->printConfiguration(theMainSec, &astr, true);
			txtOut = astr.std_str();
		}
	}
	retRefTool<IjvxConfigProcessor>(tHost, JVX_COMPONENT_CONFIG_PROCESSOR, cfg);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::putConfiguration(const std::string& txtOut)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxToolsHost* tHost = reqInterface<IjvxToolsHost>(facHost);
	refComp<IjvxConfigProcessor> cfg = reqRefTool< IjvxConfigProcessor>(
		tHost, JVX_COMPONENT_CONFIG_PROCESSOR);
	if (cfg.cpPtr)
	{
		lock_processing();
		jvxErrorType resStopped = disengage_processing();
		
		jvxConfigData* theMainSec = nullptr;
		jvxSize cnt = 0;
		res = cfg.cpPtr->parseTextField(txtOut.c_str(), "Preset", 0);
		if (res == JVX_NO_ERROR)
		{
			res = cfg.cpPtr->getConfigurationHandle(&theMainSec);
			if (theMainSec)
			{
				for (const oneComponentDescriptor& cp : registeredComponents)
				{
					IjvxConfiguration* locCfg = reqInterface<IjvxConfiguration>(cp.theNode);
					if (locCfg)
					{
						jvxConfigData* locSec = nullptr;
						std::string sec_token = "NODE_" + jvx_size2String(cnt);
						cfg.cpPtr->getReferenceEntryCurrentSection_name(theMainSec, &locSec, sec_token.c_str());
						if (locSec)
						{
							jvxCallManagerConfiguration callMan;
							jvxErrorType res = locCfg->put_configuration(&callMan, cfg.cpPtr, locSec);
						}
					}
				}
			}
			jvxApiString astr;
			cfg.cpPtr->removeHandle(theMainSec);
		}
		if (resStopped == JVX_NO_ERROR)
		{
			engage_processing();
		}
		unlock_processing();
	}
	retRefTool<IjvxConfigProcessor>(tHost, JVX_COMPONENT_CONFIG_PROCESSOR, cfg);
	return JVX_NO_ERROR;
}
// ===========================================================================

jvxErrorType 
CjvxPluginProcessor::handle_message_from_controller(
	IjvxPluginMessageControl* respond,
	jvxAudioPluginBinaryHeader* bHdr)
{
	std::string tag;
	jvxSize i;
	jvxSize numC = 0;
	jvxByte* ptr = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxInt32 id = -1;
	jvxData normVal = 0;
	jvxApiStringList strl; 
	jvxAudioPluginBinaryParamHeader* hdr = (jvxAudioPluginBinaryParamHeader*)bHdr;
	jvxAudioPluginBinaryLinFieldHeader* hdrlf = (jvxAudioPluginBinaryLinFieldHeader*)hdr;
	IjvxAudioPluginReport* rep_ref = nullptr;
	jvxAudioPluginBinaryField_trigger* trigger_ids = nullptr;
	jvxAudioPluginBinaryField_sync_norm* update_ids = nullptr;
	jvxAudioPluginBinaryField_sync_norm* ptrParam = nullptr;
	jvxAudioPluginBinaryField_sync_norm_strlist* ptrParamStrLst = nullptr;

	if (bHdr->command == JVX_AUDIOPLUGIN_REPORT_PROCESS_COMPLETE)
	{
		assert(0);
	}
	else if (bHdr->command == JVX_AUDIOPLUGIN_TRIGGER_SYNC_STATE)
	{
		jvxAudioPluginBinaryStateChangeHeader hdrR;
		hdrR.hdr.command = JVX_AUDIOPLUGIN_REPORT_CURRENT_STATE;
		hdrR.hdr.sz = sizeof(jvxAudioPluginBinaryStateChangeHeader);
		hdrR.hdr.ass_process_id = bHdr->rep_process_id;
		hdrR.hdr.rep_process_id = -1; // A response never requires a response!
		hdrR.stateId = currentState;
		res = respond->plugin_send_message((jvxAudioPluginBinaryHeader*)&hdrR);

		if (bHdr->rep_process_id > 0)
		{
			// Send a response where required
			jvxAudioPluginBinaryHeader theRequest;
			CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
				&theRequest, -1, bHdr->rep_process_id);
			respond->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
		}
	}
	else if (bHdr->command == JVX_AUDIOPLUGIN_TRIGGER_REQ_LIN_FIELD)
	{
		jvxAudioPluginBinaryLinFieldHeader* ret_hdr = nullptr;
		ptr = (jvxByte*)hdrlf + sizeof(jvxAudioPluginBinaryLinFieldHeader);
		numC = hdr->numChar;
		if (numC)
		{
			tag.assign(ptr, numC);
		}
		theExtender->request_report_handle(&rep_ref, tag.c_str());
		if (rep_ref)
		{
			jvxData* fld_ptr = nullptr;
			jvxSize fld_sz = 0;

			CjvxPluginCommon::prepare_controller_processor_message_send_fld(
				&fld_ptr, &fld_sz, hdrlf->id, hdrlf->subid,
				hdrlf->num, hdrlf->offset,
				&ret_hdr, -1, hdrlf->hdr.rep_process_id);
			if (fld_ptr)
			{
				res = rep_ref->request_field_context(fld_ptr, fld_sz, hdrlf->id,
					hdrlf->subid, hdrlf->offset, hdrlf->num);

				res = respond->plugin_send_message((jvxAudioPluginBinaryHeader*)ret_hdr);
			}
			CjvxPluginCommon::release_controller_processor_message(
				(jvxAudioPluginBinaryHeader*)ret_hdr);

			if (hdrlf->hdr.rep_process_id > 0)
			{
				// Send a response where required
				jvxAudioPluginBinaryHeader theRequest;
				CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
					&theRequest, -1, hdrlf->hdr.rep_process_id);
				respond->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
			}
		}
		theExtender->return_report_handle(rep_ref);
	}
	else
	{
		jvxInt16 proc_id = hdr->hdr.rep_process_id;
		ptr = (jvxByte*)hdr + sizeof(jvxAudioPluginBinaryParamHeader);
		numC = hdr->numChar;
		if (numC)
		{
			tag.assign(ptr, numC);
		}

		theExtender->request_report_handle(&rep_ref, tag.c_str());
		if (rep_ref)
		{
			ptr += hdr->numChar;
			switch (hdr->hdr.command)
			{
			case JVX_AUDIOPLUGIN_SEND_NORM_PARAM:

				ptrParam = (jvxAudioPluginBinaryField_sync_norm*)(ptr);
				ptrParamStrLst = NULL;
				for (i = 0; i < hdr->numFields; i++)
				{
					switch (ptrParam->hdr.type_entry)
					{
					case JVX_AUDIOPLUGIN_PARAM_NORMALIZED:
						resL = (jvxErrorType)ptrParam->hdr.err_entry;
						ptrParam = CjvxPluginCommon::decode_norm_param(ptrParam, id, normVal);
						if (resL == JVX_NO_ERROR)
						{
							res = rep_ref->post_single_parameter_norm(id, normVal);
						}
						break;
					case JVX_AUDIOPLUGIN_PARAM_NORM_STRLIST:
						ptrParamStrLst = (jvxAudioPluginBinaryField_sync_norm_strlist*)ptrParam;
						resL = (jvxErrorType)ptrParam->hdr.err_entry;
						ptrParam = (jvxAudioPluginBinaryField_sync_norm*)
							CjvxPluginCommon::decode_norm_strlist_param(ptrParamStrLst, id, normVal, &strl);

						// Modify the string list
						if (resL == JVX_NO_ERROR)
						{
							res = rep_ref->post_single_parameter_norm_strlist(id, normVal, &strl);
						}
						break;
					}
					/*
					if (update_ids->hdr.type_entry == JVX_AUDIOPLUGIN_PARAM_NORMALIZED)
					{
					}
					*/
				}

				if (proc_id > 0)
				{
					// Send a response where required
					jvxAudioPluginBinaryHeader theRequest;
					CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
						&theRequest, -1, proc_id);
					respond->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
				}
				break;
			case JVX_AUDIOPLUGIN_TRIGGER_SYNC_PARAM:

				trigger_ids = (jvxAudioPluginBinaryField_trigger*)(ptr);
				for (i = 0; i < hdr->numFields; i++)
				{
					jvxApiStringList strl;
					jvxData normVal = 0;
					jvxAudioPluginBinaryParamHeader* loc_hdr = nullptr;
					res = rep_ref->request_single_parameter_norm_strlist(trigger_ids[i].id, &normVal, &strl);
					if (res == JVX_NO_ERROR)
					{
						// Send to the other side via message
						CjvxPluginCommon::prepare_controller_processor_message_param_strlist(
							&loc_hdr, trigger_ids[i].id, 
							normVal, res, &strl,
							nullptr, -1, proc_id);
					}
					else
					{
						res = rep_ref->request_single_parameter_norm(trigger_ids[i].id, &normVal);

						// Send to the other side via message
						CjvxPluginCommon::prepare_controller_processor_message_param_norm(
							&loc_hdr, trigger_ids[i].id, 
							normVal, res,
							nullptr, -1, proc_id);
					}

					if (res == JVX_NO_ERROR)
					{
						res = respond->plugin_send_message((jvxAudioPluginBinaryHeader*)loc_hdr);
						CjvxPluginCommon::release_controller_processor_message((jvxAudioPluginBinaryHeader*)loc_hdr);
					}
				}

				if (proc_id > 0)
				{
					// Send a response where required
					jvxAudioPluginBinaryHeader theRequest;
					CjvxPluginCommon::prepare_controller_processor_message_proc_complete(
						&theRequest, -1, proc_id);
					respond->plugin_send_message((jvxAudioPluginBinaryHeader*)&theRequest);
				}
				break;				
			}
			theExtender->return_report_handle(rep_ref);
		}
	}
	return res;
}

// ===============================================================================

jvxErrorType 
CjvxPluginProcessor::disengage_processing()
{
	// Must be in a locked section
	return stop_connection();
}

jvxErrorType 
CjvxPluginProcessor::engage_processing()
{
	// Must be in a locked section
	return start_connection();
}

jvxErrorType
CjvxPluginProcessor::try_lock_processing()
{
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE retVal;
	JVX_TRY_LOCK_MUTEX(retVal, safeAccessProcessing);
	if (retVal == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType
CjvxPluginProcessor::lock_processing()
{
	JVX_LOCK_MUTEX(safeAccessProcessing);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxPluginProcessor::unlock_processing()
{
	JVX_UNLOCK_MUTEX(safeAccessProcessing);
	return JVX_NO_ERROR;

}

// ===========================================================================

