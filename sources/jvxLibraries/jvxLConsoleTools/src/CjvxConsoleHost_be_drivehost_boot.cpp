#include "CjvxConsoleHost_be_drivehost.h"
#include "jvxHJvx.h"

#if 0
void
CjvxConsoleHost_be_drivehost::initConfigureHost()
{
	if (!_command_line_parameters.configFilename.empty())
	{
		configureFromFile(_command_line_parameters.configFilename, jvxFlagTag(0));
	}
	postbootup_specific();
}

void
CjvxConsoleHost_be_drivehost::initializeHost()
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	std::string message;
	jvxComponentIdentification cpType;
	bool multObj = false;
	jvxCBool cout_mess = c_false;
	jvxCBool verbose_dll = c_false;
	jvxCBool textLog_activate = c_false;
	jvxApiString theStr;
	IjvxProperties* theProps = NULL;
	std::string nm = "ERROR";
	jvxBool isValid = false;
	jvxSize id = 0;

	res = jvxHJvx_init(&involvedComponents.theHost.hobject);
	if ((res != JVX_NO_ERROR) || (involvedComponents.theHost.hobject == NULL))
	{
		fatalStop("Fatal Error", "Could not load main host component!");
	}

	message = textMessagePrioToString("Successfully opened host", JVX_REPORT_PRIORITY_INFO);
	postMessage_inThread(message);

	involvedComponents.theHost.hobject->request_specialization((jvxHandle**)&involvedComponents.theHost.hHost, &cpType, &multObj);

	// Check component type of specialization
	if (involvedComponents.theHost.hHost && (cpType == JVX_COMPONENT_HOST))
	{
		if (!_command_line_parameters.changeWorkDir.empty())
		{
			JVX_CHDIR(_command_line_parameters.changeWorkDir.c_str());
		}

		// Initialize the host..
		involvedComponents.theHost.hHost->initialize(NULL);
		involvedComponents.theHost.hHost->select();

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
		res = involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
		if ((res == JVX_NO_ERROR) && theProps)
		{

			// Set the predefined property for dll path for subcomponents
			if (jvx_findPropertyDescriptor("jvx_component_path_str_list", &id, NULL, NULL, NULL))
			{
				res = jvx_pushPullPropertyStringList(_command_line_parameters.dllsDirectories, involvedComponents.theHost.hobject, theProps, id, 
					JVX_PROPERTY_CATEGORY_PREDEFINED, &isValid);
			}

			res = jvx_set_property(theProps, &cout_mess, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/host_output_cout");
			assert(res == JVX_NO_ERROR);

			res = jvx_set_property(theProps, &verbose_dll, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/host_verbose_dll");
			assert(res == JVX_NO_ERROR);

			theStr.assign_const(_command_line_parameters.textLog_filename.c_str(), _command_line_parameters.textLog_filename.size());
			res = jvx_set_property(theProps, &theStr, 0, 1, JVX_DATAFORMAT_STRING, false, "/textLog_filename");
			assert(res == JVX_NO_ERROR);

			res = jvx_set_property(theProps, &_command_line_parameters.textLog_dbglevel, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_dbglevel");
			assert(res == JVX_NO_ERROR);

			res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferFile");
			assert(res == JVX_NO_ERROR);

			_command_line_parameters.textLog_sizeInternBufferFile = _command_line_parameters.textLog_sizeInternBufferFile >> 3;
			res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferFile, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeTransferFile");
			assert(res == JVX_NO_ERROR);

			res = jvx_set_property(theProps, &_command_line_parameters.textLog_sizeInternBufferRW, 0, 1, JVX_DATAFORMAT_SIZE, false, "/textLog_sizeInternBufferRW");
			assert(res == JVX_NO_ERROR);

			theStr.assign_const(_command_line_parameters.textLog_expression.c_str(), _command_line_parameters.textLog_expression.size());
			res = jvx_set_property(theProps, &theStr, 0, 1, JVX_DATAFORMAT_STRING, false, "/textLog_expression");
			assert(res == JVX_NO_ERROR);

			res = jvx_set_property(theProps, &textLog_activate, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/textLog_activate");
			assert(res == JVX_NO_ERROR);

			res = involvedComponents.theHost.hHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
		}

		involvedComponents.theHost.hHost->set_external_report_target(static_cast<IjvxReport*>(this));
		involvedComponents.theHost.hHost->set_external_report_on_config(static_cast<IjvxReportOnConfig*>(this));

		res = bootup_specific();
		if (res != JVX_NO_ERROR)
		{
			fatalStop("Fatal Error", "Specific bootup was not successful!");
		}

		// =================================================================
		// Activate the host
		// =================================================================
		involvedComponents.theHost.hHost->activate();
	}
	else
	{
		fatalStop("Fatal Error", "Invalid object specialization!");
	}


	// Tools interfaces
	res = involvedComponents.theHost.hHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&involvedComponents.theTools.hTools));
	if (res != JVX_NO_ERROR)
	{
		fatalStop("Fatal Error", "No access to JVX tools!");
	}

	// ==========================================================================
}

#endif
