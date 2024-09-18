#include "jvxLibHost.h"
#include "jvx-helpers.h"
#include "jvxHost_config.h"

// ========================================================================
// ========================================================================

extern void
jvx_deriveRunArgsJvx(std::vector<std::string>& lst, const std::string& argv0);

// ========================================================================
// ========================================================================

jvxLibHost::jvxLibHost(): CjvxProperties("jvxLibHost", *this)
{
	JVX_INITIALIZE_MUTEX(safeAccessEngineThreads);
	JVX_INITIALIZE_MUTEX(safeAccessSubReports);
	confHostFeatures = static_cast<configureHost_features*>(&theHostFeatures);
}

jvxLibHost::~jvxLibHost()
{
	JVX_TERMINATE_MUTEX(safeAccessEngineThreads);
	JVX_TERMINATE_MUTEX(safeAccessSubReports);
}

// ========================================================================
// ========================================================================

jvxErrorType
jvxLibHost::initSystem(const char* argv[] , int argc, callbacks_capi* cbks, bool executableInBinFolder)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	char** newArgv = NULL;
	int newArgc = 0;
	jvxBool generatedArgs = false;
	std::string exec_name = "noName";
	std::vector<std::string> args;
	if (synchronize_thread_enter(true))
	{
		if (init_done == false)
		{
			reqHandle.fwdRequestAllowed = true;
			if (cbks)
			{
				cbks_api = *cbks;
			}

			if (argc >= 1)
			{
				exec_name = argv[0];
				for (i = 1; i < argc; i++)
				{
					args.push_back(argv[i]);
				}
				if (argc == 1)
				{
					// No specified arguments
					jvx_deriveRunArgsJvx(args, exec_name, executableInBinFolder); // <- in flutter standard installation, the executable is not in a subfolder
					jvx_appArgs(newArgv, newArgc, args);
					generatedArgs = true;
				}
			}

			/*
			char buf[1024] = { 0 };
			JVX_GETCURRENTDIRECTORY(buf, 1024);
			*/

			jvxConfigSectionTypes myCfgTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
			jvxApiString errMess;

			/* Start splash screen if desired */

			IjvxConfigurationExtender* cfgExt = NULL;
			if (generatedArgs)
			{
				commLine.assign_args((const char**)newArgv, newArgc, exec_name.c_str());
			}
			else
			{
				commLine.assign_args((const char**)argv, argc, exec_name.c_str());
			}

			/* Update splash screen  */
			// "Starting audio host ...

			/* Update splash screen  */
			// "Configuring audio host ..."


			res = boot_configure(&errMess,
				static_cast<IjvxCommandLine*>(&commLine),
				static_cast<IjvxReport*>(this),
				static_cast<IjvxReportOnConfig*>(this),
				static_cast<IjvxReportStateSwitch*>(this),
				NULL);
			if (res != JVX_NO_ERROR)
			{
				// this->fatalStop("Fatal Error", errMess.c_str());
			}

			/* Update splash screen  */
			// "Initializing audio host ..."

			res = boot_initialize(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				// this->fatalStop("Fatal Error", errMess.c_str());
			}

			/* Update splash screen  */
			// "Activating audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);

			res = boot_activate(&errMess, NULL);
			if (res != JVX_NO_ERROR)
			{
				// this->fatalStop("Fatal Error", errMess.c_str());
			}

			/* Update splash screen  */
			// "Preparing audio host ...", Qt::AlignHCenter, JVX_QT_SPLASH_TEXT_COLOR);

			res = boot_prepare(&errMess, nullptr);
			if (res != JVX_NO_ERROR)
			{
				// this->fatalStop("Fatal Error", errMess.c_str());
			}

			/*
			theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
				static_cast<IjvxMainWindowController_report*>(this),
				reinterpret_cast<jvxHandle*>(this),
				"Timeout Period Sequencer [ms]",
				&id_period_seq_ms,
				false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);

			theControl.register_functional_callback(JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_INT_VALUE,
				static_cast<IjvxMainWindowController_report*>(this),
				reinterpret_cast<jvxHandle*>(this),
				"Timeout Period Main Central [ms]",
				&id_rtview_period_maincentral_ms,
				false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);
			theControl.register_functional_callback(
				JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
				static_cast<IjvxMainWindowController_report*>(this),
				reinterpret_cast<jvxHandle*>(this),
				"Auto Start",
				&id_autostart_id,
				false, JVX_REGISTER_FUNCTION_MENU_SEPARATOR_IF_NOT_FIRST);
			theControl.register_functional_callback(
				JVX_MAINWINDOWCONTROLLER_REGISTER_MENU_ITEM_ONOFF,
				static_cast<IjvxMainWindowController_report*>(this),
				reinterpret_cast<jvxHandle*>(this),
				"Auto Stop",
				&id_autostop_id,
				false, JVX_REGISTER_FUNCTION_MENU_NO_SEPARATOR);
			*/

			// Trigger boot Delayed callback (to leave constructor)
			// emit emit_bootDelayed();

			/* jvx_stop_splash_qt(spl, this); */

			if (res == JVX_NO_ERROR)
			{
				mainThreadId = JVX_GET_CURRENT_THREAD_ID();
				init_done = true;

				allocate_json_show_handle();
				if (showHandle)
				{
					// std::string command = "show(system)";
					// this->run_direct_show_command(command);
				}
			}

			if (generatedArgs)
			{
				delete[](newArgv);
				newArgc = 0;
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		synchronize_thread_leave(true);
	}

	return res;
}

jvxErrorType 
jvxLibHost::terminateSystem()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString errMess;

	if (synchronize_thread_enter())
	{
		// We decouple the asyn messages here - no more messages to be transmitted as we need to shutdown in "one step"
		// reqHandle.fwdRequestAllowed = false;

		res = shutdown_stop(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			// This may happen if the start was not called
			// 
			// assert(0);
		}

		res = shutdown_postprocess(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_deactivate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		res = shutdown_terminate(&errMess, NULL);
		if (res != JVX_NO_ERROR)
		{
			assert(0);
		}

		commLine.unassign_args();
		
		init_done = false;
		synchronize_thread_leave();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_SETTING;
}

// ==========================================================================================

void
jvxLibHost::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context) 
{
	// Forward towards GUI host
}

void
jvxLibHost::inform_config_read_start(const char* nmfile, jvxHandle* context) 
{
	// Forward towards GUI host
}

void 
jvxLibHost::inform_config_read_stop(jvxErrorType resRead, const char* nmfile, jvxHandle* context) 
{
	// Forward towards GUI host -> Text output
}

void 
jvxLibHost::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{
	// Forward towards GUI host
}

void 
jvxLibHost::get_configuration_specific_ext(
	jvxCallManagerConfiguration* callConf,
	IjvxConfigurationExtender* ext)
{
	// Forward towards GUI host
	jvxValue val;

	/*
	val.assign(theHostFeatures.rtv_period_plots_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(theHostFeatures.rtv_period_props_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	val.assign(theHostFeatures.timeout_viewer_maincentral_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

	*/

	val.assign(theHostFeatures.timeout_period_seq_ms);
	ext->set_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ, &val, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, 0);

}

void 
jvxLibHost::command_line_specify_specific(IjvxCommandLine* cmdLine)
{
	cmdLine->register_option("--pixbufm", "", "Specifc the pixbuffer lib to register rendering callbacks.", "", true, JVX_DATAFORMAT_STRING);
	cmdLine->register_option("--pixbufs", "", "Specifc the pixbuffer entry symbol to register rendering callbacks.", "", true, JVX_DATAFORMAT_STRING);
}

void 
jvxLibHost::command_line_read_specific(IjvxCommandLine* cmdLine)
{
	jvxApiString entry;
	if (cmdLine->content_entry_option("--pixbufm", 0, &entry, JVX_DATAFORMAT_STRING) == JVX_NO_ERROR)
	{
		extPixBuffer.dllPixBufSystem = entry.std_str();
		extPixBuffer.symbPixBufSystemName = FLUTTER_PIXBUF_OPEN_FUNCTION_NAME;
		if (cmdLine->content_entry_option("--pixbufs", 0, &entry, JVX_DATAFORMAT_STRING) == JVX_NO_ERROR)
		{
			extPixBuffer.symbPixBufSystemName = entry.std_str();
		}
	}
}

void 
jvxLibHost::get_configuration_specific(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* cfg, jvxConfigData* data)
{
	// Forward towards GUI host
}

void 
jvxLibHost::put_configuration_specific(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* cfg, jvxConfigData* data, const char* fName)
{
	// Forward towards GUI host
}

void 
jvxLibHost::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigurationExtender* ext)
{
	jvxValue val;
	jvxConfigSectionTypes myCfgTp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxErrorType res = JVX_NO_ERROR;

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_SEQ, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.timeout_period_seq_ms);
	}

	/*
	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_MAINCENTRAL, &val, &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.timeout_viewer_maincentral_ms);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PLOTS, reinterpret_cast<jvxHandle**>(&val), &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.rtv_period_plots_ms);
	}

	myCfgTp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
	res = ext->get_configuration_entry(JVX_COMMON_PROPERTIES_HOST_PERIOD_RTVIEWER_PROPS, reinterpret_cast<jvxHandle**>(&val), &myCfgTp, 0);
	if (res == JVX_NO_ERROR)
	{
		val.toContent(&theHostFeatures.rtv_period_props_ms);
	}
	*/
	// Forward towards GUI host
}

// =============================================================================
// IjvxReportStateSwitch IjvxReportStateSwitch IjvxReportStateSwitch IjvxReportStateSwitch
// =============================================================================
#if 0 
jvxErrorType
jvxLibHost::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp) 
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
jvxLibHost::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc) 
{
	return JVX_ERROR_UNSUPPORTED;
}
#endif


