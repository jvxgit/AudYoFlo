#ifndef __CJVXMEXCALLSPROFILER__H__
#define __CJVXMEXCALLSPROFILER__H__

#include "jvx.h"
#include "CjvxMexCallsMin.h"

#include "jvx_profiler/jvx_profiler_data_entry.h"

class CjvxMexCallsProfiler: public CjvxMexCallsMin
{
protected:
	std::string varNameHdlMatlab = "hdl_profile_data";

	std::string commandProfileConfig = "jvxProfileConfig";
	std::string commandProfileStart = "jvxProfileStart";
	std::string commandProfileStep = "jvxProfileStep";
	std::string commandProfilePreStep = "";
	std::string commandProfileStop = "jvxProfileStop";

	jvxBool debugStartDone = false;
	jvxBool reportErrorPreRunMatlabData = false;

public:
	std::map<std::string, jvx_profiler_data_entry*> registeredProfilerData;
	jvxErrorType register_profiling_data(jvx_profiler_data_entry*, const std::string& nm);
	jvxErrorType unregister_profiling_data(const std::string& nm);
	jvxErrorType obtainProvideDataMexCall(jvxBool postRun = true);
	static jvxErrorType jvx_register_entry_profiling_data_cb(jvx_profiler_data_entry* dat, const char* name, jvxHandle* inst);
	static jvxErrorType jvx_unregister_entry_profiling_data_cb(const char* name, jvxHandle* inst);
	
	jvxErrorType profile_start_in_process();
	jvxErrorType profile_pre_step_in_process();
	jvxErrorType profile_step_in_process();
	jvxErrorType profile_config_on_prepare();
	jvxErrorType profile_stop_on_postprocess();
};

#endif
