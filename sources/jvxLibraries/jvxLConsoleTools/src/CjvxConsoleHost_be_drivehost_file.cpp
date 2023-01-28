#include "CjvxConsoleHost_be_drivehost.h"
#include "CjvxHostJsonDefines.h"


jvxErrorType
CjvxConsoleHost_be_drivehost::file_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList & jsec, 
	jvxCBitField* extFlags)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string operation;
	std::string filename;
	std::string errTxt;
	jvxCallManagerConfiguration callConf;
	if (args.size() > off)
	{
		operation = args[off];
	}
	if (args.size() > off+1)
	{
		filename = args[off+1];
	}
	else
	{
		filename = cfg_filename_in_use;
	}

	if (operation == "read")
	{
		if (!filename.empty())
		{
			cfg_filename_in_use = filename;
		}

		if (!cfg_filename_in_use.empty())
		{
			res = try_reopen_config(); 
			if (res == JVX_NO_ERROR)
			{
				assert(extFlags);
				jvx_bitSet(*extFlags, JVX_EVENT_CONTINUE_WANT_SHUTDOWN_RESTART_SHIFT);
			}
			else
			{ 
				errTxt = "Failed to prepare reading of config file " + filename;
				errTxt += ", reason: ";
				errTxt += jvxErrorType_descr(res);
				JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
			}
		}

		/*
		res = configureFromFile(filename, jvxFlagTag(0));
		
		if (res != JVX_NO_ERROR)
		{
			errTxt = "Failed to read config file " + filename;
			errTxt += ", reason: ";
			errTxt += jvxErrorType_descr(res);
			JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
		}
		*/
	}
	else if (operation == "write")
	{
		cfg_filename_in_use = filename;

		callConf.configModeFlags = JVX_CONFIG_MODE_FULL;

		res = configureToFile(&callConf, cfg_filename_in_use );
		if (res != JVX_NO_ERROR)
		{
			errTxt = "Failed to write config file <" + filename;
			errTxt += ">, reason: ";
			errTxt += jvxErrorType_descr(res);
			JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
		}
	}
	else
	{
		errTxt = "Operation <" + operation;
		errTxt += "> is not a valid command.";
		JVX_CREATE_ERROR_NO_RETURN(jsec, res, errTxt);
	}
	return res;
}