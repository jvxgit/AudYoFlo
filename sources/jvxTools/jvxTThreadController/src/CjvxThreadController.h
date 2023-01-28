#ifndef __CJVXTHREADCONTROLLER_H__
#define __CJVXTHREADCONTROLLER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"

#include "jvx_threads.h"

class CjvxThreadController: public IjvxThreadController, public CjvxObject
{
public:

	/** Enum for the datatype for stored datachunk fields, can be enhanced in the future
	 *///===================================================
private:

	typedef struct
	{
		std::string theNameProcess;
		JVX_PROCESS_ID theId;
		jvxPriorityType thePrio;
		jvxInt32 selector[1];
		JVX_AFFINITY_MASK initAffinity;
		jvxBool affinitySet;
	} oneProcessObject;

	typedef struct
	{
		jvxPriorityType thePrio;
		JVX_THREAD_ID selector[1];
		JVX_THREAD_HANDLE theHdl;
		JVX_AFFINITY_MASK previousValueSet;
	} oneThreadObject;


	typedef struct
	{
		jvxInt32 selector[1];
		jvxInt32 threadAffId;
		JVX_THREAD_ID idThread;
		jvxBool idRegistered;
	} oneRegisteredThread;

	struct
	{
		JVX_PROCESS_HANDLE myHandle;
		JVX_PROCESS_ID myId;
		jvxSize numCores;
		std::vector<oneProcessObject> lstProcesses;
		std::vector<oneThreadObject> lstMyThreads;
		std::vector<oneRegisteredThread> lstRegThreads;
		jvxInt32 uniqueCnt;
		struct
		{
			jvxInt32 percent_non_time_critical;
			jvxInt32 timeout_refresh_ms;
		} config;

		struct
		{
			jvxSize numCoresRetain;
			jvxInt32 threadAffMaskEverythingElse;

		} derived;

		struct
		{
			jvxHandle* hdl;
			jvx_thread_handler cbs;
		} thread;

	} runtime;

	JVX_MUTEX_HANDLE safeAccessLists;

public:

	//! Constructor: Do nothing
	CjvxThreadController(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor
	~CjvxThreadController();

	// =====================================================================================
	// Interface API
	// =====================================================================================
	
	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef);

	//! Initialize the module, set size of buffer and strategy
	virtual jvxErrorType JVX_CALLINGCONVENTION number_cores(jvxSize* numCores);

	virtual jvxErrorType JVX_CALLINGCONVENTION configure(jvxInt32 percentNonTimeCritival, jvxInt32 timeout_refresh_ms);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();

	virtual jvxErrorType JVX_CALLINGCONVENTION register_tc_thread(jvxInt32* id_address);

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_tc_thread(jvxInt32 id_address);

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare();

	virtual jvxErrorType JVX_CALLINGCONVENTION associate_tc_thread(JVX_THREAD_ID theThread, jvxInt32 id_address);

	virtual jvxErrorType JVX_CALLINGCONVENTION post_process();

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

	//! Terminate
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate();


	// =====================================================================================

	
	jvxErrorType createProcessList(std::vector<oneProcessObject>& lst, std::string& err);

	jvxErrorType createThreadList(std::vector<oneThreadObject>& lst, std::string& err, JVX_PROCESS_ID idP);

	jvxErrorType updateProcessList_nolock(std::vector<oneProcessObject>& lst, jvxBool setMask, jvxInt32 mask);

	jvxErrorType updateThreadList_nolock(std::vector<oneThreadObject>& lst);

	static jvxErrorType static_wokeup_cb(jvxHandle* privateData_thread, jvxInt64 timestamp_us, jvxSize* delta_ms);
	jvxErrorType wokeup_cb(jvxInt64 timestamp_us);

#include "codeFragments/simplify/jvxObjects_simplify.h"

};

#endif
