#include "jvxLibHost.h"

#ifdef JVX_LIBHOST_WITH_JSON_SHOW
#include "CjvxHostJsonCommandsActShow.h"

class CjvxHostJsonCommandsActShowStartStop : public CjvxHostJsonCommandsActShow
{
public:
	jvxLibHost* theRef = nullptr;
	CjvxHostJsonCommandsActShowStartStop(IjvxHost*& hHostRefArg, jvxLibHost* ref) :
		CjvxHostJsonCommandsActShow(hHostRefArg)
	{
		theRef = ref;
	};

	virtual jvxErrorType act_start_sequencer(CjvxJsonElementList& lstelmr) override
	{
		CjvxJsonElement jelm;
		jvxErrorType res = JVX_NO_ERROR;
		if (theRef)
		{
			res = theRef->sequencer_start(JVX_SIZE_UNSELECTED);
			
			if (res != JVX_NO_ERROR)
			{
				std::string txt = "Operation returned error code <";
				txt += jvxErrorType_descr(res);
				txt += ">.";
				
				jelm.makeAssignmentString("error_description", txt);
				lstelmr.addConsumeElement(jelm);
			}
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
			jelm.makeAssignmentString("error_description", "No implementation available."); 
			lstelmr.addConsumeElement(jelm); 
		}
		return res;
	};

	virtual jvxErrorType act_stop_sequencer(CjvxJsonElementList& lstelmr, jvxBool full_stop) override
	{
		CjvxJsonElement jelm;
		jvxErrorType res = JVX_NO_ERROR;
		if (theRef)
		{
			res = theRef->sequencer_stop();

			/*
			jelm.makeAssignmentString("return_code", jvxErrorType_str[res].friendly);
			lstelmr.insertConsumeElementFront(jelm);
			*/
			if (res != JVX_NO_ERROR)
			{
				std::string txt = "Operation returned error code <";
				txt += jvxErrorType_descr(res);
				txt += ">.";
				CjvxJsonElement jelm;
				jelm.makeAssignmentString("error_description", txt);
				lstelmr.addConsumeElement(jelm);
			}
		}
		else
		{
			res = JVX_ERROR_UNSUPPORTED;
			jelm.makeAssignmentString("error_description", "No implementation available.");
			lstelmr.addConsumeElement(jelm);
		}
		return res;
	};
};

#endif

jvxErrorType 
jvxLibHost::allocate_json_show_handle()
{
#ifdef JVX_LIBHOST_WITH_JSON_SHOW
	CjvxHostJsonCommandsActShowStartStop* hdl = nullptr;
	if (showHandle == nullptr)
	{
		hdl = new CjvxHostJsonCommandsActShowStartStop(involvedHost.hHost, this);
		showHandle = reinterpret_cast<jvxHandle*>(hdl);
		
		// Make the array output readable
		hdl->config_show.outputPropertyFieldsBase64 = false;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
#endif
	return JVX_ERROR_UNSUPPORTED;
}
	
jvxErrorType 
jvxLibHost::deallocate_json_show_handle()
{
#ifdef JVX_LIBHOST_WITH_JSON_SHOW
	if (showHandle)
	{
		CjvxHostJsonCommandsActShowStartStop* hdl = reinterpret_cast<CjvxHostJsonCommandsActShowStartStop*>(showHandle);
		delete hdl;
		showHandle = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
#endif
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
jvxLibHost::interpret_command(const char* command, jvxApiString* strRet, jvxBool jsonOut) 
//jvxLibHost::run_direct_show_command(const std::string& command, jvxApiString* strRet, jvxBool jsonOut)
{
#ifdef JVX_LIBHOST_WITH_JSON_SHOW
	std::vector<std::string> args;
	std::string addArg;
	oneDrivehostCommand dh_command;
	jvxBool translate_command_failed = false;
	jvxBool parse_error = false;
	jvxBool command_empty = false;
	jvxBool regular_command = false;
	jvxBool translate_firstarg_failed = false;
	jvxBool systemUpdate = false;
	jvxSize off = 1;
	CjvxJsonElementList jsec;
	CjvxJsonElement jelm;
	CjvxJsonElement* jelmfind = nullptr;
	std::string response;
	CjvxHostJsonCommandsActShowStartStop* hdl = reinterpret_cast<CjvxHostJsonCommandsActShowStartStop*>(showHandle);
	assert(hdl);
	hdl->translate_full_command(
		command, dh_command,
		args,
		addArg,
		translate_command_failed,
		parse_error,
		command_empty,
		regular_command,
		translate_firstarg_failed);
	if (regular_command)
	{
		
		jvxErrorType resL = hdl->process_command_abstraction(dh_command,
			args, addArg, off, jsec, &systemUpdate);
		
		// Default behavior: If an error_code was returned, use the corresponding return values. Otherwise produce a generic 
		// entry!
		// If a return code has not been set before set it here!
		jelmfind = nullptr;
		jsec.reference_element(&jelmfind, "return_code");
		if (jelmfind == nullptr)
		{
			jelm.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
		}
		jsec.insertConsumeElementFront(jelm);
		if (resL != JVX_NO_ERROR)
		{
			// if an error description has not been set before, return it here
			jelmfind = nullptr;
			jsec.reference_element(&jelmfind, "error_description");
			if (jelmfind == nullptr)
			{
				std::string txt = "Command <";
				txt += command;
				txt += "> failed, reason: ";
				txt += jvxErrorType_descr(resL);
				txt += ">.";
				jelm.makeAssignmentString("error_description", txt);
				jsec.addConsumeElement(jelm);
			}
		}
	}
	else
	{
		jelm.makeAssignmentString("return_code", jvxErrorType_str[JVX_ERROR_INVALID_ARGUMENT].friendly);
		jsec.insertConsumeElementFront(jelm);

		std::string txt = "Command <";
		txt += command;
		txt += "> failed, reason: <";
		if (parse_error)
		{
			txt += "Parse error";
		}
		else if (command_empty)
		{
			txt += "Command empty";
		}
		else if (translate_firstarg_failed)
		{
			txt += "Command argument is incorrect.";
		}
		else if (translate_firstarg_failed)
		{
			txt += "Unknown reason";
		}

		txt += ">.";
		jelm.makeAssignmentString("error_description", txt);
		jsec.addConsumeElement(jelm);		
	}

	if (jsonOut)
	{
		jsec.printToJsonView(response);
	}
	else
	{
		jsec.printToStringView(response);
	}
	
	// convert the asciis to utf8
	response = jvx::helper::asciToUtf8(response);

	if (strRet)
	{
		strRet->assign(response);
	}
	else
	{
		std::cout << response << std::endl;
	}
	return JVX_NO_ERROR;
#else
	return JVX_ERROR_UNSUPPORTED;
#endif

}

