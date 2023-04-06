#include "jvx.h"
#include "CjvxMexCalls_prv.h"
#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "mcg_exports_matlab.h"
#endif

#include <iostream>
#define JVX_ALPHA_SMOOTH_NRT 0.95

CjvxMexCalls_prv::CjvxMexCalls_prv(CjvxMexCalls& parRef): parent(parRef)
{
	JVX_INITIALIZE_MUTEX(this->exchangeInHookup.safeAccessSamples);
	theHostRef = NULL;
	thePropsRef = NULL;
	theReportRef = NULL;
	runtime.theMode = JVX_OPERATION_NOT_SET;

	local_slotid = JVX_SIZE_UNSELECTED;
	local_slotsubid = JVX_SIZE_UNSELECTED;
}

CjvxMexCalls_prv::~CjvxMexCalls_prv()
{
	JVX_TERMINATE_MUTEX(this->exchangeInHookup.safeAccessSamples);
}

jvxErrorType
CjvxMexCalls_prv::select(IjvxHiddenInterface* theHost, CjvxProperties* theProps, const char* c_postfix, const char* componentName)
{
#ifdef JVX_EXTERNAL_CALL_ENABLED

	if (componentName)
		parent._theExtCallObjectName = componentName;

	parent._theExtCallHandler = NULL;
	requestExternalCallHandle(theHost, &parent._theExtCallHandler);
	if (!parent._theExtCallHandler)
		std::cerr << "[CjvxMexCalls_prv] WARNING: could not obtain mex call handler, disabling external call interface" << std::endl;
	initExternalCallhandler(parent._theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), parent._theExtCallObjectName);
	runtime.idThread = JVX_GET_CURRENT_THREAD_ID();

#else
#error "This module makes no sense if external access is inactive."
#endif
	call_postfix = c_postfix;
	thePropsRef = theProps;
	theHostRef = theHost;
	theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReportRef));

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::unselect()
{
#ifdef JVX_EXTERNAL_CALL_ENABLED
	terminateExternalCallhandler(parent._theExtCallHandler);
	returnExternalCallHandle(theHostRef, parent._theExtCallHandler);
	parent._theExtCallHandler = NULL;
	parent._theExtCallObjectName = "";

	theHostRef->return_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle*>(theReportRef));
	theReportRef = NULL;
	theHostRef = NULL;
	thePropsRef = NULL;
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::activate()
{
	genMexCall_node::init__properties_active();
	genMexCall_node::allocate__properties_active();
	genMexCall_node::register__properties_active(thePropsRef);

	genMexCall_node::init__properties_active_higher();
	genMexCall_node::allocate__properties_active_higher();
	genMexCall_node::register__properties_active_higher(thePropsRef);

	genMexCall_node::init__properties_common_object_name();
	genMexCall_node::allocate__properties_common_object_name();
	genMexCall_node::register__properties_common_object_name(thePropsRef);

	genMexCall_node::properties_common_object_name.object_name.value = parent._theExtCallObjectName;
	genMexCall_node::properties_active.hookupTriggerFunction.value = "triggerNextBatch";
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::deactivate()
{
	genMexCall_node::unregister__properties_common_object_name(thePropsRef);
	genMexCall_node::deallocate__properties_common_object_name();

	genMexCall_node::unregister__properties_active_higher(thePropsRef);
	genMexCall_node::deallocate__properties_active_higher();

	genMexCall_node::unregister__properties_active(thePropsRef);
	genMexCall_node::deallocate__properties_active();
	return JVX_NO_ERROR;
};

jvxErrorType
CjvxMexCalls_prv::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxState theState,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	if (theState == JVX_STATE_ACTIVE)
	{
		genMexCall_node::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = "<CjvxMexCalls> inherited by <";
				txt += parent._theExtCallObjectName + ">: ";
				txt += warnings[i];
				if (theReportRef)
				{
					theReportRef->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		genMexCall_node::put_configuration__properties_active_higher(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = "<CjvxMexCalls> inherited by <";
				txt += parent._theExtCallObjectName + ">: ";
				txt += warnings[i];
				if (theReportRef)
				{
					theReportRef->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		/*
		 * Why would I read this twice? It is only a copy paste bug right?
		genMexCall_node::put_configuration__properties_active(processor, sectionToContainAllSubsectionsForMe, flagtag, warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				if (theReportRef)
				{
					theReportRef->report_simple_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
				}
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		*/
	}
	return res;
}

jvxErrorType
CjvxMexCalls_prv::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{

	// Write all parameters from this class
	genMexCall_node::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	// Write all parameters from this class
	genMexCall_node::get_configuration__properties_active_higher(callConf, processor, sectionWhereToAddAllSubsections);

	// Write all parameters from this class
	genMexCall_node::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::prepare()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		if (JVX_EVALUATE_BITFIELD(genMexCall_node::properties_active.operationMode.value.selection() & 0x1))
		{
			runtime.theMode = JVX_OPERATION_OFFLINE_CALLS;
		}
		else
		{
			runtime.theMode = JVX_OPERATION_HOOKUP;
		}

		thePropsRef->_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY,
			genMexCall_node::properties_active.operationMode);

	}
	return res;
}

jvxErrorType
CjvxMexCalls_prv::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	if (runtime.theMode == JVX_OPERATION_OFFLINE_CALLS)
	{
		// Attach additional hint to data type
		theData->con_user.chain_spec_user_hints = jvx_hintDesriptor_push_front(
			&runtime.frameExchangeCnt, JVX_DATAPROCESSOR_HINT_DESCRIPTOR_ASYNC_FRAME_COUNT_INT32,
			jvxComponentIdentification( JVX_COMPONENT_EXTERNAL_CALL, local_slotid, local_slotsubid) ,
			theData->con_user.chain_spec_user_hints);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, const char** hintsTags, const char** hintsValues, jvxSize numHints)
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxInt32 valI32 = 0;
	jvxExternalDataType* copyToWs = NULL;
	std::string command;
	jvxSize i;
	jvxSize n = 0;
	jvxInt32 isOffline = 1;
	std::string additionalArg = "";

	jvxLinkDataDescriptor* theDataInCb = theData_in;

	// Copy the runtime data parameter settings
	runtime.descrInUse = *theData_in;
	runtime.descrOutUse = *theData_out;

	// BEGIN
	// We use the "old" method internally - hence, we need to prepare it!
	runtime.descrInUse.con_compat.buffersize =
		runtime.descrOutUse.con_params.buffersize;
	runtime.descrInUse.con_compat.format =
		runtime.descrOutUse.con_params.format;
	runtime.descrInUse.con_compat.from_receiver_buffer_allocated_by_sender =
		runtime.descrOutUse.con_data.buffers;
	runtime.descrInUse.con_compat.number_buffers =
		runtime.descrOutUse.con_data.number_buffers;
	runtime.descrInUse.con_compat.number_channels =
		runtime.descrOutUse.con_params.number_channels;
	runtime.descrInUse.con_compat.rate =
		runtime.descrOutUse.con_params.rate;
	runtime.descrInUse.con_compat.ext.segmentation_x =
		runtime.descrOutUse.con_params.segmentation.x;
	runtime.descrInUse.con_compat.ext.segmentation_y =
		runtime.descrOutUse.con_params.segmentation.y;
	runtime.descrInUse.con_compat.ext.subformat =
		runtime.descrOutUse.con_params.format_group;
	runtime.descrInUse.con_compat.ext.hints =
		runtime.descrOutUse.con_params.hints;
	// END
	
	// set the pointers to null values to avoid uncontrolled access
	runtime.descrInUse.con_data.buffers = NULL;
	runtime.descrInUse.con_compat.from_receiver_buffer_allocated_by_sender = NULL;

	runtime.frameExchangeCnt = 0;
	exchangeInHookup.inMatlabLoop = false;

	if (runtime.theMode == JVX_OPERATION_HOOKUP)
	{
		/*
		 * If we reach here, the pipeline has been completely created and theData_in is the link descriptor
		 * to drive this object.
		 * In hookup mode, we define a new "small" pipe which is only the Matlab function callback to produce
		 * output from input: exchangeInHookup.theData_from as the input, exchangeInHookup.theData_to
		 * as the output. This allows us to call the regular process function which is also in use by 
		 * the offline mode.
		 */
		genMexCall_node::properties_active_higher.hookupLostFrames.value = 0;

		isOffline = 0;

		/* Increase the priority: this makes sense if we take into account that the hookup procedure
		 * will be realized in the main thread and the current thread IS this main thread. */
		exchangeInHookup.prioStart = JVX_GET_THREAD_PRIORITY(JVX_GET_CURRENT_THREAD_HANDLE());
		JVX_SET_THREAD_PRIORITY(JVX_GET_CURRENT_THREAD_HANDLE(), JVX_THREAD_PRIORITY_REALTIME);

		// Set the callback timout
		exchangeInHookup.timeout_ms = genMexCall_node::properties_active.hookupTimeoutMs.value;

		// Set number of buffers in hookup exchange stack
		exchangeInHookup.offlineStackDepth = genMexCall_node::properties_active.hookupStackDepth.value;
		assert(exchangeInHookup.offlineStackDepth > 0);

		JVX_SAFE_NEW_FLD(this->exchangeInHookup.oneBufferSet_input, oneBufExchange, exchangeInHookup.offlineStackDepth);
		JVX_SAFE_NEW_FLD(this->exchangeInHookup.oneBufferSet_output, oneBufExchange, exchangeInHookup.offlineStackDepth);

		JVX_SAFE_NEW_FLD(exchangeInHookup.oneTickSet_input, jvxTick, exchangeInHookup.offlineStackDepth);
		JVX_SAFE_NEW_FLD(exchangeInHookup.oneTickSet_output, jvxTick, exchangeInHookup.offlineStackDepth);

		for (n = 0; n < exchangeInHookup.offlineStackDepth; n++)
		{
			exchangeInHookup.oneTickSet_input[n] = -1;
			exchangeInHookup.oneTickSet_output[n] = -1;

			this->exchangeInHookup.oneBufferSet_input[n].buffer = NULL;
			this->exchangeInHookup.oneBufferSet_input[n].frameCnt = -1;
			this->exchangeInHookup.oneBufferSet_output[n].buffer = NULL;
			this->exchangeInHookup.oneBufferSet_output[n].frameCnt = 0;

			if (runtime.descrInUse.con_params.number_channels > 0)
			{
				jvxHandle*** ptr_ptr = &this->exchangeInHookup.oneBufferSet_input[n].buffer;
				JVX_SAFE_NEW_FLD(*ptr_ptr, jvxHandle*, runtime.descrInUse.con_params.number_channels);
				for (i = 0; i < runtime.descrInUse.con_params.number_channels; i++)
				{
					JVX_SAFE_NEW_FLD((*ptr_ptr)[i], jvxByte,
						jvxDataFormat_size[runtime.descrInUse.con_params.format] * runtime.descrInUse.con_params.buffersize);
					memset((*ptr_ptr)[i], 0,
						jvxDataFormat_size[runtime.descrInUse.con_params.format] * runtime.descrInUse.con_params.buffersize);
				}
			}
			if (runtime.descrOutUse.con_params.number_channels > 0)
			{
				jvxHandle*** ptr_ptr = &this->exchangeInHookup.oneBufferSet_output[n].buffer;
				JVX_SAFE_NEW_FLD(*ptr_ptr, jvxHandle*, runtime.descrOutUse.con_params.number_channels);
				for (i = 0; i < runtime.descrOutUse.con_params.number_channels; i++)
				{
					JVX_SAFE_NEW_FLD((*ptr_ptr)[i], jvxByte,
						jvxDataFormat_size[runtime.descrOutUse.con_params.format] * runtime.descrOutUse.con_params.buffersize);
					memset((*ptr_ptr)[i], 0,
						jvxDataFormat_size[runtime.descrOutUse.con_params.format] * runtime.descrOutUse.con_params.buffersize);
				}
			}
		}

		jvx_initDataLinkDescriptor(&exchangeInHookup.theData_from);
		jvx_copyDataLinkDescriptor(&exchangeInHookup.theData_from, theData_in);
		exchangeInHookup.theData_from.con_data.number_buffers = 1;

		/*
		exchangeInHookup.theData_from.con_data.user_hints = jvx_hintDesriptor_push_front(
			&runtime.valid_ExchangeCnt, JVX_DATAPROCESSOR_HINT_DESCRIPTOR_ASYNC_FRAME_COUNT_INT32,
			jvxComponentIdentification(JVX_COMPONENT_EXTERNAL_CALL, local_slotid, local_slotsubid),
			exchangeInHookup.theData_from.con_data.user_hints);
		*/

		// Allocate memory
		jvx_allocateDataLinkDescriptor(&exchangeInHookup.theData_from, true);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(exchangeInHookup.theData_from.con_sync.reserve_timestamp, jvxTick,
			exchangeInHookup.theData_from.con_data.number_buffers);
		exchangeInHookup.theData_from.con_sync.type_timestamp = theData_in->con_sync.type_timestamp;

		jvx_initDataLinkDescriptor(&exchangeInHookup.theData_to);

		// Use the same output settings as the chained processing block
		jvx_copyDataLinkDescriptor(&exchangeInHookup.theData_to, theData_out);
		exchangeInHookup.theData_to.con_compat.number_buffers = 1;

		jvx_ccopyDataLinkDescriptor(&exchangeInHookup.theData_from, &exchangeInHookup.theData_to);

		// Allocate memory
		jvx_allocateDataLinkDescriptor(&exchangeInHookup.theData_to, true);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(exchangeInHookup.theData_to.con_sync.reserve_timestamp, jvxTick,
			exchangeInHookup.theData_to.con_compat.number_buffers);
		exchangeInHookup.theData_to.con_sync.type_timestamp = theData_in->con_sync.type_timestamp;

		// Cross link pointers
		exchangeInHookup.theData_from.con_compat.from_receiver_buffer_allocated_by_sender =
			exchangeInHookup.theData_to.con_data.buffers;

		// ==============================================================================

		exchangeInHookup.bFHeight = 0;
		exchangeInHookup.idxFromMatlab = 0;
		JVX_INITIALIZE_NOTIFICATION(exchangeInHookup.notificationNextData);
		exchangeInHookup.inMatlabLoop = false;

		exchangeInHookup.xState = JVX_INPUT_OUTPUT_STARTED;

		theDataInCb = &exchangeInHookup.theData_from;
	}

	exchangeInHookup.time_theo = (jvxData)runtime.descrInUse.con_params.buffersize / 
		(jvxData)runtime.descrInUse.con_params.rate;
	exchangeInHookup.time_theo *= 1e6; // sec to usec
	JVX_GET_TICKCOUNT_US_SETREF(&exchangeInHookup.time_ref);
	genMexCall_node::properties_active.hookupNtimeRT.value = -1;

	// In both modes, we will call the Matlab functions
#ifdef JVX_EXTERNAL_CALL_ENABLED
	if (parent._theExtCallHandler)
	{
		copyToWs = NULL;

		valI32 = (jvxInt32)theDataInCb->con_params.buffersize;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_bsize_in", copyToWs);

		copyToWs = NULL;
		valI32 = (jvxInt32)theDataInCb->con_params.rate;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_srate_in", copyToWs);

		copyToWs = NULL;
		valI32 = JVX_SIZE_INT32(theDataInCb->con_params.number_channels);
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_num_in", copyToWs);

		copyToWs = NULL;
		valI32 = (jvxInt32)theDataInCb->con_params.format;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_format_in", copyToWs);

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

		// valI32 = (jvxInt32)theDataInCb->con_compat.buffersize;
		valI32 = (jvxInt32)theData_out->con_params.buffersize;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_bsize_out", copyToWs);

		copyToWs = NULL;
		//valI32 = (jvxInt32)theDataInCb->con_compat.rate;
		valI32 = (jvxInt32)theData_out->con_params.rate;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_srate_out", copyToWs);

		copyToWs = NULL;
		// valI32 = JVX_SIZE_INT32(theDataInCb->con_compat.number_channels);
		valI32 = JVX_SIZE_INT32(theData_out->con_params.number_channels);
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_num_out", copyToWs);

		copyToWs = NULL;
		//valI32 = (jvxInt32)theDataInCb->con_compat.format;
		valI32 = (jvxInt32)theData_out->con_params.format;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_format_out", copyToWs);

		copyToWs = NULL;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, hintsTags, hintsValues, numHints);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_add_hints", copyToWs);

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

		copyToWs = NULL;
		valI32 = isOffline;
		parent._theExtCallHandler->convertCToExternal(&copyToWs, &valI32, 1, JVX_DATAFORMAT_32BIT_LE);
		parent._theExtCallHandler->putVariableToExternal("caller", "jvx_is_offline", copyToWs);

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

		if (genMexCall_node::properties_active.subFolderName.value.empty())
		{
#ifdef JVX_DATA_FORMAT_FLOAT
			command = genMexCall_node::properties_active.startScriptName.value + call_postfix +
				"(jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, 'single', jvx_add_hints);";
#else
			command = genMexCall_node::properties_active.startScriptName.value + call_postfix +
				"(jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, 'double', jvx_add_hints);";
#endif
		}
		else
		{

#ifdef JVX_DATA_FORMAT_FLOAT
			command = genMexCall_node::properties_active.subFolderName.value + call_postfix +
				"." + genMexCall_node::properties_active.startScriptName.value +
				"(jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, 'single', jvx_add_hints);";
#else
			command = genMexCall_node::properties_active.subFolderName.value + call_postfix +
				"." + genMexCall_node::properties_active.startScriptName.value +
				"(jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, 'double', jvx_add_hints);";
#endif
		}

		try
		{
			resL = parent._theExtCallHandler->executeExternalCommand(command.c_str());
			if (resL != JVX_NO_ERROR)
			{
				jvxApiString errDescr;
				parent._theExtCallHandler->getLastErrorReason(&errDescr);
				parent._theExtCallHandler->postMessageExternal(("Warning: Execution of command <" + command + "> failed, error: \n" + errDescr.std_str() + ".\n").c_str(), false);
				res = JVX_ERROR_ABORT;
			}
		}

		catch (...)
		{
			parent._theExtCallHandler->postMessageExternal(("Command <" + command + "> aborted.\n").c_str(), false);
			res = JVX_ERROR_ABORT;
		}
		runtime.firstCall = true;
	}
#endif // JVX_EXTERNAL_CALL_ENABLED

	return res;
}

/*
struct oneEntryDbgMe
{
	int operation;
	int val;
	int idx;
	int fh;
};
static oneEntryDbgMe fldBIdx[1000];
static int cntMe = 0;
*/
jvxErrorType
CjvxMexCalls_prv::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool notifyBufferConsumed = false;
	jvxTick tStamp = -1;

	if (runtime.theMode == JVX_OPERATION_HOOKUP)
	{
		/*
		  JVX_TRY_LOCK_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_MUTEX_SUCCESS;
		  JVX_TRY_LOCK_MUTEX(res, exchangeInHookup.safeAccessSamples);
		  if( res == JVX_TRY_LOCK_MUTEX_SUCCESS)
		*/

		JVX_LOCK_MUTEX(exchangeInHookup.safeAccessSamples);
		switch (exchangeInHookup.xState)
		{
		case JVX_INPUT_OUTPUT_STARTED:

			genMexCall_node::properties_active_higher.hookupLostFrames.value++;
			// Do nothing
			break;
		case JVX_INPUT_OUTPUT_READY:
		default:
			// JVX_INPUT_OUTPUT_READY
			if (exchangeInHookup.bFHeight > 0)
			{
				notifyBufferConsumed = true;
				jvxSize idxToDevice = (exchangeInHookup.idxFromMatlab + exchangeInHookup.offlineStackDepth - exchangeInHookup.bFHeight) % exchangeInHookup.offlineStackDepth;
				/*
				fldBIdx[cntMe].operation = 1;
				fldBIdx[cntMe].val = idxToDevice;
				fldBIdx[cntMe].idx = exchangeInHookup.idxFromMatlab;
				fldBIdx[cntMe].fh = exchangeInHookup.bFHeight;
				cntMe = (cntMe + 1) % 1000;
				if (cntMe == 0)
				{
					int a = 0;
				}*/
				jvxHandle** in = exchangeInHookup.oneBufferSet_input[idxToDevice].buffer;
				jvxHandle** out = exchangeInHookup.oneBufferSet_output[idxToDevice].buffer;
				exchangeInHookup.bFHeight--;

				// In kookup mode, copy to internal buffers ...
				for (i = 0; i < theData->con_params.number_channels; i++)
				{
					memcpy(in[i],
						theData->con_data.buffers[idx_sender_to_receiver][i],
						jvxDataFormat_size[theData->con_params.format] * theData->con_params.buffersize);
				}

				if (
					(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK) ||
					(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK))
				{
					tStamp = ((jvxTick*)theData->con_sync.reserve_timestamp)[idx_sender_to_receiver];
				}
				exchangeInHookup.oneTickSet_input[idxToDevice] = tStamp;

				// .. and copy from internal buffers
				for (i = 0; i < theData->con_compat.number_channels; i++)
				{
					memcpy(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][i],
						out[i],
						jvxDataFormat_size[theData->con_compat.format] * theData->con_compat.buffersize);
				}

				tStamp = exchangeInHookup.oneTickSet_output[idxToDevice];
				if (theData_out)
				{
					if (theData_out->con_sync.reserve_timestamp)
					{
						if (
							(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK) ||
							(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK))
						{
							((jvxTick*)theData_out->con_sync.reserve_timestamp)[idx_receiver_to_sender] = tStamp;
						}
					}
				}
				exchangeInHookup.oneBufferSet_input[idxToDevice].frameCnt = JVX_SIZE_INT32(runtime.frameExchangeCnt);
				exchangeInHookup.oneBufferSet_output[idxToDevice].frameCnt = JVX_SIZE_INT32(runtime.frameExchangeCnt);
					//CjvxAudioNode::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
			}
			else
			{
				genMexCall_node::properties_active_higher.hookupLostFrames.value++;
				exchangeInHookup.xState = JVX_INPUT_OUTPUT_STARTED;
			}
		}
		JVX_UNLOCK_MUTEX(exchangeInHookup.safeAccessSamples);

		if (notifyBufferConsumed)
		{
			JVX_SET_NOTIFICATION(exchangeInHookup.notificationNextData);
		}
		runtime.frameExchangeCnt++;

	}
	else
	{
		res = process_st_callEx(theData, idx_sender_to_receiver, idx_receiver_to_sender, theData_out);
		runtime.frameExchangeCnt++;
	}
	return res;
};

jvxErrorType
CjvxMexCalls_prv::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData_in)
{
	jvxSize n;
	jvxSize i;
	std::string command;
	jvxErrorType resL = JVX_NO_ERROR;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	if (parent._theExtCallHandler)
	{
		if (genMexCall_node::properties_active.subFolderName.value.empty())
		{
			command = genMexCall_node::properties_active.stopScriptName.value + call_postfix + "();";
		}
		else
		{
			command = genMexCall_node::properties_active.subFolderName.value +
				"." + genMexCall_node::properties_active.stopScriptName.value + call_postfix + "();";
		}

		try
		{
			resL = parent._theExtCallHandler->executeExternalCommand(command.c_str());
			if (resL != JVX_NO_ERROR)
			{
				jvxApiString errDescr;
				parent._theExtCallHandler->getLastErrorReason(&errDescr);
				parent._theExtCallHandler->postMessageExternal(("Warning: Execution of command <" + command + "> failed, error: " + errDescr.std_str() + ".\n").c_str(), false);
			}
		}

		catch (...)
		{
			parent._theExtCallHandler->postMessageExternal(("Command <" + command + "> aborted.\n").c_str(), false);
		}
	}
#endif // JVX_EXTERNAL_CALL_ENABLED

	thePropsRef->_undo_update_property_access_type(
		genMexCall_node::properties_active.operationMode);

	if (runtime.theMode == JVX_OPERATION_HOOKUP)
	{

		JVX_LOCK_MUTEX(exchangeInHookup.safeAccessSamples);
		exchangeInHookup.xState = JVX_INPUT_OUTPUT_OFFLINE;
		JVX_UNLOCK_MUTEX(exchangeInHookup.safeAccessSamples);

		// Notify Matlab side if necessary
		JVX_SET_NOTIFICATION(exchangeInHookup.notificationNextData);

		// Wait until Matlab code has been completed - should be single threaded,
		// this is all a little bit overhead
		while (exchangeInHookup.inMatlabLoop)
		{
			JVX_SLEEP_MS(100);
		}

		// Undo cross link pointers
		exchangeInHookup.theData_from.con_compat.from_receiver_buffer_allocated_by_sender = NULL;

		// Deallocate memory
		JVX_SAFE_DELETE_FIELD_TYPE(exchangeInHookup.theData_to.con_sync.reserve_timestamp, jvxTick);
		exchangeInHookup.theData_to.con_sync.type_timestamp = 0;

		jvx_deallocateDataLinkDescriptor(&exchangeInHookup.theData_to, true);

		// Reset content
		jvx_initDataLinkDescriptor(&exchangeInHookup.theData_to);

		// Deallocate memory
		JVX_SAFE_DELETE_FIELD_TYPE(exchangeInHookup.theData_from.con_sync.reserve_timestamp, jvxTick);
		exchangeInHookup.theData_from.con_sync.type_timestamp = 0;
		jvx_deallocateDataLinkDescriptor(&exchangeInHookup.theData_from, true);

		// Remove additional hints
		exchangeInHookup.theData_from.con_user.chain_spec_user_hints = jvx_hintDesriptor_pop_front(
			exchangeInHookup.theData_from.con_user.chain_spec_user_hints,
			NULL, NULL, NULL);

		// Reset content
		jvx_initDataLinkDescriptor(&exchangeInHookup.theData_from);

		// Next, deal with the data exchange buffers
		for (n = 0; n < exchangeInHookup.offlineStackDepth; n++)
		{
			if (runtime.descrInUse.con_params.number_channels > 0)
			{
				jvxHandle*** ptr_ptr = &this->exchangeInHookup.oneBufferSet_input[n].buffer;
				for (i = 0; i < runtime.descrInUse.con_params.number_channels; i++)
				{
					JVX_SAFE_DELETE_FLD((*ptr_ptr)[i], jvxByte);
				}
				JVX_SAFE_DELETE_FLD((*ptr_ptr), jvxHandle*);
			}
			if (runtime.descrOutUse.con_params.number_channels > 0)
			{
				jvxHandle*** ptr_ptr = &this->exchangeInHookup.oneBufferSet_output[n].buffer;
				for (i = 0; i < runtime.descrOutUse.con_params.number_channels; i++)
				{
					JVX_SAFE_DELETE_FLD((*ptr_ptr)[i], jvxByte);
				}
				JVX_SAFE_DELETE_FLD(*ptr_ptr, jvxHandle*);
			}
		}
		JVX_SAFE_DELETE_FLD(this->exchangeInHookup.oneBufferSet_input, oneBufExchange);
		JVX_SAFE_DELETE_FLD(this->exchangeInHookup.oneBufferSet_output, oneBufExchange);

		JVX_SAFE_DELETE_FIELD(exchangeInHookup.oneTickSet_input);
		JVX_SAFE_DELETE_FIELD(exchangeInHookup.oneTickSet_output);

		JVX_SET_THREAD_PRIORITY(JVX_GET_CURRENT_THREAD_HANDLE(), exchangeInHookup.prioStart);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxMexCalls_prv::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	if (runtime.theMode == JVX_OPERATION_OFFLINE_CALLS)
	{
		// Attach additional hint to data type
		theData->con_user.chain_spec_user_hints = jvx_hintDesriptor_pop_front(
			theData->con_user.chain_spec_user_hints, NULL, NULL, NULL);
	}
	return JVX_NO_ERROR;
}

jvxErrorType CjvxMexCalls_prv::postprocess()
{
	// Switch operation mode back to not read-only
	thePropsRef->_undo_update_property_access_type(
		genMexCall_node::properties_active.operationMode);

	return JVX_NO_ERROR;
}

jvxErrorType CjvxMexCalls_prv::is_extcall_reference_present(jvxBool* isPresent)
{
	if (isPresent)
	{
		*isPresent = (parent._theExtCallHandler != NULL);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType CjvxMexCalls_prv::is_matlab_processing_engaged(jvxBool* isEngaged)
{
	if (isEngaged)
	{
		*isEngaged = (genMexCall_node::properties_active.engageMatlabProcessing.value != 0);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxMexCalls_prv::process_st_callEx(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out)
{
	jvxExternalDataType* copyToWs = NULL;
	jvxExternalDataType* copyFromWs = NULL;
	std::string command;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool noOutput = false;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxBool isString = false;
	jvxInt32 dimX = 0;
	jvxInt32 dimY = 0;
	jvxBool convertFloats = false;
	jvxSize i;
	jvxInt32 tStamp = -1;

	jvxErrorType res = JVX_NO_ERROR;

	if (genMexCall_node::properties_active.hookupNtimeRT.value < 0)
	{
		genMexCall_node::properties_active.hookupNtimeRT.value = 0;
	}

	jvxTick tStart = JVX_GET_TICKCOUNT_US_GET_CURRENT(&exchangeInHookup.time_ref);

#ifdef JVX_EXTERNAL_CALL_ENABLED
	if (parent._theExtCallHandler)
	{
		if (
			!(
			(runtime.descrInUse.con_params.buffersize == theData->con_params.buffersize) &&
				(runtime.descrInUse.con_params.number_channels == theData->con_params.number_channels) &&
				(runtime.descrInUse.con_params.rate == theData->con_params.rate) &&
				(runtime.descrInUse.con_params.format == theData->con_params.format) &&

				(runtime.descrOutUse.con_params.buffersize == theData->con_compat.buffersize) &&
				(runtime.descrOutUse.con_params.number_channels == theData->con_compat.number_channels) &&
				(runtime.descrOutUse.con_params.rate == theData->con_compat.rate) &&
				(runtime.descrOutUse.con_params.format == theData->con_compat.format)
				))
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
		else
		{
			if (runtime.firstCall == true)
			{
				// Matlab call
				if (genMexCall_node::properties_active.subFolderName.value.empty())
				{
					if (runtime.theMode == JVX_OPERATION_OFFLINE_CALLS)
					{
						command = genMexCall_node::properties_active.beforeProcessScriptName.value + call_postfix + "( true);";
					}
					else
					{
						command = genMexCall_node::properties_active.beforeProcessScriptName.value + call_postfix + "( false);";
					}
				}
				else
				{
					command = genMexCall_node::properties_active.subFolderName.value +
						"." + genMexCall_node::properties_active.beforeProcessScriptName.value + "();";

					//if (runtime.theMode == JVX_OPERATION_OFFLINE_CALLS)
					//{
					//	command = genMexCall_node::properties_active.subFolderName.value +
					//		"." + genMexCall_node::properties_active.beforeProcessScriptName.value + "( true);";
					//}
					//else
					//{
					//	command = genMexCall_node::properties_active.subFolderName.value +
					//		"." + genMexCall_node::properties_active.beforeProcessScriptName.value + "( false);";
					//}
				}

				try
				{
					resL = parent._theExtCallHandler->executeExternalCommand(command.c_str());
					if (resL != JVX_NO_ERROR)
					{
						jvxApiString errDescr;
						parent._theExtCallHandler->getLastErrorReason(&errDescr);
						parent._theExtCallHandler->postMessageExternal(("Warning: Execution of command <" + command + "> failed, error: " + errDescr.std_str() + ".\n").c_str(), false);
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
						noOutput = true;
					}
				}
				catch (...)
				{
					parent._theExtCallHandler->postMessageExternal(("Command <" + command + "> aborted.\n").c_str(), false);
					error.description = "Command aborted.";
					res = JVX_ERROR_ABORT;
				}


				runtime.firstCall = false;
			}

			copyToWs = NULL;
			parent._theExtCallHandler->convertCToExternal(&copyToWs, (const jvxHandle**)theData->con_data.buffers[idx_sender_to_receiver],
				(jvxInt32)theData->con_params.number_channels, (jvxInt32)theData->con_params.buffersize, theData->con_params.format);
			parent._theExtCallHandler->putVariableToExternal("caller", "jvx_in_frame", copyToWs);

			copyToWs = NULL;
			// Get additional argument frame counter
		
			tStamp = -1;
			if (theData->con_sync.reserve_timestamp)
			{
				if (
					(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK) ||
					(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK))
				{
					tStamp = ((jvxTick*)theData->con_sync.reserve_timestamp)[0];
				}
			}

			if (theData_out)
			{
				if (theData_out->con_sync.reserve_timestamp)
				{
					if (
						(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK) ||
						(theData->con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK))
					{
						((jvxTick*)theData_out->con_sync.reserve_timestamp)[0] = tStamp;
					}

				}
			}
			/*
			jvxInt32* fcountAsyc_ptr = NULL;
			jvxErrorType resL = jvx_hintDesriptor_find(theData->con_user.chain_spec_user_hints,
				reinterpret_cast<jvxHandle**>(&fcountAsyc_ptr), JVX_DATAPROCESSOR_HINT_DESCRIPTOR_ASYNC_FRAME_COUNT_INT32, 
				jvxComponentIdentification(JVX_COMPONENT_UNKNOWN, JVX_SIZE_DONTCARE, JVX_SIZE_DONTCARE));
			if(resL == JVX_NO_ERROR)
			{
				tmp = *fcountAsyc_ptr;
			}
			*/
			parent._theExtCallHandler->convertCToExternal(&copyToWs, &tStamp, 1, JVX_DATAFORMAT_32BIT_LE);
			parent._theExtCallHandler->putVariableToExternal("caller", "jvx_xchg_cnt", copyToWs);

			// Matlab call
			if (genMexCall_node::properties_active.subFolderName.value.empty())
			{
				command = "[ jvx_out_frame] = " + genMexCall_node::properties_active.processScriptName.value + "( jvx_in_frame, jvx_xchg_cnt);";
			}
			else
			{
				command = "[ jvx_out_frame] = " + genMexCall_node::properties_active.subFolderName.value +
					"." + genMexCall_node::properties_active.processScriptName.value + "( jvx_in_frame, jvx_xchg_cnt);";
			}

			try
			{
				resL = parent._theExtCallHandler->executeExternalCommand(command.c_str());
				if (resL != JVX_NO_ERROR)
				{
					jvxApiString errDescr;
					parent._theExtCallHandler->getLastErrorReason(&errDescr);
					parent._theExtCallHandler->postMessageExternal(("Warning: Execution of command <" + command + "> failed, error: " + errDescr.std_str() + ".\n").c_str(), false);
					res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
					noOutput = true;
					res = JVX_ERROR_INTERNAL;
				}
				else
				{
					copyFromWs = NULL;
					resL = parent._theExtCallHandler->getVariableFromExternal("caller", "jvx_out_frame", &copyFromWs);
					if (resL == JVX_NO_ERROR)
					{
						resL = parent._theExtCallHandler->getPropertiesVariable(copyFromWs, &format, &isString, &dimX, &dimY, &convertFloats);

						if (resL == JVX_NO_ERROR)
						{
							if (theData->con_compat.number_channels > 0)
							{
								if (
									(format == theData->con_compat.format) &&
									(isString == false) &&
									(dimX == theData->con_compat.buffersize) &&
									(dimY == theData->con_compat.number_channels))
								{
									res = JVX_NO_ERROR;
									parent._theExtCallHandler->convertExternalToC(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
										dimY, dimX, format, copyFromWs, "jvx_out_frame", convertFloats);
								}
								else
								{
									res = JVX_ERROR_INVALID_SETTING;
									std::string txt = "Warning: Properties of processing output do ot match those expected.\n";
									if (format != theData->con_compat.format)
									{
										txt += "Expected format ";
										txt += jvxDataFormat_txt(theData->con_compat.format);
										txt += " but got ";
										txt += jvxDataFormat_txt(format);
										txt += ".\n";
									}
									if (isString)
									{
										txt += "Expected a numeric buffer but got a string. \n";
									}
									if (dimX != theData->con_compat.buffersize)
									{
										txt += "Expected buffersize  ";
										txt += jvx_size2String(theData->con_compat.buffersize);
										txt += " but got ";
										txt += jvx_size2String(dimX);
										txt += ".\n";

									}
									if (dimY != theData->con_compat.number_channels)
									{
										txt += "Expected a number of channels of  ";
										txt += jvx_size2String(theData->con_compat.number_channels);
										txt += " but got ";
										txt += jvx_size2String(dimY);
										txt += ".\n";
									}

									parent._theExtCallHandler->postMessageExternal(txt.c_str(), false);
									noOutput = true;
								}
							}
						}
						else
						{
							res = JVX_ERROR_INVALID_SETTING;
							std::string txt = "Warning: Could not get properties of variable <jvx_out_frame>.\n";							
							parent._theExtCallHandler->postMessageExternal(txt.c_str(), false);
							noOutput = true;
						}
					}
					else
					{
						res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;

						parent._theExtCallHandler->postMessageExternal("Warning: Failed to get processing output from worksapce.\n", false);
						noOutput = true;
					}
				}
			}

			catch (...)
			{
				parent._theExtCallHandler->postMessageExternal(("Command <" + command + "> aborted.\n").c_str(), false);
				res = JVX_ERROR_ABORT;
			}

			if (noOutput)
			{
				for (i = 0; i < theData->con_compat.number_channels; i++)
				{
					memset(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][i], 0, jvxDataFormat_size[theData->con_compat.format] * theData->con_compat.buffersize);
				}
			}
		}
	}
#endif // JVX_EXTERNAL_CALL_ENABLED

	jvxTick tStop = JVX_GET_TICKCOUNT_US_GET_CURRENT(&exchangeInHookup.time_ref);
	jvxData delta = (jvxData)tStop - (jvxData)tStart;
	delta = delta / exchangeInHookup.time_theo;

	genMexCall_node::properties_active.hookupNtimeRT.value = JVX_ALPHA_SMOOTH_NRT * genMexCall_node::properties_active.hookupNtimeRT.value + (1 - JVX_ALPHA_SMOOTH_NRT) * delta;

	return res;
}

#ifdef JVX_EXTERNAL_CALL_ENABLED

jvxErrorType
CjvxMexCalls_prv::triggerNextBatch()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataFormat formatIn = JVX_DATAFORMAT_NONE;
	jvxSize i;
	jvxInt32 dimXIn = 0;
	jvxInt32 dimYIn = 0;
	nextActionType nextAction = LEAVE_OFFLINE;
	jvxBool contLoop = true;

	if (!parent._theExtCallHandler)
		return JVX_ERROR_UNSUPPORTED;

	exchangeInHookup.inMatlabLoop = true;

	if (runtime.theMode == JVX_OPERATION_HOOKUP)
	{
		while (contLoop)
		{
			JVX_LOCK_MUTEX(exchangeInHookup.safeAccessSamples);
			if (exchangeInHookup.xState == JVX_INPUT_OUTPUT_OFFLINE)
			{
				nextAction = LEAVE_OFFLINE;
			}
			else
			{
				if (exchangeInHookup.bFHeight == exchangeInHookup.offlineStackDepth)
				{
					nextAction = WAIT_FOR_EVENT;
				}
				else
				{
					nextAction = COPY_DIRECT;
				}
			}

			if (nextAction == COPY_DIRECT)
			{
				// Do the copy
				/*
				fldBIdx[cntMe].operation = 2;
				fldBIdx[cntMe].val = exchangeInHookup.idxFromMatlab;
				fldBIdx[cntMe].idx = exchangeInHookup.idxFromMatlab;
				fldBIdx[cntMe].fh = exchangeInHookup.bFHeight;

				cntMe = (cntMe + 1) % 1000;
				if (cntMe == 0)
				{
					int a = 0;
				}*/
				// Step I: Prepare the output
				jvxTick tStamp = -1;
				if (exchangeInHookup.theData_to.con_sync.reserve_timestamp)
				{
					if (exchangeInHookup.theData_to.con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK)
					{
						jvxTick* ptr = (jvxTick*)exchangeInHookup.theData_to.con_sync.reserve_timestamp;
						tStamp = ptr[0];
					}
				}
				exchangeInHookup.oneTickSet_output[exchangeInHookup.idxFromMatlab] = tStamp;

				for (i = 0; i < runtime.descrOutUse.con_params.number_channels; i++)
				{
					memcpy(exchangeInHookup.oneBufferSet_output[exchangeInHookup.idxFromMatlab].buffer[i],
						exchangeInHookup.theData_from.con_compat.from_receiver_buffer_allocated_by_sender[0][i],
						jvxDataFormat_size[runtime.descrOutUse.con_params.format] * runtime.descrOutUse.con_params.buffersize);
				}
				
				// =========================================================================================
				tStamp = exchangeInHookup.oneTickSet_input[exchangeInHookup.idxFromMatlab];
				if (exchangeInHookup.theData_from.con_sync.reserve_timestamp)
				{
					if (exchangeInHookup.theData_from.con_sync.type_timestamp == JVX_DATA_SYNC_FRAME_COUNT_TICK)
					{
						jvxTick* ptr = (jvxTick*)exchangeInHookup.theData_from.con_sync.reserve_timestamp;
						ptr[0] = tStamp;
					}
				}
				// Step II: Copy the input
				for (i = 0; i < runtime.descrInUse.con_params.number_channels; i++)
				{
					memcpy(exchangeInHookup.theData_from.con_data.buffers[0][i],
						exchangeInHookup.oneBufferSet_input[exchangeInHookup.idxFromMatlab].buffer[i],
						jvxDataFormat_size[runtime.descrInUse.con_params.format] * runtime.descrInUse.con_params.buffersize);
				}

				// Copy frame counter
				runtime.valid_ExchangeCnt = exchangeInHookup.oneBufferSet_input[exchangeInHookup.idxFromMatlab].frameCnt;

				exchangeInHookup.idxFromMatlab = (exchangeInHookup.idxFromMatlab + 1) % exchangeInHookup.offlineStackDepth;
				exchangeInHookup.bFHeight++;
				if (exchangeInHookup.xState == JVX_INPUT_OUTPUT_STARTED)
				{
					if (exchangeInHookup.bFHeight == exchangeInHookup.offlineStackDepth)
					{
						// Switch into running state..
						exchangeInHookup.xState = JVX_INPUT_OUTPUT_READY;
					}
				}
				nextAction = CALL_EXTERNAL;
			}

			// Step III: Leave critical section
			JVX_UNLOCK_MUTEX(exchangeInHookup.safeAccessSamples);

			switch (nextAction)
			{
			case LEAVE_OFFLINE:
				contLoop = false;
				break;

			case CALL_EXTERNAL:

				// Step IV: Call Matlab callback
				res = this->process_st_callEx(&exchangeInHookup.theData_from, 0, 0, &exchangeInHookup.theData_to);
				contLoop = false;
				break;
			case WAIT_FOR_EVENT:

				// We may lose one batch or the other
				JVX_WAIT_FOR_NOTIFICATION_I(exchangeInHookup.notificationNextData);
				JVX_WAIT_RESULT resW = JVX_WAIT_FOR_NOTIFICATION_II_MS(exchangeInHookup.notificationNextData, exchangeInHookup.timeout_ms);
				if (resW != JVX_WAIT_SUCCESS)
				{
					res = JVX_ERROR_TIMEOUT;
					contLoop = false;
				}
				break;
			}
		}// while(contLoop)
	} // if(runtime.theMode == JVX_OPERATION_HOOKUP)
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}

	exchangeInHookup.inMatlabLoop = false;
	return res;
}

jvxErrorType
CjvxMexCalls_prv::askForNextBatch(jvxInt16* paramOut0)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataFormat formatIn = JVX_DATAFORMAT_NONE;
	jvxInt32 dimXIn = 0;
	jvxInt32 dimYIn = 0;
	nextActionType nextAction = LEAVE_OFFLINE;
	jvxBool contLoop = true;

	if (!parent._theExtCallHandler)
		return JVX_ERROR_UNSUPPORTED;

	*paramOut0 = 0;

	if (runtime.theMode == JVX_OPERATION_HOOKUP)
	{
		if (exchangeInHookup.xState != JVX_INPUT_OUTPUT_OFFLINE)
		{
			if (exchangeInHookup.bFHeight != exchangeInHookup.offlineStackDepth)
			{
				// Space is available for next buffer switch
				*paramOut0 = 1;
			}
		}
	}
	else // if(runtime.theMode == JVX_OPERATION_HOOKUP)
	{
		res = JVX_ERROR_INVALID_SETTING;
	}

	return res;
}
#endif
