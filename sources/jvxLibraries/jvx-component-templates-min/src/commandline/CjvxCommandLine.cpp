
#include "commandline/CjvxCommandLine.h"

typedef enum
{
	JVX_COMMANDLINE_STATUS_START,
	JVX_COMMANDLINE_STATUS_TOKEN,
	JVX_COMMANDLINE_STATUS_CONTENT
} jvxScanCommandLineStatus;

CjvxCommandLine::CjvxCommandLine()
{
	state_parse = JVX_STATE_INIT;
	copiedappname = "NOT_READY";
}

CjvxCommandLine::~CjvxCommandLine()
{
}

jvxErrorType 
CjvxCommandLine::assign_args(const char* clStr[], int clNum, const char* appname)
{
	jvxSize i;
	if (state_parse == JVX_STATE_INIT)
	{
		for (i = 0; i < clNum; i++)
			copiedargv.push_back(clStr[i]);
		copiedappname = appname;
		state_parse = JVX_STATE_SELECTED;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxCommandLine::unassign_args()
{
	if (state_parse == JVX_STATE_SELECTED)
	{
		copiedappname = "NOT_READY";
		copiedargv.clear();
		state_parse = JVX_STATE_INIT;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxCommandLine::register_option(const char* scan_token,
	const char* scan_tokenshortcut,	const char* scan_description, const char* default_assign,
	jvxBool expect_second_arg, jvxDataFormat format_scan, jvxSize num_args_max)
{
	if (state_parse != JVX_STATE_SELECTED)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	std::map<std::string, oneEntryCommandLine>::iterator elm = registered_options.find(scan_token);
	if (elm != registered_options.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}

	oneEntryCommandLine newEntry;
	newEntry.scan_token = scan_token;
	newEntry.scan_tokenshortcut = scan_tokenshortcut;
	newEntry.scan_description = scan_description;
	newEntry.default_setting = default_assign;
	newEntry.format_scan = format_scan;
	newEntry.num_args_max = num_args_max;
	if (newEntry.num_args_max == 0)
		newEntry.num_args_max = 1;
	newEntry.expect_second_arg = expect_second_arg;
	if (newEntry.expect_second_arg == false)
	{
		newEntry.num_args_max = 1;
	}

	newEntry.num_specified = 0;

	registered_options[scan_token] = newEntry;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCommandLine::reset_command_line_content()
{
	if (state_parse == JVX_STATE_ACTIVE)
	{
		std::map<std::string, oneEntryCommandLine>::iterator elm = registered_options.begin();
		for (; elm != registered_options.end(); elm++)
		{
			elm->second.theContent.clear();
			elm->second.num_specified = 0;
		}
		state_parse = JVX_STATE_SELECTED;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType 
CjvxCommandLine::reset_command_line()
{
	if (state_parse == JVX_STATE_SELECTED)
	{
		registered_options.clear();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxCommandLine::parse_command_line(jvxSize* idErrToken, jvxApiString* descr, jvxBool allow_unmatched)
{
	int cnt = 0;
	bool cont = true;
	jvxBool err = false;
	jvxSize i;
	std::string arg_token;
	std::map<std::string, oneEntryCommandLine>::iterator elm;
	jvxScanCommandLineStatus status = JVX_COMMANDLINE_STATUS_START;
	oneEntryContentCommandLine newContent;
	jvxBool locErr;
	if (state_parse == JVX_STATE_SELECTED)
	{
		unmatched_args.clear();
		for (i = 0; i < copiedargv.size(); i++)
		{
			switch (status)
			{
			case JVX_COMMANDLINE_STATUS_START:
				status = JVX_COMMANDLINE_STATUS_TOKEN;

				// Fallthrough with intention here				

			case JVX_COMMANDLINE_STATUS_TOKEN:

				if (copiedargv[i] == "--help")
				{

					return JVX_ERROR_ABORT;
				}

				elm = registered_options.find(copiedargv[i]);
				if (elm == registered_options.end())
				{
					elm = registered_options.begin();
					for (; elm != registered_options.end(); elm++)
					{
						if (elm->second.scan_tokenshortcut == copiedargv[i])
						{
							break;
						}
					}
				}

				if (elm != registered_options.end())
				{
					elm->second.num_specified++;
					if (elm->second.expect_second_arg)
					{
						status = JVX_COMMANDLINE_STATUS_CONTENT;
					}
				}
				else
				{
					if (allow_unmatched)
					{
						unmatched_args.push_back(copiedargv[i]);
					}
					else
					{
						if (idErrToken)
							*idErrToken = i;
						if (descr)
							descr->assign(std::string("Invalid option ") + copiedargv[i]);
						return JVX_ERROR_PARSE_ERROR;
					}
				}
				break;

			case JVX_COMMANDLINE_STATUS_CONTENT:
				newContent.cStr = copiedargv[i];
				newContent.cData = 0;
				newContent.cInt = 0;
				newContent.cSize = 0;
				switch (elm->second.format_scan)
				{
				case JVX_DATAFORMAT_STRING:

					break;
				case JVX_DATAFORMAT_32BIT_LE:
					locErr = false;
					newContent.cInt = JVX_INTMAX_INT32(jvx_string2IntMax(newContent.cStr, locErr));
					if (locErr)
					{
						if (idErrToken)
							*idErrToken = i;
						if (descr)
							descr->assign(std::string("Invalid specification of integer option ") + copiedargv[i]);
						return JVX_ERROR_INVALID_ARGUMENT;
					}
					break;
				case JVX_DATAFORMAT_SIZE:
					locErr = false;
					newContent.cSize = jvx_string2Size(newContent.cStr, locErr);
					if (locErr)
					{
						if (idErrToken)
							*idErrToken = i;
						if (descr)
							descr->assign(std::string("Invalid specification of integer option ") + copiedargv[i]);
						return JVX_ERROR_INVALID_ARGUMENT;
					}
					break;
				case JVX_DATAFORMAT_DATA:
					locErr = false;
					newContent.cData = jvx_string2Data(newContent.cStr, locErr);
					if (locErr)
					{
						if (idErrToken)
							*idErrToken = i;
						if (descr)
							descr->assign(std::string("Invalid specification of data option ") + copiedargv[i]);
						return JVX_ERROR_INVALID_ARGUMENT;
					}
					break;
				}
				elm->second.theContent.push_back(newContent);
				status = JVX_COMMANDLINE_STATUS_TOKEN;
				break;
			}
		}

		elm = registered_options.begin();
		for (; elm != registered_options.end(); elm++)
		{
			if (elm->second.num_specified == 0)
			{
				// Set default value if not specified
				newContent.cStr = elm->second.default_setting;
				newContent.cData = 0;
				newContent.cInt = 0;
				newContent.cSize = 0;

				if (
					elm->second.expect_second_arg &&
					!elm->second.default_setting.empty()
					)
				{
					// Empty means there is no default - field is "empty"
					switch (elm->second.format_scan)
					{
					case JVX_DATAFORMAT_STRING:

						break;
					case JVX_DATAFORMAT_32BIT_LE:
						locErr = false;
						newContent.cInt = JVX_INTMAX_INT32(jvx_string2IntMax(newContent.cStr, locErr));
						if (locErr)
						{
							if (idErrToken)
								*idErrToken = i;
							if (descr)
								descr->assign(std::string("Invalid specification of integer option ") + copiedargv[i]);
							return JVX_ERROR_INVALID_ARGUMENT;
						}
						break;
					case JVX_DATAFORMAT_SIZE:
						locErr = false;
						newContent.cSize = jvx_string2Size(newContent.cStr, locErr);
						if (locErr)
						{
							if (idErrToken)
								*idErrToken = i;
							if (descr)
								descr->assign(std::string("Invalid specification of integer option ") + copiedargv[i]);
							return JVX_ERROR_INVALID_ARGUMENT;
						}
						break;
					case JVX_DATAFORMAT_DATA:
						locErr = false;
						newContent.cData = jvx_string2Data(newContent.cStr, locErr);
						if (locErr)
						{
							if (idErrToken)
								*idErrToken = i;
							if (descr)
								descr->assign(std::string("Invalid specification of data option ") + copiedargv[i]);
							return JVX_ERROR_INVALID_ARGUMENT;
						}
						break;
					}
					elm->second.theContent.push_back(newContent);
				}
			}

			if (JVX_CHECK_SIZE_UNSELECTED(elm->second.num_args_max))
			{
				if (elm->second.num_args_max < elm->second.num_specified)
				{
					if (descr)
						descr->assign("Invalid number of specifications for option " + elm->first + ", a maximum of " + jvx_size2String(elm->second.num_args_max) + " entries is expected.");
					return JVX_ERROR_INVALID_SETTING;
				}
			}
		}
		state_parse = JVX_STATE_ACTIVE;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
};

jvxErrorType 
CjvxCommandLine::number_entries_option(const char* scan_token, jvxSize* numEntries)
{
	std::map<std::string, oneEntryCommandLine>::iterator elm;
	elm = registered_options.find(scan_token);
	if (elm != registered_options.end())
	{
		if (elm->second.expect_second_arg)
		{
			if (numEntries)
			{
				*numEntries = elm->second.theContent.size();
			}
		}
		else
		{
			if (numEntries)
			{
				*numEntries = elm->second.num_specified;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxCommandLine::content_entry_option(const char* scan_token, jvxSize id, jvxHandle* fld, jvxDataFormat form)
{
	std::map<std::string, oneEntryCommandLine>::iterator elm;
	if(fld)
	{
		elm = registered_options.find(scan_token);
		if (elm != registered_options.end())
		{
			if (elm->second.expect_second_arg)
			{
				if (id < elm->second.theContent.size())
				{
					if (form == elm->second.format_scan)
					{
						switch (form)
						{
						case JVX_DATAFORMAT_STRING:
							((jvxApiString*)fld)->assign(elm->second.theContent[id].cStr);
							break;
						case JVX_DATAFORMAT_SIZE:
							*((jvxSize*)fld) = elm->second.theContent[id].cSize;
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							*((jvxInt32*)fld) = elm->second.theContent[id].cInt;
							break;
						case JVX_DATAFORMAT_DATA:
							*((jvxData*)fld) = elm->second.theContent[id].cData;
							break;
						}
						return JVX_NO_ERROR;
					}
					else
					{
						return JVX_ERROR_INVALID_SETTING;
					}
				}
				else
				{
					return JVX_ERROR_ID_OUT_OF_BOUNDS;
				}
			}
			else
			{
				if (form == JVX_DATAFORMAT_U16BIT_LE)
				{
					if (elm->second.num_specified)
					{
						*(jvxUInt16*)fld = true;
					}
					else
					{
						*(jvxUInt16*)fld = false;
					}
					return JVX_NO_ERROR;
				}
				else
				{
					return JVX_ERROR_INVALID_SETTING;
				}
			}
		}
		else
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

void
CjvxCommandLine::printUsage(jvxApiString* usage_string)
{
	std::string txtLoc;

	std::map<std::string, oneEntryCommandLine>::iterator elm;
	txtLoc += "Usage: ";
	txtLoc += copiedappname;
	txtLoc += " [args]\n";
	
	elm = registered_options.begin();
	for (; elm != registered_options.end(); elm++)
	{
		if (elm->second.expect_second_arg)
		{
			txtLoc += "  ";
			txtLoc += elm->second.scan_token;
			txtLoc += " <arg>: ";
			txtLoc += elm->second.scan_description;
			txtLoc += ", default specification: ";
			if (elm->second.default_setting.empty())
			{
				txtLoc += "none.";
			}
			else
			{
				txtLoc += "<arg> = ";
				txtLoc += elm->second.default_setting;
				txtLoc += ".";
			}
		}
		else
		{
			txtLoc += "  ";
			txtLoc += elm->second.scan_token;
			txtLoc += " (simple switch): ";
			txtLoc += elm->second.scan_description;
			txtLoc += ".";
		}
		txtLoc += "\n";
	}

	if (usage_string)
	{
		usage_string->assign(txtLoc);
	}
	else
	{
		std::cout << txtLoc << std::flush;
	}
}

jvxErrorType
CjvxCommandLine::arg_app_name(jvxApiString* retStr)
{
	if (retStr)
	{
		retStr->assign(copiedappname);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCommandLine::number_args(jvxSize* num)
{
	if (num)
	{
		*num = copiedargv.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxCommandLine::single_arg_index(jvxSize id, jvxApiString* retStr)
{
	if (id < copiedargv.size())
	{
		if (retStr)
			retStr->assign(copiedargv[id]);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

// =======================================================================================

jvxErrorType 
CjvxCommandLine::number_unmatched(jvxSize* sz)
{
	if (sz)
	{
		*sz = unmatched_args.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxCommandLine::single_unmatched_index(jvxSize id, jvxApiString* retStr)
{
	if (id < unmatched_args.size())
	{
		if (retStr)
			retStr->assign(unmatched_args[id]);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}
