#include "CjvxConsoleHost_be_drivehost.h"
#include "interfaces/all-hosts/jvxHostHookupEntries.h"

#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTThreads.h"
#include "jvxTDataLogger.h"
#include "jvxTSocket.h"
#include "jvxTLogRemoteHandler.h"

#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"
#include "jvxAuTGenericWrapper.h"
#include "jvxTDataConverter.h"
#include "jvxTResampler.h"

JVX_THREAD_ENTRY_FUNCTION(observeProcess, param)
{
	CjvxConsoleHost_be_drivehost* this_pointer = reinterpret_cast<CjvxConsoleHost_be_drivehost*> (param);
	if (this_pointer)
	{
		this_pointer->observeThreadLoop();
	}
	return 0;
}

#ifndef JVX_CONFIGURE_HOST_STATIC_NODE

// Default black listing in case of host that loads dynamic objects
void
CjvxConsoleHost_be_drivehost::myComponentFilterCallback(jvxBool* doNotLoad, const char* module_name, jvxComponentType tp, jvxHandle* priv)
{
	*doNotLoad = false;
	switch (tp)
	{
	case JVX_COMPONENT_HOST:
	case JVX_COMPONENT_CONFIG_PROCESSOR:
	case JVX_COMPONENT_WEBSERVER:
	case JVX_COMPONENT_SYSTEM_TEXT_LOG:
	case JVX_COMPONENT_THREADS:
		*doNotLoad = true;
	}
}

#endif

jvxErrorType 
CjvxConsoleHost_be_drivehost::boot_initialize_specific(jvxApiString* errloc)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	jvxApiString descriptionComponent;
	oneAddedStaticComponent comp;
	IjvxProperties* theProps = NULL;

	/* 
	 * Add some shortcut callbacks
	*/
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	if (theProps)
	{
		theCallback_saveconfig.cb = cb_save_config;
		theCallback_saveconfig.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_saveconfig, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_save_config", callGate);

		theCallback_exchg_property.cb = cb_xchg_property;
		theCallback_exchg_property.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvx_set_property(theProps, &theCallback_exchg_property, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_xchg_property", callGate);

		supports_shutdown = false;
		linkedPriFrontend.fe->query_property(jvxFrontendSupportQueryType::JVX_FRONTEND_READ_SUPPORTS_SHUTDOWN, &supports_shutdown);

		// Shutdown may be controlled by frontend setting and backend command line option!!
		if (supports_shutdown && !config_noquit)
		{
			theCallback_shutdown.cb = cb_shutdown;
			theCallback_shutdown.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_shutdown, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);

			theCallback_restart.cb = cb_restart;
			theCallback_restart.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_restart, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		}
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	}
	theProps = NULL;

	involvedComponents.theHost.hFHost->add_external_interface(
		reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(evLop)), JVX_INTERFACE_EVENTLOOP);

	// =================================================================================================================
	
	// Reserve a space of 20 properties for now
	_common_set_properties.propIdSpan = 20;
	_common_set_min.theState = JVX_STATE_ACTIVE;

	genConsoleHost::init_all();
	genConsoleHost::allocate_all();
	genConsoleHost::register_all(static_cast<CjvxProperties*>(this));
	genConsoleHost::register_callbacks(static_cast<CjvxProperties*>(this), cb_set_options, this, nullptr);

	IjvxPropertyAttach* thePropExp = nullptr;
	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle**)&thePropExp);
	if (thePropExp)
	{
		thePropExp->attach_property_submodule("frontend_hooks", static_cast<IjvxProperties*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle*)thePropExp);
		thePropExp = nullptr;
	}

	IjvxConfigurationAttach* theCfgAtt = nullptr;
	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle**)&theCfgAtt);
	if (theCfgAtt)
	{
		theCfgAtt->attach_configuration_submodule("frontend_hooks", static_cast<IjvxConfiguration*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle*)theCfgAtt);
		theCfgAtt = nullptr;
	}

	// =================================================================================================================

	JVX_START_SLOTS_BASE(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);
#ifndef JVX_NO_SYSTEM_EXTENSIONS
	JVX_START_SLOTS_SUBPRODUCT(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);
#endif

	// Attach backward references
	theHostFeatures.if_report = static_cast<IjvxReport*>(this);

	theHostFeatures.fHost = involvedComponents.theHost.hFHost;
	theHostFeatures.hHost = involvedHost.hHost;

	jvx_configure_factoryhost_features(&theHostFeatures);

#ifndef JVX_CONFIGURE_HOST_STATIC_NODE
	//parentRef->theHostFeatures.activateDefaultAlgorithm = false;
	// theHostFeatures.cb_loadfilter = myComponentFilterCallback;
	// theHostFeatures.cb_loadfilter_priv = NULL;

#else
	//parentRef->theHostFeatures.activateDefaultAlgorithm = true;
	theHostFeatures.cb_loadfilter = NULL;
	theHostFeatures.cb_loadfilter_priv = NULL;
#endif
	boot_initialize_base(theHostFeatures.numSlotsComponents, theHostFeatures.mapSpecSubSlots);

	// Open all tool components
	LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init, jvxTSystemTextLog_terminate, "Text Log", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init, jvxTconfigProcessor_terminate, "Config Parser", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTThreads_init, jvxTThreads_terminate, "Threads", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init, jvxTDataLogger_terminate, "Data Logger", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTSocket_init, jvxTSocket_terminate, "Sockets", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTLogRemoteHandler_init, jvxTDataLogger_terminate, "Remote Log Handler", involvedComponents.addedStaticObjects,involvedComponents.theHost.hFHost);

	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init,
		jvxTDataConverter_terminate,
		"Data Converter",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init,
		jvxTResampler_terminate,
		"Resampler",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init,
		jvxTrtAudioFileReader_terminate,
		"RT Audio Reader",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init,
		jvxTrtAudioFileWriter_terminate,
		"RT Audio Writer",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init,
		jvxAuTGenericWrapper_terminate,
		"Generic Wrapper",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);

	// Do not allow that host components are loaded via DLL
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_MIN_HOST);
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_OFF_HOST);
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_FACTORY_HOST);

	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_INIT)
	{
		TjvxEventLoopElement qel;
		qel.message_id = 0;
		qel.param = involvedComponents.theHost.hFHost;
		qel.paramType = JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF;
		qel.eventType = JVX_EVENTLOOP_EVENT_SELECT;
		qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		qel.autoDeleteOnProcess = false;
		qel.oneHdlBlock = NULL;

		resL = linkedPriFrontend.fe->report_process_event(&qel);
		if (resL == JVX_NO_ERROR)
		{
			linkedPriFrontend.st = JVX_STATE_SELECTED;
		}
		else
		{
			assert(0);
		}
	}

	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_INIT)
		{
			TjvxEventLoopElement qel;
			qel.message_id = 0;
			qel.param = involvedComponents.theHost.hFHost;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_IJVXHOST_REF;
			qel.eventType = JVX_EVENTLOOP_EVENT_SELECT;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.autoDeleteOnProcess = false;
			qel.oneHdlBlock = NULL;

			resL = elm->fe->report_process_event(&qel);
			if (resL == JVX_NO_ERROR)
			{
				elm->st = JVX_STATE_SELECTED;
			}
			else
			{
				assert(0);
			}
		}
	}

	// Run the loop to get the components
	static_load_loop();

	involvedComponents.theHost.hFHost->set_component_load_filter_function(theHostFeatures.cb_loadfilter, theHostFeatures.cb_loadfilter_priv);
	/*
	if (theHostFeatures.automation.if_autoconnect)
	{
		involvedComponents.theHost.hFHost->add_external_interface(theHostFeatures.automation.if_autoconnect, JVX_INTERFACE_AUTO_DATA_CONNECT);
	}
	*/
	return res;
}

jvxErrorType
CjvxConsoleHost_be_drivehost::boot_specify_slots_specific()
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::boot_prepare_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if (config.auto_start)
	{
		std::string command = "act(sequencer, start);";
		TjvxEventLoopElement evelm;
		evelm.origin_fe = this;
		evelm.priv_fe = NULL;
		evelm.target_be = this;
		evelm.priv_be = NULL;

		evelm.param = (jvxHandle*)&command;
		evelm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

		evelm.eventType = JVX_EVENTLOOP_EVENT_TEXT_INPUT;
		evelm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
		evelm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		evelm.delta_t = 0;
		evelm.autoDeleteOnProcess = c_true;
		jvxErrorType resL = evLop->event_schedule(&evelm, NULL, NULL);
	}

	if (!startAppExe.empty())
	{		
		char bufWdir[JVX_MAXSTRING] = { 0 };
		JVX_CREATE_PROCESS_RESULT resP = JVX_CREATE_PROCESS_FAILED;
		JVX_GETCURRENTDIRECTORY(bufWdir, JVX_MAXSTRING);
		std::string cwdStr = bufWdir;
		cmd = cwdStr + JVX_SEPARATOR_DIR + startAppExe;			
		resP = JVX_CREATE_PROCESS_WITH_ARGS(hdlProc, cmd, startAppArgs);
		if (resP == JVX_CREATE_PROCESS_SUCCESS)
		{			
			startAppRunning = true;
			observerThreadRunning = true;
			JVX_CREATE_THREAD(startAppObserveThread, observeProcess, this, startAppObserveThreadId);
		}
	}

	return res;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::boot_start_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::boot_activate_specific(jvxApiString* errloc)
{
	jvxErrorType resL;
	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle**>(&config.hostRefConfigExtender));
	if ((resL == JVX_NO_ERROR) && config.hostRefConfigExtender)
	{
		resL = config.hostRefConfigExtender->register_extension(static_cast<IjvxConfigurationExtender_report*>(this), JVX_COMMON_PROPERTIES_HOST_SECTION_NAME);
	}

	// Inform all frontends about that the main backend has been started
	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_SELECTED)
	{
		TjvxEventLoopElement qel;
		qel.message_id = 0;
		qel.param = NULL;
		qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
		qel.eventType = JVX_EVENTLOOP_EVENT_ACTIVATE;
		qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		qel.autoDeleteOnProcess = false;
		qel.oneHdlBlock = NULL;

		resL = linkedPriFrontend.fe->report_process_event(&qel);
		if (resL == JVX_NO_ERROR)
		{
			linkedPriFrontend.st = JVX_STATE_ACTIVE;
		}
		else
		{
			assert(0);
		}
	}

	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_SELECTED)
		{
			TjvxEventLoopElement qel;
			qel.message_id = 0;
			qel.param = NULL;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
			qel.eventType = JVX_EVENTLOOP_EVENT_ACTIVATE;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.autoDeleteOnProcess = false;
			qel.oneHdlBlock = NULL;

			resL = elm->fe->report_process_event(&qel);
			if (resL == JVX_NO_ERROR)
			{
				elm->st = JVX_STATE_ACTIVE;
			}
			else
			{
				// Startup error code - print on every line
				startupErrorCode = resL;
				std::string token = jvxErrorType_descr(startupErrorCode);
				linestart = token + linestart;
				elm->st = JVX_STATE_ACTIVE;
				// assert(0);
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::shutdown_terminate_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	/*
	if (theHostFeatures.automation.if_autoconnect)
	{
		involvedComponents.theHost.hFHost->remove_external_interface(theHostFeatures.automation.if_autoconnect, JVX_INTERFACE_AUTO_DATA_CONNECT);
	}
	*/
	involvedComponents.theHost.hFHost->set_component_load_filter_function(NULL, NULL);

	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_SELECTED)
		{
			TjvxEventLoopElement qel;
			qel.message_id = 0;
			qel.param = NULL;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
			qel.eventType = JVX_EVENTLOOP_EVENT_UNSELECTED;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.autoDeleteOnProcess = false;
			qel.oneHdlBlock = NULL;

			res = elm->fe->report_process_event(&qel);
			if (res != JVX_NO_ERROR)
			{
				return res;
			}
			elm->st = JVX_STATE_INIT;
		}
	}

	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_SELECTED)
	{
		TjvxEventLoopElement qel;
		qel.message_id = 0;
		qel.param = NULL;
		qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
		qel.eventType = JVX_EVENTLOOP_EVENT_UNSELECTED;
		qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		qel.autoDeleteOnProcess = false;
		qel.oneHdlBlock = NULL;

		linkedPriFrontend.fe->report_process_event(&qel);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
		linkedPriFrontend.st = JVX_STATE_INIT;
	}

	// =========================================================================
	shutdown_specific();
	
	this->involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

	static_unload_loop();

	// Free all host types and data connections
	shutdown_terminate_base();

	// ===============================================================================================

	IjvxConfigurationAttach* theCfgAtt = nullptr;
	jvxErrorType resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle**)&theCfgAtt);
	if (theCfgAtt)
	{
		theCfgAtt->detach_configuration_submodule(static_cast<IjvxConfiguration*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle*)theCfgAtt);
		theCfgAtt = nullptr;
	}


	IjvxPropertyAttach* thePropExp = nullptr;

	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle**)&thePropExp);
	if (thePropExp)
	{
		thePropExp->detach_property_submodule(static_cast<IjvxProperties*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle*)thePropExp);
		thePropExp = nullptr;
	}

	genConsoleHost::unregister_all(static_cast<CjvxProperties*>(this));
	genConsoleHost::deallocate_all();
	_common_set_min.theState = JVX_STATE_NONE;
	_common_set_properties.propIdSpan = JVX_SIZE_UNSELECTED;

	// ===============================================================================================

	jvx_invalidate_factoryhost_features(&theHostFeatures);

	theHostFeatures.if_report = nullptr;
	theHostFeatures.fHost = nullptr;
	theHostFeatures.hHost = nullptr;

	involvedComponents.theHost.hFHost->remove_external_interface(
		reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(evLop)), JVX_INTERFACE_EVENTLOOP);

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::shutdown_deactivate_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string strResponse = "";

	// Inform that the main backend has been shut down - at that time, the host reference becomes invalid
	// The frontends and backends may not be able to shutdown. In that case, the
	// functions return false and await next attempt shortly after
	std::list<jvxOneFrontendAndState>::iterator elm = linkedSecFrontends.begin();
	for (; elm != linkedSecFrontends.end(); elm++)
	{
		if (elm->st == JVX_STATE_ACTIVE)
		{
			TjvxEventLoopElement qel;
			qel.message_id = 0;
			qel.param = NULL;
			qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
			qel.eventType = JVX_EVENTLOOP_EVENT_DEACTIVATE;
			qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
			qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
			qel.autoDeleteOnProcess = false;
			qel.oneHdlBlock = NULL;

			res = elm->fe->report_process_event(&qel);
			if (res != JVX_NO_ERROR)
			{
				return res;
			}
			elm->st = JVX_STATE_SELECTED;
		}
	}

	assert(linkedPriFrontend.fe);
	if (linkedPriFrontend.st == JVX_STATE_ACTIVE)
	{
		TjvxEventLoopElement qel;
		qel.message_id = 0;
		qel.param = NULL;
		qel.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;
		qel.eventType = JVX_EVENTLOOP_EVENT_DEACTIVATE;
		qel.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
		qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
		qel.autoDeleteOnProcess = false;
		qel.oneHdlBlock = NULL;

		linkedPriFrontend.fe->report_process_event(&qel);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
		linkedPriFrontend.st = JVX_STATE_SELECTED;
	}

	if (config.hostRefConfigExtender)
	{
		config.hostRefConfigExtender->unregister_extension(JVX_COMMON_PROPERTIES_HOST_SECTION_NAME);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, reinterpret_cast<jvxHandle*>(config.hostRefConfigExtender));
		config.hostRefConfigExtender = NULL;
	}

	// We need to make sure there is no lingering message and no more messages are no more
	// messages entering the queue. It must be completed before call of terminateHost since
	// the components may request some blocking calls on deactivate!
	//evLop->force_clear_end_event_loop();
	evLop->backend_block(this);
	evLop->event_clear(JVX_SIZE_UNSELECTED, NULL, static_cast<IjvxEventLoop_backend*>(this));

	strResponse = "Shutting down backend..";
	strResponse += "\n";
	outputResult(strResponse, "\n\t\t", '\n', '\t');
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::shutdown_postprocess_specific(jvxApiString* errloc)
{
	if (startAppRunning)
	{
		observerThreadRunning = false;

		// This should trigger the observer thread
		JVX_TERMINATE_PROCESS(hdlProc, 0);

		// Wait for the thread to complete
		JVX_WAIT_FOR_THREAD_TERMINATE_INF(startAppObserveThread);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxConsoleHost_be_drivehost::shutdown_stop_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

void 
CjvxConsoleHost_be_drivehost::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)
{
	std::string txtStr;
	TjvxEventLoopElement qel;
	
	switch (prio)
	{
	case JVX_REPORT_PRIORITY_INFO:
		txtStr = "<INFO> ";
		break;
	case JVX_REPORT_PRIORITY_ERROR:
		txtStr = "<ERROR> ";
		break;
	case JVX_REPORT_PRIORITY_WARNING:
		txtStr = "<WARNING> ";
		break;
	}
	txtStr += txt;

	qel.message_id = 0;
	qel.param = &txtStr;
	qel.target_be = this;
	qel.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;
	qel.eventType = JVX_EVENTLOOP_EVENT_TEXT_SHOW;
	qel.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	qel.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	qel.autoDeleteOnProcess = true;
	qel.oneHdlBlock = NULL;

	evLop->event_schedule(&qel, NULL, NULL);
}

void 
CjvxConsoleHost_be_drivehost::inform_config_read_start(const char* nmfile, jvxHandle* context)
{
	// std::cout << "Starting to read config file " << nmfile << std::endl;
}

void 
CjvxConsoleHost_be_drivehost::inform_config_read_stop(jvxErrorType resRead, const char* nmFile, jvxHandle* context)
{
	if (resRead == JVX_NO_ERROR)
	{
		//std::cout << "Reading of config file " << nmFile << "successful." << std::endl;
	}
	else
	{
		std::cout << "Reading of config file <" << nmFile << "> failed, reason: " << jvxErrorType_descr(resRead) << " (review console output for a detailled description)." << std::endl;
	}
}

void 
CjvxConsoleHost_be_drivehost::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{

}

void
CjvxConsoleHost_be_drivehost::observeThreadLoop()
{
	while (1)
	{
		JVX_WAIT_RESULT resW = JVX_WAIT_FOR_PROCESS_COMPLETE(hdlProc);
		startAppRunning = false;

		if (observerThreadRunning)
		{
			JVX_CREATE_PROCESS_RESULT resP = JVX_CREATE_PROCESS_FAILED;
			std::string cmdLine = cmd;			
			resP = JVX_CREATE_PROCESS_WITH_ARGS(hdlProc, cmd, startAppArgs);
			if (resP == JVX_CREATE_PROCESS_SUCCESS)
			{
				startAppRunning = true;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

