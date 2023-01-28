#ifndef __CJVXMEXCALLS_PRV_H__
#define __CJVXMEXCALLS_PRV_H__

#include "jvx.h"
#include "common/CjvxProperties.h"
#include "pcg_exports_mexcall.h"
#include "CjvxMexCalls.h"

/**
 * Class to encapsulate mexcall functoionality for integration in various classes
 *///===============================================================================
class CjvxMexCalls_prv:  public genMexCall_node, public IjvxExternalCallTarget
{
	CjvxMexCalls& parent;
	//IjvxExternalCall* _theExtCallHandler = nullptr;
	//std::string _theExtCallObjectName;

	struct oneBufExchange
	{
		jvxHandle** buffer;
		jvxInt32 frameCnt;
	};

protected:

	//! States for thread synchronization
	typedef enum
	{
		LEAVE_OFFLINE,
		WAIT_FOR_EVENT,
		COPY_DIRECT,
		CALL_EXTERNAL
	} nextActionType;

	// Operation mode
	typedef enum
	{
		JVX_OPERATION_OFFLINE_CALLS,
		JVX_OPERATION_HOOKUP,
		JVX_OPERATION_NOT_SET
	} operationMode;

	//Buffer xchange states
	typedef enum
	{
		JVX_INPUT_OUTPUT_OFFLINE = 0,
		JVX_INPUT_OUTPUT_STARTED = 1,
		JVX_INPUT_OUTPUT_READY = 2
	} xChangeState;

	// Struct for data exchange between both threads in hook-up mode
	struct
	{
		// Create a local data processing chain for secondary (Matlab) thread
		jvxLinkDataDescriptor theData_from;
		jvxLinkDataDescriptor theData_to;

		jvxSize offlineStackDepth;
		oneBufExchange* oneBufferSet_input;
		oneBufExchange* oneBufferSet_output;
		jvxTick* oneTickSet_input;
		jvxTick* oneTickSet_output;

		jvxInt32 idxFromMatlab;
		jvxSize bFHeight;
		JVX_MUTEX_HANDLE safeAccessSamples;
		JVX_NOTIFY_HANDLE notificationNextData;
		xChangeState xState;
		jvxBool inMatlabLoop;
		jvxInt32 timeout_ms;
		JVX_THREAD_PRIORITY prioStart;

		jvxData time_theo;
		jvxTimeStampData time_ref;
	} exchangeInHookup;

	// Variables to store processing data
	struct
	{
		JVX_THREAD_ID idThread;
		operationMode theMode;
		jvxBool firstCall;
		jvxLinkDataDescriptor descrInUse;
		jvxLinkDataDescriptor descrOutUse;
		jvxSize frameExchangeCnt;
		jvxSize valid_ExchangeCnt;
	} runtime;

	struct
	{
		std::string description;
		jvxSize id;
		jvxSize level;
	} error;

	// Function call postfix
	std::string call_postfix;

	// Some cross references from driving object
	IjvxHiddenInterface* theHostRef;
	CjvxProperties* thePropsRef;
	IjvxReport* theReportRef;

	jvxSize local_slotid;
	jvxSize local_slotsubid;

public:
	
	CjvxMexCalls_prv(CjvxMexCalls& parRef);

	~CjvxMexCalls_prv();

	jvxErrorType select(IjvxHiddenInterface* theHost, CjvxProperties* theProps, const char* c_postfix = "", const char* component_specified = NULL);

	jvxErrorType unselect();

	jvxErrorType activate();

	jvxErrorType deactivate();
	
	jvxErrorType put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxState theState, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno);
	
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections);

	jvxErrorType prepare();
	
	jvxErrorType prepare_sender_to_receiver(jvxLinkDataDescriptor* theData);
	jvxErrorType prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData_in, 
		jvxLinkDataDescriptor* theData_out,
		const char** hintsTags = NULL, const char** hintsValues = NULL, jvxSize numHints = 0);

	jvxErrorType is_extcall_reference_present(jvxBool* isPresent);

	jvxErrorType is_matlab_processing_engaged(jvxBool* isEngaged);

	jvxErrorType process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out);
	
	jvxErrorType postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData);

	jvxErrorType before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData);

	jvxErrorType postprocess();

	jvxErrorType process_st_callEx(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out);

	// ==============================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION extcall_lasterror(jvxApiError* theErr)
	{
		if (theErr)
		{
			theErr->errorCode = JVX_SIZE_INT32(error.id);
			theErr->errorLevel = JVX_SIZE_INT32(error.level);
			theErr->errorDescription.assign(error.description);
		}
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION extcall_set(const char* descr, int level, int id)
	{
		error.description = descr;
		error.level = level;
		error.id = id;
		return(JVX_NO_ERROR);
	};

#include "mcg_exports_matlab_prototypes.h"

};

#endif