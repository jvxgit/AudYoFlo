#include "CjvxAppFactoryHostBase.h"

// Includes for main host application
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#include "jvxFHDefault_nd.h"
#else
#include "jvxFHDefault.h"
#endif

extern "C"
{

  #define FUNC_CORE_PROT_DECLARE jvx_command_line_specify
  #define FUNC_CORE_PROT_DECLARE_LOCAL jvx_command_line_specify_local
  #define FUNC_CORE_PROT_ARGS IjvxCommandLine* cmdLine
  #define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_command_line_specify=jvx_command_line_specify_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_command_line_specify=_jvx_command_line_specify_local")
#endif

  #include "platform/jvx_platform_weak_defines.h"
     {
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}

  #undef FUNC_CORE_PROT_DECLARE
  #undef FUNC_CORE_PROT_DECLARE_LOCAL
  #undef FUNC_CORE_PROT_ARGS
  #undef FUNC_CORE_PROT_RETURNVAL
  
  #define FUNC_CORE_PROT_DECLARE jvx_command_line_read
  #define FUNC_CORE_PROT_DECLARE_LOCAL jvx_command_line_read_local
  #define FUNC_CORE_PROT_ARGS IjvxCommandLine* cmdLine
  #define FUNC_CORE_PROT_RETURNVAL void

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_command_line_read=jvx_command_line_read_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_command_line_read=_jvx_command_line_read_local")
#endif
  #include "platform/jvx_platform_weak_defines.h"
        {
		 std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
	}
  #undef FUNC_CORE_PROT_DECLARE
  #undef FUNC_CORE_PROT_DECLARE_LOCAL
  #undef FUNC_CORE_PROT_ARGS
  #undef FUNC_CORE_PROT_RETURNVAL

// =======================================================================
// HOST FEATURE WEAK SYMBOL IMPLEMENTATIONS
// =======================================================================
#if 0
#define FUNC_CORE_PROT_DECLARE jvx_configure_factoryhost_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_configure_host_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesA
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_configure_factoryhost_features=jvx_configure_factoryhost_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_configure_factoryhost_features=_jvx_configure_factoryhost_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
		{
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			// Default implementation does just nothing
			return(JVX_NO_ERROR);
		}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL


#define FUNC_CORE_PROT_DECLARE jvx_invalidate_factoryhost_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_invalidate_factoryhost_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesA
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_invalidate_factoryhost_features=jvx_invalidate_factoryhost_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_invalidate_factoryhost_features=_jvx_invalidate_factoryhost_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
		{
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			// Default implementation does just nothing
			return(JVX_NO_ERROR);
		}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL
#endif


#define FUNC_CORE_PROT_DECLARE jvx_access_link_objects
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_access_link_objects_local
#define FUNC_CORE_PROT_ARGS jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* description, jvxComponentType tp, jvxSize id
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_access_link_objects=jvx_access_link_objects_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_access_link_objects=_jvx_access_link_objects_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
		{
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			// Default implementation does just nothing
			return(JVX_ERROR_ELEMENT_NOT_FOUND);
		}

}


// ================================================================
// ================================================================
// ================================================================
// ================================================================

JVX_APP_FACTORY_HOST_CLASSNAME::JVX_APP_FACTORY_HOST_CLASSNAME()
{
	bootState = JVX_STATE_NONE;
	verbose = false;
	report = NULL;
	report_on_config = NULL;
	command_line = NULL;
	boot_steps = NULL;
	resetReferences();
}

JVX_APP_FACTORY_HOST_CLASSNAME::~JVX_APP_FACTORY_HOST_CLASSNAME()
{
}

void
JVX_APP_FACTORY_HOST_CLASSNAME::resetReferences()
{
	involvedComponents.theHost.hobject = NULL;
	involvedComponents.theHost.hFHost = NULL;
	involvedComponents.theTools.hTools = NULL;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::openHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification cpType;
	bool multObj = false;

	// ======================================================
	// Initialize the host component
	// ======================================================
	// Start factory host

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	res = jvxFHDefault_nd_init(&involvedComponents.theHost.hobject, NULL);
#else
	res = jvxFHDefault_init(&involvedComponents.theHost.hobject, NULL);
#endif

	if (res != JVX_NO_ERROR)
	{
		if (errorMessage)
		{
			errorMessage->assign("Unable to open core host lib.");
		}
		return JVX_ERROR_INTERNAL;
	}

	if (verbose == true)
	{
		// Report progress
		report_text_output("Successfully openened host library", JVX_REPORT_PRIORITY_VERBOSE, NULL);
	}

	// ======================================================
	// Get pointer access to host object
	// ======================================================
	involvedComponents.theHost.hobject->request_specialization((jvxHandle**)&involvedComponents.theHost.hFHost,
		&cpType, &multObj);
	assert(involvedComponents.theHost.hFHost);

	// ======================================================
	// Check component type of specialization
	// ======================================================
	assert(cpType.tp == JVX_COMPONENT_FACTORY_HOST);

	// ==============================================================================
	return res;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::closeHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	jvxFHDefault_nd_terminate(this->involvedComponents.theHost.hobject);
#else
	jvxFHDefault_terminate(this->involvedComponents.theHost.hobject);
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdownHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
	return JVX_NO_ERROR;
}

// ==============================================================================

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::boot_configure(jvxApiString* errorMessage, IjvxCommandLine* commLine, 
	IjvxReport* report_ptr, IjvxReportOnConfig* report_on_config_ptr, IjvxReportStateSwitch* report_on_state_switch_ptr, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxApiString errTxt;
	jvxSize errId = 0;
	jvxApiString opt_text;
	jvxSize opt_sz = 0;
	jvxSize i;
	std::string errTxtLoc;

	if (bootState == JVX_STATE_NONE)
	{
		resetReferences();
		bootState = JVX_STATE_INIT;
		report = report_ptr;
		report_on_config = report_on_config_ptr;
		report_on_state_switch = report_on_state_switch_ptr;
		command_line = commLine;
		// Set default values
		_command_line_parameters.configFilename = "";
		_command_line_parameters.configFilename_ovlay = "";
		_command_line_parameters.changeWorkDir = "";
		_command_line_parameters.out_cout = true;
		_command_line_parameters.verbose_dll = false;
		_command_line_parameters.verbose_slots = false;
		_command_line_parameters.textLog_filename = ".tlog";
		_command_line_parameters.textLog_expressions.clear();
		_command_line_parameters.textLog_activate = false;
		_command_line_parameters.textLog_sizeInternBufferFile = 8192;
		_command_line_parameters.textLog_sizeInternBufferRW = 8192;
		_command_line_parameters.textLog_dbglevel = 0;
		_command_line_parameters.dllsDirectories.clear();
		_command_line_parameters.configDirectories.clear();
		_command_line_parameters.systemPath = "";
		_command_line_parameters.userPath = "";

		init_command_line_parameters(commLine);

		if (commLine)
		{
			commLine->register_option("--config", "-c", "Specify configuration file", "", true, JVX_DATAFORMAT_STRING);
			commLine->register_option("--configo", "-co", "Specify configuration overlay file", "", true, JVX_DATAFORMAT_STRING);
			commLine->register_option("--workdir", "-w", "Specify working directy to change to before initialization", "", true, JVX_DATAFORMAT_STRING);
			commLine->register_option("--jvxdir", "-d", "Specify directy from which to load dynamic components", "", true, JVX_DATAFORMAT_STRING, JVX_SIZE_UNSELECTED);
			commLine->register_option("--configdir", "-cd", "Specify directory where to look for configuration file fragments", "", true, JVX_DATAFORMAT_STRING, JVX_SIZE_UNSELECTED);
			commLine->register_option("--systemdir", "-d", "Specify directy from which to load other system parts", "", true, JVX_DATAFORMAT_STRING);
			commLine->register_option("--userdir", "-cd", "Specify directory where to look for user components", "", true, JVX_DATAFORMAT_STRING);

			register_command_line_parameters(commLine);

			commLine->register_option("--n_out_cout", "", "Activate activation to cout");
			commLine->register_option("--verbose_dll", "", "Activate verbose output when loading dynamic component libraries");
			commLine->register_option("--verbose_slots", "", "Activate verbose output when setting up slots");

			commLine->register_option("--textlogfilename", "", "Specify name of global log file", ".tlog", true, JVX_DATAFORMAT_STRING);
			commLine->register_option("--textlog", "", "Activate logging to global log file");
			commLine->register_option("--textlogszfile", "", "Specify internal buffersize for text log file [in Bytes]", "8192", true, JVX_DATAFORMAT_SIZE);
			commLine->register_option("--textlogszrw", "", "Specify size of chunks to be written to text log file [in Bytes]", "8192", true, JVX_DATAFORMAT_SIZE);
			commLine->register_option("--textloglev", "", "Specify log level [0: no log, 10: all log file entries]", "0", true, JVX_DATAFORMAT_SIZE);
			commLine->register_option("--textlogexpr", "", "Specify log level expression to limit log file output to specific modules", "", true, JVX_DATAFORMAT_STRING, JVX_SIZE_UNSELECTED);
			
			jvx_command_line_specify(commLine);

			res = commLine->parse_command_line(&errId, &errTxt, true);
			if (res != JVX_NO_ERROR)
			{
				jvxApiString retStr;
				jvxSize num;
				commLine->arg_app_name(&retStr);
				errTxtLoc += retStr.std_str();
				commLine->number_args(&num);
				for (i = 0; i < num; i++)
				{
					retStr.clear();
					commLine->single_arg_index(i, &retStr);
					errTxtLoc += " ";
					errTxtLoc += retStr.std_str();
				}
				errTxtLoc += "\n";
				errTxtLoc += "Error parsing command line options: entry ";
				errTxtLoc += jvx_size2String(errId);
				errTxtLoc += " is incorrect, reason: ";
				errTxtLoc += errTxt.std_str();
				errTxtLoc += "\n";
				retStr.clear();
				commLine->printUsage(&retStr);
				errTxtLoc += retStr.std_str();
				if (errorMessage)
				{
					errorMessage->assign(errTxtLoc);
				}
			}
			else
			{
				opt_text.clear();
				resL = commLine->content_entry_option("--config", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.configFilename = opt_text.std_str();
				}

				opt_text.clear();
				resL = commLine->content_entry_option("--configo", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.configFilename_ovlay = opt_text.std_str();
				}

				opt_text.clear();
				resL = commLine->content_entry_option("--workdir", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.changeWorkDir = opt_text.std_str();
				}

				jvxSize numEntries = 0;
				resL = commLine->number_entries_option("--jvxdir", &numEntries);
				for (i = 0; i < numEntries; i++)
				{
					opt_text.clear();
					resL = commLine->content_entry_option("--jvxdir", i, &opt_text, JVX_DATAFORMAT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						std::string txt = opt_text.std_str();
						if (!txt.empty())
						{
							_command_line_parameters.dllsDirectories.push_back(txt);
						}
					}
				}

				numEntries = 0;
				resL = commLine->number_entries_option("--configdir", &numEntries);
				for (i = 0; i < numEntries; i++)
				{
					opt_text.clear();
					resL = commLine->content_entry_option("--configdir", i, &opt_text, JVX_DATAFORMAT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						std::string txt = opt_text.std_str();
						if (!txt.empty())
						{
							_command_line_parameters.configDirectories.push_back(txt);
						}
					}
				}
				if (_command_line_parameters.configDirectories.empty())
				{
					_command_line_parameters.configDirectories.push_back(".");
				}

				opt_text.clear();
				resL = commLine->content_entry_option("--systemdir", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.systemPath = opt_text.std_str();
				}

				opt_text.clear();
				resL = commLine->content_entry_option("--userdir", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.userPath = opt_text.std_str();
				}
				
				read_command_line_parameters(commLine);

				numEntries = 0;
				resL = commLine->number_entries_option("--n_out_cout", &numEntries);
				if (numEntries)
				{
					_command_line_parameters.out_cout = false;
				}

				numEntries = 0;
				resL = commLine->number_entries_option("--verbose_dll", &numEntries);
				if (numEntries)
				{
					_command_line_parameters.verbose_dll = true;
				}

				numEntries = 0;
				resL = commLine->number_entries_option("--verbose_slots", &numEntries);
				if (numEntries)
				{
					_command_line_parameters.verbose_slots = true;
				}

				opt_text.clear();
				resL = commLine->content_entry_option("--textlogfilename", 0, &opt_text, JVX_DATAFORMAT_STRING);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.textLog_filename = opt_text.std_str();
				}

				numEntries = 0;
				resL = commLine->number_entries_option("--textlog", &numEntries);
				if (numEntries)
				{
					_command_line_parameters.textLog_activate = true;
				}

				opt_sz = 0;
				resL = commLine->content_entry_option("--textlogszfile", 0, &opt_sz, JVX_DATAFORMAT_SIZE);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.textLog_sizeInternBufferFile = opt_sz;
				}

				opt_sz = 0;
				resL = commLine->content_entry_option("--textlogszrw", 0, &opt_sz, JVX_DATAFORMAT_SIZE);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.textLog_sizeInternBufferRW = opt_sz;
				}

				opt_sz = 0;
				resL = commLine->content_entry_option("--textloglev", 0, &opt_sz, JVX_DATAFORMAT_SIZE);
				if (resL == JVX_NO_ERROR)
				{
					_command_line_parameters.textLog_dbglevel = opt_sz;
				}

				numEntries = 0;
				opt_text.clear();
				resL = commLine->number_entries_option("--textlogexpr", &numEntries);
				for (i = 0; i < numEntries; i++)
				{
					resL = commLine->content_entry_option("--textlogexpr", i, &opt_text, JVX_DATAFORMAT_STRING);
					if (resL == JVX_NO_ERROR)
					{
						_command_line_parameters.textLog_expressions.push_back(opt_text.std_str());
					}
				}
				jvx_command_line_read(commLine);
			}
		}

		// It is a valid option not to specify a command line object
		return res;
	}
	if (bootState == JVX_STATE_INIT)
	{
		// The module had been configured before
		return JVX_NO_ERROR;
	}
	else
	{

		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_NONE);
		txt += ">.";
		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_unconfigure(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	if (bootState == JVX_STATE_INIT)
	{
		bootState = JVX_STATE_NONE;
	}
	return JVX_NO_ERROR;
}

/*
jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_dropconfig(jvxApiString* errorMessage, jvxHandle* context)
{
	if (bootState == JVX_STATE_INIT)
	{
		resetReferences();
		report = NULL;
		report_on_config = NULL;
		if (command_line)
		{
			command_line->reset_command_line_content();
			command_line = NULL;
		}

		bootState = JVX_STATE_NONE;
		return JVX_NO_ERROR;
	}
	else
	{

		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_INIT);
		txt += ">.";
		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}
*/
// ==============================================================================================

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::boot_initialize(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxProperties* theProps = NULL;
	jvxBool isValid = false;
	jvxSize id = 0;
	jvxCBool cout_mess = c_false;
	jvxCBool verbose_dll = c_false;
	jvxCBool textLog_activate = c_false;
	jvxApiString theStr;
	jvxApiString errLoc;
	jvxApiStringList theStrLst;
	jvxCallManagerProperties callGate;
	if (bootState == JVX_STATE_INIT)
	{
		// ======================================================
		// Set all pointer references to contain NULL pointer values
		// ======================================================
		resetReferences();

		res = openHostFactory(errorMessage, context);
		if (res != JVX_NO_ERROR)
		{
			return res;
		}

		// ======================================================
		// Set host to point to desired working dir
		// ======================================================
		if (!_command_line_parameters.changeWorkDir.empty())
		{
			std::cout << "#### Changing working directory to <" << _command_line_parameters.changeWorkDir << ">" << std::endl;
			JVX_CHDIR(_command_line_parameters.changeWorkDir.c_str());
		}
		else
		{
			std::cout << "#### Working directory is empty, using active value." << std::endl;
		}
		// ======================================================
		// Initialize the host..
		// ======================================================
		involvedComponents.theHost.hFHost->initialize(NULL);
		involvedComponents.theHost.hFHost->select();

		// ======================================================
		// Set the predefined property for dll path for subcomponents
		// ======================================================
		if (_command_line_parameters.out_cout)
		{
			cout_mess = c_true;
		}
		if (_command_line_parameters.verbose_dll)
		{
			verbose_dll = c_true;
		}

		if (_command_line_parameters.textLog_activate)
		{
			textLog_activate = c_true;
		}
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);

		assert((res == JVX_NO_ERROR) && theProps);

		// Set the predefined property for dll path for subcomponents
		if (jvx_findPropertyDescriptor("jvx_component_path_str_list", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyStringList(_command_line_parameters.dllsDirectories, 
				involvedComponents.theHost.hobject, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}

		// Set the predefined property for dll path for subcomponents
		if (jvx_findPropertyDescriptor("jvx_config_path_str_list", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyStringList(_command_line_parameters.configDirectories,
				involvedComponents.theHost.hobject, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}

		// Set the predefined property for dll path for subcomponents
		if (jvx_findPropertyDescriptor("jvx_system_path_str", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyString(_command_line_parameters.systemPath,
				involvedComponents.theHost.hobject, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}

		if (jvx_findPropertyDescriptor("jvx_user_path_str", &id, NULL, NULL, NULL))
		{
			res = jvx_pushPullPropertyString(_command_line_parameters.userPath,
				involvedComponents.theHost.hobject, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, callGate);
		}
		
		res = jvx_set_property(theProps, &cout_mess, 0, 1, JVX_DATAFORMAT_BOOL, true, "/host_output_cout", callGate);
		assert(res == JVX_NO_ERROR);

		res = jvx_set_property(theProps, &verbose_dll, 0, 1, JVX_DATAFORMAT_BOOL, true, "/host_verbose_dll", callGate);
		assert(res == JVX_NO_ERROR);

		theStr.assign_const(_command_line_parameters.textLog_filename.c_str(), _command_line_parameters.textLog_filename.size());
		res = jvx_set_property(theProps, &theStr, 0, 1, JVX_DATAFORMAT_STRING, false, "/textLog_filename", callGate);
		assert(res == JVX_NO_ERROR);

		res = jvx_set_property(theProps, &_command_line_parameters.textLog_dbglevel, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_dbglevel", callGate);
		assert(res == JVX_NO_ERROR);

		res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferFile", callGate);
		assert(res == JVX_NO_ERROR);

		_command_line_parameters.textLog_sizeInternBufferFile = _command_line_parameters.textLog_sizeInternBufferFile >> 3;
		res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeTransferFile", callGate);
		assert(res == JVX_NO_ERROR);

		res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferRW, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferRW", callGate);
		assert(res == JVX_NO_ERROR);

		theStrLst.assign(_command_line_parameters.textLog_expressions);
		res = jvx_set_property(theProps, &theStrLst, 0, 1, JVX_DATAFORMAT_STRING_LIST, false, "/textLog_expressions", callGate);
		assert(res == JVX_NO_ERROR);

		res = jvx_set_property(theProps, &textLog_activate, 0, 1, JVX_DATAFORMAT_BOOL, true, "/textLog_activate", callGate);
		assert(res == JVX_NO_ERROR);

		res = involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);

		// Set cross references such that all report callbacks end up in this class
		involvedComponents.theHost.hFHost->set_external_report_target(report);
		involvedComponents.theHost.hFHost->set_external_report_on_config(report_on_config);
		involvedComponents.theHost.hFHost->add_external_report_state_switch(report_on_state_switch, "application");
		involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(command_line), JVX_INTERFACE_COMMAND_LINE);
		if (boot_steps)
		{
			involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(boot_steps), JVX_INTERFACE_BOOT_STEPS);
		}		

		res = boot_initialize_specific(&errLoc);
		if (res == JVX_NO_ERROR)
		{
			bootState = JVX_STATE_SELECTED;
		}
		else
		{
			if (errorMessage)
			{
				errorMessage->assign("Specific <boot_initialize> was not successful: " + errLoc.std_str());
			}
		}

		return res;
	}
	else
	{
		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_INIT);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_terminate(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	if (bootState > JVX_STATE_SELECTED)
	{
		res = shutdown_deactivate(&errLoc, context);
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += "shutdown_deactivate";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}
	}

	if (bootState == JVX_STATE_SELECTED)
	{
		res = shutdown_terminate_specific(&errLoc);
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += __FUNCTION__;
				txt += "_specific";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}

		// We changed the order: components may unregister tools on deactivate!
		if (involvedComponents.theTools.hTools)
		{
			involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(involvedComponents.theTools.hTools));
			involvedComponents.theTools.hTools = NULL;
		}

		if (boot_steps)
		{
			involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(boot_steps), JVX_INTERFACE_BOOT_STEPS);
		}
		involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(command_line), JVX_INTERFACE_COMMAND_LINE);
		involvedComponents.theHost.hFHost->remove_external_report_state_switch(report_on_state_switch);
		command_line->reset_command_line_content();
		command_line->reset_command_line();

		this->involvedComponents.theHost.hFHost->unselect();
		this->involvedComponents.theHost.hFHost->terminate();

		this->closeHostFactory(errorMessage, context);

		this->involvedComponents.theHost.hobject = NULL;
		this->involvedComponents.theHost.hFHost = NULL;

		bootState = JVX_STATE_INIT;
		// bootState = JVX_STATE_NONE; // <- complete shutdown to run "boot_configure" a second time! Requires shutdown_unconfigure now!
		return JVX_NO_ERROR;
	}
	
	// Host is already in correct state
	/*
	std::string txt = "Module is in wrong state, state is <";
	txt += jvxState_txt(bootState);
	txt += "> whereas it is expected to be <";
	txt += jvxState_txt(JVX_STATE_SELECTED);
	txt += ">.";

	errorMessage->assign(txt);
	*/
	return JVX_NO_ERROR;
}

// =====================================================================================================
// ACTIVATE / DEACTIVATE ACTIVATE / DEACTIVATE ACTIVATE / DEACTIVATE ACTIVATE / DEACTIVATE ACTIVATE / DEACTIVATE
// =====================================================================================================

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::boot_activate(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	// =================================================================
	// Activate the host
	// =================================================================
	if (bootState == JVX_STATE_SELECTED)
	{
		res = involvedComponents.theHost.hFHost->activate();
		if (res == JVX_NO_ERROR)
		{
			bootState = JVX_STATE_ACTIVE;
		}
		else
		{
			if (errorMessage)
			{
				errorMessage->assign("<boot_activate> was not successful: " + errLoc.std_str());
			}
			return res;
		}

		// Tools interfaces
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&involvedComponents.theTools.hTools));
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				errorMessage->assign("Failed to obtain tools host reference: " + errLoc.std_str());
			}
			return res;
		}

		res = boot_activate_specific(&errLoc); // postbootup_specific();
		if (res == JVX_NO_ERROR)
		{
			bootState = JVX_STATE_ACTIVE;
		}
		else
		{
			if (errorMessage)
			{
				errorMessage->assign("Specific >boot_activate> was not successful: " + errLoc.std_str());
			}
		}
		return res;
	}
	else
	{
		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_SELECTED);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_deactivate(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	if (bootState > JVX_STATE_ACTIVE)
	{
		res = shutdown_postprocess(&errLoc, NULL);
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += "shutdown_postprocess";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}
	}

	if (bootState == JVX_STATE_ACTIVE)
	{
		res = shutdown_deactivate_specific(&errLoc); // postbootup_specific();
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += __FUNCTION__;
				txt += "_specific";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}

		// The deactivate function in the host checks that no component is left unterminated
		res = this->involvedComponents.theHost.hFHost->deactivate();
		bootState = JVX_STATE_SELECTED;
		return res;
	}
	else
	{

		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_ACTIVE);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

// =====================================================================================================
// PREPARE / POSTPROCESS PREPARE / POSTPROCESS PREPARE / POSTPROCESS PREPARE / POSTPROCESS PREPARE / POSTPROCESS
// =====================================================================================================
jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::boot_prepare(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxApiString errLoc;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// If config file was specified,
	jvxCallManagerConfiguration callConf;

	if (bootState == JVX_STATE_ACTIVE)
	{
		res = boot_specify_slots_specific(); // postbootup_specific();
		assert(res == JVX_NO_ERROR);

		boot_prepare_host_start();

		// Read and confirm current working directory
		char wdir[JVX_MAXSTRING+1];
		memset(wdir, 0, JVX_MAXSTRING + 1);
		JVX_GETCURRENTDIRECTORY(wdir, JVX_MAXSTRING);
		std::cout << "### Current working directory: " << wdir << " ###" << std::endl;

		// ============================================================================================

		if (!_command_line_parameters.configFilename.empty())
		{
			// At first try to read config file from user path
			cfg_filename_in_use = jvx_makeFilePath(_command_line_parameters.userPath, _command_line_parameters.configFilename);
			callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
			resL = configureFromFile(&callConf, cfg_filename_in_use, _command_line_parameters.configDirectories, context);
			if (resL != JVX_NO_ERROR)
			{
				// Second try to read file from system path
				std::string cfg_filename_from_sys = jvx_makeFilePath(_command_line_parameters.systemPath, _command_line_parameters.configFilename);
				resL = configureFromFile(&callConf, cfg_filename_from_sys, _command_line_parameters.configDirectories, context);
			}		
		}		

		if (!_command_line_parameters.configFilename_ovlay.empty())
		{
			cfg_filename_olay_in_use = jvx_makeFilePath(_command_line_parameters.userPath, _command_line_parameters.configFilename_ovlay);
			callConf.configModeFlags = JVX_CONFIG_MODE_OVERLAY_READ;			
			resL = configureFromFile(&callConf, cfg_filename_olay_in_use, _command_line_parameters.configDirectories, context);
			if(resL != JVX_NO_ERROR)
			{
				std::string cfg_filename_from_sys = jvx_makeFilePath(_command_line_parameters.systemPath, _command_line_parameters.configFilename);
				resL = configureFromFile(&callConf, cfg_filename_from_sys, _command_line_parameters.configDirectories, context);
			}
		}

		// Configuration complete will be reported even if the configuration file does not exist
		configureComplete();

		// ============================================================================================
		
		// Here, we run the default component activation with the call to systemReady - which we may need to use as the last step before system is up
		boot_prepare_host_stop();
		
		res = boot_prepare_specific(&errLoc); // postbootup_specific();
		if (res == JVX_NO_ERROR)
		{
			bootState = JVX_STATE_PREPARED;			
		}
		else
		{
			if (errorMessage)
			{
				errorMessage->assign("Specific <boot_run> was not successful: " + errLoc.std_str());
			}
		}
		return res;
	}
	else
	{
		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_ACTIVE);
		txt += ">.";
		if (errorMessage)
		{
			errorMessage->assign(txt);
		}
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_postprocess(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	jvxState theState = JVX_STATE_INIT;

	if (bootState > JVX_STATE_PREPARED)
	{
		res = shutdown_stop(&errLoc, context); // shutdown_specific();
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += "shutdown_stop";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}
	}

	if (bootState == JVX_STATE_PREPARED)
	{
		// Return the section entry file association
		if (lstSectionsFile)
		{
			jvxComponentIdentification tpCfg(JVX_COMPONENT_CONFIG_PROCESSOR, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
			IjvxObject* obj = NULL;
			IjvxConfigProcessor* cfgProc = NULL;
			this->involvedComponents.theTools.hTools->reference_tool(tpCfg, &obj, 0, NULL);
			if (obj)
			{
				obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
				if (cfgProc)
				{
					cfgProc->release_section_origin_list(lstSectionsFile);
				}
				this->involvedComponents.theTools.hTools->return_reference_tool(tpCfg, obj);
			}
		}

		shutdown_postprocess_host();

		res = shutdown_postprocess_specific(&errLoc); // shutdown_specific();
		if (res != JVX_NO_ERROR)
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += __FUNCTION__;
				txt += "_specific";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}
		
		(static_cast<IjvxObject*>(this->involvedComponents.theHost.hFHost))->state(&theState);
		//this->involvedComponents.theHost.hFHost->state(&theState);

		assert(theState > JVX_STATE_INIT);

		shutdownHostFactory(errorMessage, context);

		bootState = JVX_STATE_ACTIVE;

		return res;
	}
	else
	{

		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += "> whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_PREPARED);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

// =====================================================================================================
// START / STOP START / STOP START / STOP START / STOP START / STOP START / STOP START / STOP START / STOP
// =====================================================================================================

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::boot_start(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;

	if (bootState == JVX_STATE_PREPARED)
	{

		res = boot_start_specific(&errLoc); // postbootup_specific();
		if (res == JVX_NO_ERROR)
		{
			bootState = JVX_STATE_PROCESSING;
		}
		else
		{
			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += __FUNCTION__;
				txt += "_specific";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
		}
		return res;
	}
	else
	{
		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_PREPARED);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_stop(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errLoc;
	if (bootState == JVX_STATE_PROCESSING)
	{
		res = shutdown_stop_specific(&errLoc); // postbootup_specific();
		if (res != JVX_NO_ERROR)
		{

			if (errorMessage)
			{
				std::string txt = "Function <";
				txt += __FUNCTION__;
				txt += "_specific";
				txt += "> was not successful: ";
				txt += errLoc.std_str();
				txt += ".";
				errorMessage->assign(txt);
			}
			return res;
		}
		bootState = JVX_STATE_PREPARED;
		return res;

	}
	else
	{

		std::string txt = "Module is in wrong state, state is <";
		txt += jvxState_txt(bootState);
		txt += " whereas it is expected to be <";
		txt += jvxState_txt(JVX_STATE_PROCESSING);
		txt += ">.";

		errorMessage->assign(txt);
	}
	return JVX_ERROR_WRONG_STATE;
}

void 
JVX_APP_FACTORY_HOST_CLASSNAME::init_command_line_parameters(IjvxCommandLine* commLine)
{
}

void
JVX_APP_FACTORY_HOST_CLASSNAME::register_command_line_parameters(IjvxCommandLine* commLine)
{
}

void
JVX_APP_FACTORY_HOST_CLASSNAME::read_command_line_parameters(IjvxCommandLine* commLine)
{
}

void 
JVX_APP_FACTORY_HOST_CLASSNAME::boot_prepare_host_start()
{
}

void 
JVX_APP_FACTORY_HOST_CLASSNAME::boot_prepare_host_stop()
{
}

void 
JVX_APP_FACTORY_HOST_CLASSNAME::shutdown_postprocess_host()
{
}

void
JVX_APP_FACTORY_HOST_CLASSNAME::static_load_loop()
{
	oneAddedStaticComponent comp;
	jvxApiString descriptionComponent;
	jvxSize i, cnt;
	jvxErrorType resL = JVX_NO_ERROR;
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		cnt = 0;
		while (1)
		{
			//memset(descriptionComponent, 0, JVX_MAXSTRING);
			comp.reset();
			resL = jvx_access_link_objects(&comp.funcInit,
				&comp.funcTerm, &descriptionComponent,
				(jvxComponentType)i, cnt);
			if (resL == JVX_NO_ERROR)
			{
				jvxComponentIdentification cpTpId;
				comp.theStaticObject = NULL;
				resL = comp.funcInit(&comp.theStaticObject, &comp.theStaticGlobal, NULL);
				assert(resL == JVX_NO_ERROR);
				assert(comp.theStaticObject);

				comp.theStaticObject->request_specialization(nullptr, &cpTpId, nullptr);
				if (cpTpId.tp == JVX_COMPONENT_PACKAGE)
				{
					oneAddedStaticComponent compFromPackage;
					jvxApiString descriptionComponentPackage;
					IjvxPackage* thePack = reqInterfaceObj<IjvxPackage>(comp.theStaticObject);
					if (thePack)
					{
						jvxSize numCompsPackage = 0;
						thePack->number_components(&numCompsPackage);
						for (jvxSize cpi = 0; cpi < numCompsPackage; cpi++)
						{
							resL = thePack->request_entries_component(cpi, &descriptionComponentPackage,
								&compFromPackage.funcInit, &compFromPackage.funcTerm);
							if (resL == JVX_NO_ERROR)
							{
								compFromPackage.theStaticObject = NULL;
								compFromPackage.packageRef = comp.theStaticObject;

								resL = compFromPackage.funcInit(&compFromPackage.theStaticObject, &compFromPackage.theStaticGlobal, NULL);
								assert(resL == JVX_NO_ERROR);
								assert(compFromPackage.theStaticObject);

								resL = involvedComponents.theHost.hFHost->add_external_component(
									compFromPackage.theStaticObject, compFromPackage.theStaticGlobal,
									descriptionComponentPackage.c_str(), true, compFromPackage.funcInit, compFromPackage.funcTerm);
								if (resL == JVX_NO_ERROR)
								{
									involvedComponents.addedStaticObjects.push_back(compFromPackage);
								}
								else
								{
									compFromPackage.funcTerm(compFromPackage.theStaticObject);
								}
							}
						}
					}
				}

				resL = involvedComponents.theHost.hFHost->add_external_component(comp.theStaticObject, comp.theStaticGlobal,
					descriptionComponent.c_str(), true, comp.funcInit, comp.funcTerm);
				if (resL == JVX_NO_ERROR)
				{
					involvedComponents.addedStaticObjects.push_back(comp);
				}
				else
				{
					comp.funcTerm(comp.theStaticObject);
				}
			}
			else
			{
				break;
			}
			cnt++;
		}
	}
}

void
JVX_APP_FACTORY_HOST_CLASSNAME::static_unload_loop()
{
	jvxSize i;
	// Remove all library objects
	for (i = 0; i < involvedComponents.addedStaticObjects.size(); i++)
	{
		if (involvedComponents.addedStaticObjects[i].packageRef == nullptr)
		{
			UNLOAD_ONE_MODULE_LIB_FULL(involvedComponents.addedStaticObjects[i], involvedComponents.theHost.hFHost);
		}
		else
		{
			// This is an object which comes from a package. At the moment, we do the same action, however.
			UNLOAD_ONE_MODULE_LIB_FULL(involvedComponents.addedStaticObjects[i], involvedComponents.theHost.hFHost);
		}
	}
	involvedComponents.addedStaticObjects.clear();
}
