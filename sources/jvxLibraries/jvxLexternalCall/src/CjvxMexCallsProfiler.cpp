#include "CjvxMexCallsProfiler.h"

#define AYF_COPY_TPX(fld, sz, nm, cplx) \
		

#define AYF_COPY_TPX_STR(entry, nm) \
	AYF_COPY_TPX(entry->fld, entry->sz, nm, entry.cplx)

jvxErrorType 
CjvxMexCallsProfiler::register_profiling_data(jvx_profiler_data_entry * data, const std::string& nm)
{
	auto elm = registeredProfilerData.find(nm);
	if (elm == registeredProfilerData.end())
	{
		registeredProfilerData[nm] = data;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_DUPLICATE_ENTRY;
}

jvxErrorType 
CjvxMexCallsProfiler::unregister_profiling_data(const std::string& nm)
{
	auto elm = registeredProfilerData.find(nm);
	if (elm != registeredProfilerData.end())
	{
		registeredProfilerData.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxMexCallsProfiler::provideDataMexCall()
{
	jvxExternalDataType* passToMatlab = NULL;
	for (auto& elm : registeredProfilerData)
	{
		if (elm.second->fld)
		{
			_theExtCallHandler->convertCToExternal(&passToMatlab, elm.second->fld,
				elm.second->sz, JVX_DATAFORMAT_DATA, elm.second->cplx);
			if (passToMatlab)
			{
				_theExtCallHandler->putVariableToExternal("caller", elm.first.c_str(), passToMatlab);
				std::string command = "global " + varNameHdlMatlab + "; " + varNameHdlMatlab + ".tp.";
				command += elm.first;
				command += " = ";
				command += elm.first;
				command += ";";
				jvxErrorType resCommand = _theExtCallHandler->executeExternalCommand(command.c_str()); 
					if (resCommand != JVX_NO_ERROR)
					{
						jvxApiString astr;
						_theExtCallHandler->getLastErrorReason(&astr);
						_theExtCallHandler->postMessageExternal(("Last operation <" + command + "> failed, reason: <" + astr.std_str() + ">.").c_str(), true);
					}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxMexCallsProfiler::jvx_register_entry_profiling_data_cb(jvx_profiler_data_entry* dat, const char* name, jvxHandle* inst)
{
	if (inst)
	{
		CjvxMexCallsProfiler* this_pointer = (CjvxMexCallsProfiler*)inst;
		return this_pointer->register_profiling_data(dat, name);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType 
CjvxMexCallsProfiler::jvx_unregister_entry_profiling_data_cb(const char* name, jvxHandle* inst)
{
	if (inst)
	{
		CjvxMexCallsProfiler* this_pointer = (CjvxMexCallsProfiler*)inst;
		return this_pointer->unregister_profiling_data(name);
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxMexCallsProfiler::profile_start_in_process()
{
	if (!debugStartDone)
	{
		std::string command = "global " + CjvxMexCallsProfiler::varNameHdlMatlab + "; " + CjvxMexCallsProfiler::varNameHdlMatlab +
			" = " + CjvxMexCallsProfiler::commandProfileStart + "(" + CjvxMexCallsProfiler::varNameHdlMatlab + "); ";
		jvxErrorType resM = _theExtCallHandler->executeExternalCommand(command.c_str());
		if (resM != JVX_NO_ERROR)
		{
			jvxApiString astr;
			_theExtCallHandler->getLastErrorReason(&astr);
			_theExtCallHandler->postMessageExternal(("Last operation <" + command + "> failed, reason: <" + astr.std_str() + ">.").c_str(), true);
		}
		debugStartDone = true;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCallsProfiler::profile_step_in_process()
{

	std::string command = "global " + CjvxMexCallsProfiler::varNameHdlMatlab +
		"; [" + CjvxMexCallsProfiler::varNameHdlMatlab + "] = " + CjvxMexCallsProfiler::commandProfileStep + "(" + CjvxMexCallsProfiler::varNameHdlMatlab + "); ";
	jvxErrorType resM = _theExtCallHandler->executeExternalCommand(command.c_str());

	if (resM != JVX_NO_ERROR)
	{
		jvxApiString astr;
		_theExtCallHandler->getLastErrorReason(&astr);
		_theExtCallHandler->postMessageExternal(("Last operation <" + command + "> failed, reason: <" + astr.std_str() + ">.").c_str(), true);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCallsProfiler::profile_config_on_prepare()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resM = JVX_NO_ERROR;
	debugStartDone = false;
	std::string command = "global " + CjvxMexCallsProfiler::varNameHdlMatlab + "; " + CjvxMexCallsProfiler::varNameHdlMatlab + " = " + CjvxMexCallsProfiler::commandProfileConfig + "(); ";
	resM = _theExtCallHandler->executeExternalCommand(command.c_str());
	if (resM != JVX_NO_ERROR)
	{
		jvxApiString astr;
		_theExtCallHandler->getLastErrorReason(&astr);
		_theExtCallHandler->postMessageExternal(("Last operation <" + command + "> failed, reason: <" + astr.std_str() + ">.").c_str(), true);

		// Standard exception indicates that the debugger was detached!!
		if (resM == JVX_ERROR_STANDARD_EXCEPTION)
		{
			res = resM;
		}
	}
	return res;
}

jvxErrorType
CjvxMexCallsProfiler::profile_stop_on_postprocess()
{
	std::string command = "global " + CjvxMexCallsProfiler::varNameHdlMatlab + " ; " + CjvxMexCallsProfiler::commandProfileStop + "(" + CjvxMexCallsProfiler::varNameHdlMatlab + "); ";
	jvxErrorType resM = _theExtCallHandler->executeExternalCommand(command.c_str());
	if (resM != JVX_NO_ERROR)
	{
		jvxApiString astr;
		_theExtCallHandler->getLastErrorReason(&astr);
		_theExtCallHandler->postMessageExternal(("Last operation <" + command + "> failed, reason: <" + astr.std_str() + ">.").c_str(), true);
	}
	debugStartDone = false;
	return JVX_NO_ERROR;
}