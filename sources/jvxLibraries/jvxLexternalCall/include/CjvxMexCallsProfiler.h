#ifndef __CJVXMEXCALLSPROFILER__H__
#define __CJVXMEXCALLSPROFILER__H__

#include "jvx.h"
#include "CjvxMexCallsMin.h"

#include "jvx_profiler/jvx_profiler_data_entry.h"

class CjvxMexCallsProfiler: public CjvxMexCallsMin
{
protected:
	std::string varNameHdlMatlab = "hdl_profile_data";
public:
	std::map<std::string, jvx_profiler_data_entry*> registeredProfilerData;
	jvxErrorType register_profiling_data(jvx_profiler_data_entry*, const std::string& nm);
	jvxErrorType unregister_profiling_data(const std::string& nm);
	jvxErrorType provideDataMexCall();
	static jvxErrorType jvx_register_entry_profiling_data_cb(jvx_profiler_data_entry* dat, const char* name, jvxHandle* inst);
	static jvxErrorType jvx_unregister_entry_profiling_data_cb(const char* name, jvxHandle* inst);
};

#endif
