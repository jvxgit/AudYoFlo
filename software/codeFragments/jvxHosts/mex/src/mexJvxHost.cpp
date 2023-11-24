#include <string>

//#include <matrix.h>
#include <cassert>

#include "mexJvxHost.h"
#include "mexCmdMapperJvxHost.h"
#include "CjvxMatlabToCConverter.h"
#include "jvx-helpers.h"
#include "realtimeViewer_helpers.h"
extern "C"
{
#include "jvx_misc/jvx_printf.h"
};

#include "jvxHAppHost.h"
#ifdef CONFIG_COMPILE_FOR_MATLAB
#include "jvxTExternalCall_matlab.h"
#elif defined CONFIG_COMPILE_FOR_OCTAVE
#include "jvxTExternalCall_octave.h"
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif

#include "interfaces/all-hosts/jvxHostHookupEntries.h"

// Weak fuction to do some kind of pre configuration
extern "C"
{
#define FUNC_CORE_PROT_DECLARE jvx_preconfigure_host
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_preconfigure_host_local
#define FUNC_CORE_PROT_ARGS IjvxFactoryHost* fhost, jvxBool onBoot
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_preconfigure_host=jvx_preconfigure_host_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_preconfigure_host=_jvx_preconfigure_host_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		if (onBoot)
		{
			std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
			// Default implementation does just nothing
		}
		return JVX_NO_ERROR;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

}

static const char* jvx_external_datatype_lookup_category[] =
{
	"jvxComponentType", // 0
	"jvxSequencerQueueType", // 1
	"jvxSequenceElementType", // 2
	"jvxReportEventType", // 3
	"jvxFeatureClassType", // 4
	"jvxPropertyDecoderHintType", // 5
	"jvxPropertyAccessType", // 6
	"jvxPropertyCategoryType", // 7
	"jvxPropertySetType", // 8
	"jvxSequencerEvent", // 9
	"jvxSequencerStatus", // 10
	"jvxDataFormat", // 11
	"jvxState", //12
	"jvxConfigSectionTypes", //13
	"jvxErrorType", //14
	NULL
};

static const char* jvx_external_datatype_lookup_const[] =
{
	"jvxCompiledForDoublePrecision", // 0
	"jvxTypenameFormatData", // 1
	NULL
};

//=========================================================
/**
 * Constructor: Set all values to initialized/uninitialized values,
 * Constructor opens the used fileWriter/Reader library and sets all variables to initial.
 *///========================================================
mexJvxHost::mexJvxHost(void) :
	viewer_props(static_allocateData, static_deallocateData, static_copyData),
	viewer_plots(static_allocateData, static_deallocateData, static_copyData)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	
	CjvxPropertiesToMatlabConverter::converter = this;

	this->externalCall.theObj = NULL;
	this->externalCall.theGLobal = NULL;
	externalCall.fromHost.theObj = NULL;
	externalCall.fromHost.theHdl = NULL;
	msg_queue.callback_prefix = "jvx_mesgq_callback";

	runtime.numConfigureTokens = 0;
	runtime.ptrTokens_cchar = NULL;

	tpAll[JVX_COMPONENT_UNKNOWN].reset(JVX_COMPONENT_UNKNOWN);
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		tpAll[i].reset((jvxComponentType)i, 0, 0);
	}
}

//===========================================================
// ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY ENTRY
//===========================================================

/**
 * Main entry function: The most common response is to return a struct that contains all available information
 * getCurrentSetup.
 * This function is nothing else than a dispatcher that converts the input commands
 * into calls to predefined functions.
 *///=======================================================================================================
bool
mexJvxHost::parseInput(int nlhs, mxArray *plhs[],
 					int nrhs, const mxArray *prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	jvxSize i;

	// If calling the function without input arguments, the setup is returned
	if(nrhs == 0)
	{
		printUsage();
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if(nlhs > 1)
			this->mexReturnStructSetup(plhs[1]);
		return JVX_NO_ERROR;
	}
	else
	{
		// We have at least one input argument, this must be an ID
		JVX_HOST_COMMAND command;

		// Step I: Extract command ID from input argument
		arr = prhs[0];
		if(mxIsDouble(arr))
		{
			command = (JVX_HOST_COMMAND)((int)*((double*)mxGetData(arr)));
		}
		else if(mxIsSingle(arr))
		{
			command = (JVX_HOST_COMMAND)((int)*((float*)mxGetData(arr)));
		}
		else if(mxIsInt32(arr))
		{
			command = (JVX_HOST_COMMAND)*((int*)mxGetData(arr));
		}
		else if(mxIsChar(arr))
		{
			std::string token = CjvxMatlabToCConverter::jvx_mex_2_cstring(arr);
			bool foundit = false;
			for(i = 0; i < JVX_HOST_COMMAND_LIMIT; i++)
			{
				if(mapperCommands[i].cmdToken == token)
				{
					foundit = true;
					command = mapperCommands[i].cmdId;
					break;
				}
			}

			if(!foundit)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], "Input argument command specification " + token + " is unknown.", JVX_ERROR_INVALID_ARGUMENT);
				}
				return JVX_NO_ERROR;
			}
		}
		else
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument for function ID is of wrong type", JVX_ERROR_INVALID_ARGUMENT);
			}
			return JVX_NO_ERROR;
		}

		// Depending on the ID, different actions are performed:
		//==================================================================
		switch(command)
		{
			// === BOOTUP ===

		case JVX_HOST_COMMAND_INITIALIZE:
			res = this->initialize(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_ACTIVATE:
			res = this->activate(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_READ_CONFIG_FILE:			
			res = this->read_config_file(nlhs, plhs, nrhs, prhs);
			break; 

		case JVX_HOST_COMMAND_INFO:
			res = this->info(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_SET_CONFIG_ENTRY:
			res = this->set_config_entry(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_GET_CONFIG_ENTRY:
			res = this->get_config_entry(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_WRITE_CONFIG:
			res = this->write_config(nlhs, plhs, nrhs, prhs);
			break;
			// === COMPONENTS ===

		case JVX_HOST_COMMAND_SELECT_COMPONENT:
			res = this->select_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_ACTIVATE_COMPONENT:
			res = this->activate_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_ISREADY_COMPONENT:
			res = this->isready_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_STATUS_COMPONENT:
			res = this->status_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_CONDITIONS_COMPONENT:
			res = this->conditions_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_COMMANDS_COMPONENT:
			res = this->commands_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_RELATIVE_JUMS_COMPONENT:
			res = this->rel_jumps_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_DEACTIVATE_COMPONENT:
			res = this->deactivate_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_UNSELECT_COMPONENT:
			res = this->unselect_component(nlhs, plhs, nrhs, prhs);
			break;

			// === SEQUENCER ===

		case JVX_HOST_COMMAND_INFO_SEQUENCER:
			res = this->info_sequencer(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_ADD_SEQUENCE:
			res = this->add_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_MARK_SEQUENCE_DEFAULT:
			res = this->mark_sequence_default(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_REMOVE_SEQUENCE:
			res = this->remove_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_RESET_SEQUENCE:
			res = this->unselect_component(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_INSERT_STEP_SEQUENCE:
			res = this->insert_step_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_EDIT_STEP_SEQUENCE:
			res = this->edit_step_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_REMOVE_STEP_SEQUENCE:
			res = this->remove_step_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_SWITCH_STEP_SEQUENCE:
			res = this->switch_step_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_SET_ACTIVE_SEQUENCE:
			res = this->set_active_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_START_PROCESS_SEQUENCER:
			res = this->start_process_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_TRIGGER_STOP_PROCESS_SEQUENCER:
			res = this->trigger_stop_process(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_TRIGGER_STOP_SEQUENCE_SEQUENCER:
			res = this->trigger_stop_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_WAIT_COMPLETION_PROCESS_SEQUENCER:
			res = this->wait_completion_process(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_STATUS_PROCESS_SEQUENCER:
			res = this->status_process_sequence(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_TRIGGER_EXTERNAL_PROCESS:
			res = this->trigger_external_process(nlhs, plhs, nrhs, prhs);
			break;

			// === PROPERTIES ===

		case JVX_HOST_COMMAND_INFO_PROPERTIES:
			res = this->info_properties(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_LIST_PROPERTY_TAGS:
			res = this->list_property_tags(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_IDX:
			res = this->get_property_idx(nlhs, plhs, nrhs, prhs, false);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID:
			res = this->get_property_uniqueid(nlhs, plhs, nrhs, prhs, false);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR:
			res = this->get_property_descriptor(nlhs, plhs, nrhs, prhs, false);
			break;


		case JVX_HOST_COMMAND_SET_PROPERTY_IDX:
			res = this->set_property_idx(nlhs, plhs, nrhs, prhs, false);
			break;

		case JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID:
			res = this->set_property_uniqueid(nlhs, plhs, nrhs, prhs, false);
			break;

		case JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR:
			res = this->set_property_descriptor(nlhs, plhs, nrhs, prhs, false);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_IDX_OFF:
			res = this->get_property_idx(nlhs, plhs, nrhs, prhs, true);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_UNIQUE_ID_OFF:
			res = this->get_property_uniqueid(nlhs, plhs, nrhs, prhs, true);
			break;

		case JVX_HOST_COMMAND_GET_PROPERTY_DESCRIPTOR_OFF:
			res = this->get_property_descriptor(nlhs, plhs, nrhs, prhs, true);
			break;


		case JVX_HOST_COMMAND_SET_PROPERTY_IDX_OFF:
			res = this->set_property_idx(nlhs, plhs, nrhs, prhs, true);
			break;

		case JVX_HOST_COMMAND_SET_PROPERTY_UNIQUE_ID_OFF:
			res = this->set_property_uniqueid(nlhs, plhs, nrhs, prhs, true);
			break;

		case JVX_HOST_COMMAND_SET_PROPERTY_DESCRIPTOR_OFF:
			res = this->set_property_descriptor(nlhs, plhs, nrhs, prhs, true);
			break;

		case JVX_HOST_COMMAND_START_PROPERTY_GROUP:
			res = this->start_property_group(nlhs, plhs, nrhs, prhs, true);
			break;
		case JVX_HOST_COMMAND_STATUS_PROPERTY_GROUP:
			res = this->status_property_group(nlhs, plhs, nrhs, prhs, true);
			break;
		case JVX_HOST_COMMAND_STOP_PROPERTY_GROUP:
			res = this->stop_property_group(nlhs, plhs, nrhs, prhs, true);
			break;

			// === CONNECTION RULES ===

		case JVX_HOST_CONNECTION_LIST_RULES:
			res = this->list_connection_rules(nlhs, plhs, nrhs, prhs);
			break;
			
		case JVX_HOST_CONNECTION_LIST:
			res = this->list_connections(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_CONNECTION_ADD_RULE:
			res = this->add_connection_rule(nlhs, plhs, nrhs, prhs);
			break;			

		case JVX_HOST_CONNECTION_REM_RULE:
			res = this->rem_connection_rule(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_CONNECTION_TEST_CHAIN_MASTER:
			res = this->test_chain_master(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_CONNECTION_RESULT_TEST_CHAIN_MASTER:
			res = this->return_result_chain_master(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_CONNECTION_CHECK_READY_FOR_START:
			res = this->check_ready_for_start(nlhs, plhs, nrhs, prhs);
			break;

			// === EXTERNAL CALL ===

		case JVX_HOST_COMMAND_EXTERNAL_CALL:
			res = this->external_call(nlhs, plhs, nrhs, prhs);
			break;

			// === MESSAGE QUEUE ===

		case JVX_HOST_COMMAND_MSG_QUEUE_SET_CALLBACK_FUNCTION:
			res = this->set_callback_msgq(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_MSG_QUEUE_GET_CALLBACK_FUNCTION:
			res = this->get_callback_msgq(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_MSG_QUEUE_PROCESS_NEXT_MESSAGE:
			res = this->trigger_next_msgq(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_MSG_QUEUE_ADD_MESSAGE:
			res = this->add_message_msgq(nlhs, plhs, nrhs, prhs);
			break;

			// === SHUTDOWN ===

		case JVX_HOST_COMMAND_DEACTIVATE:
			res = this->deactivate(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_COMMAND_TERMINATE:
			res = this->terminate(nlhs, plhs, nrhs, prhs);
			break;

			// ======================

		case JVX_HOST_LOOKUP_TYPE_ID_4_NAME:
			res = this->lookup_type_id__name(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_LOOKUP_TYPE_NAME_4_VALUE:
			res = this->lookup_type_name__value(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_LOOKUP_CONST_4_NAME:
			res = this->lookup_const__name(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_EXPORT_CONFIG_STRING:
			res = this->export_config_string(nlhs, plhs, nrhs, prhs);
			break;

		case JVX_HOST_EXPORT_CONFIG_STRUCT:
			res = this->export_config_struct(nlhs, plhs, nrhs, prhs);
			break;
			
		case JVX_HOST_STRING_TO_CONFIG_STRUCT:
			res = this->convert_string_config_struct(nlhs, plhs, nrhs, prhs);
			break;

			/*		case JVX_HOST_IMPORT_CONFIG_STRING:
			res = this->import_config_string(nlhs, plhs, nrhs, prhs);
			break;
			*/
		default:

			// Out of range of fileReaderMatlab commands
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument does not hold a valid ID.", JVX_ERROR_ELEMENT_NOT_FOUND);
		}

		if(res != JVX_NO_ERROR)
		{
			std::string comToken = "unknown";
			for(i = 0; i < JVX_HOST_COMMAND_LIMIT; i++)
			{
				if(mapperCommands[i].cmdId == command)
				{
					comToken = mapperCommands[i].cmdToken;
					break;
				}
			}
			mexPrintf(("Unexpected Error in <jvxJvxHost>, command <" + comToken + ">, error type " + jvxErrorType_descr(res)).c_str());
			return false;
		}
	}
	return true;
}


/**
 * Function to print usage of the fileReaderMatlab as documentation reference.
 * This message is printed whenever the fileReaderMatlab is called without additional command argument.
 *///============================================================================
void
mexJvxHost::printUsage()
{
	jvxSize i;
	mexPrintf("-->Jvx reader for data logging tool created files. Developed by Javox Solutions GmbH\n");
	mexPrintf("-->The following commands can be specified:\n");

	for(i = 0; i < JVX_HOST_COMMAND_LIMIT; i++)
	{
		std::string line = "----> " + jvx_int2String(mapperCommands[i].cmdId) + ": " + mapperCommands[i].cmdToken + ": " + mapperCommands[i].cmdDescription;
		mexPrintf("%s\n", line.c_str());
	}
}

void
mexJvxHost::printText(std::string txt)
{
	mexPrintf("%s\n", txt.c_str());
}

int 
mexJvxHost::myMexPrintfC(jvxHandle* priv, const char* txt)
{
	mexJvxHost* me = (mexJvxHost*)priv;
	assert(me);
	return me->ic_myMexPrintfC(txt);
	
}

int
mexJvxHost::ic_myMexPrintfC(const char* txt)
{
	int res = 0;
	res = mexPrintf("%s", txt);
	return res;
}

jvxErrorType 
mexJvxHost::request_printf(the_jvx_printf* fPtr)
{
	if (fPtr)
	{
		*fPtr = jvxprintf;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::initialize(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;
	jvxComponentIdentification tp;
	IjvxProperties* theProps = NULL;
	jvxBool isValid = false;
	jvxSize id = 0;
	jvxCBool cout_mess = c_false;
	jvxApiString errMess;

	if (bootState == JVX_STATE_NONE)
	{
		// Store id of Matlab thread rto decide where to route messages etc.
		runtime.theMatlabThread = JVX_GET_CURRENT_THREAD_ID();

		std::vector<std::string> strList;

		for (i = 0; i < nrhs - 1; i++)
		{
			const mxArray* arr = prhs[i + 1];
			if (mxIsChar(arr))
			{
				strList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arr));
			}
			else if (mxIsStruct(arr))
			{
				// Browse through all elements
				int numFieldsPassed = mxGetNumberOfFields(arr);
				for (int ii = 0; ii < numFieldsPassed; ii++)
				{
					// Get reference to current subfield
					mxArray* arrWork = mxGetFieldByNumber(arr, 0, ii);
					if (!mxIsChar(arrWork))
					{
						strList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arrWork));
					}
				}
			}
			else if (mxIsCell(arr))
			{
				mexCellContentToStringList(arr, strList);
			}
			else
			{
				printText("Warning: invalid argument # " + jvx_size2String(i) + " in call of <jvxJvxHost::initialize>.");
			}
		}

		runtime.numConfigureTokens = (int)strList.size();

		runtime.ptrTokens_cchar = NULL;
		JVX_SAFE_NEW_FLD(runtime.ptrTokens_cchar, char*, runtime.numConfigureTokens);

		for (i = 0; i < runtime.numConfigureTokens; i++)
		{
			runtime.ptrTokens_cchar[i] = (char*)strList[i].c_str();
		}

		if (runtime.numConfigureTokens > 0)
		{
			commLine.assign_args((const char**)&runtime.ptrTokens_cchar[1], runtime.numConfigureTokens - 1, runtime.ptrTokens_cchar[0]);
		}
		else
		{
			commLine.assign_args((const char**)NULL, 0, "--unknown--");
		}

		res = boot_configure(&errMess, static_cast<IjvxCommandLine*>(&commLine), static_cast<IjvxReport*>(this), NULL, 
			static_cast<IjvxReportStateSwitch*>(this), NULL);
		if (res != JVX_NO_ERROR)
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Function <boot_configure> failed, reason: \n" + errMess.std_str(), res);
			}
			return res;
		}
		res = boot_initialize(&errMess, NULL);
		if (res == JVX_NO_ERROR)
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs, nlhs, 1);
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Function <boot_initialize> failed, reason: " + errMess.std_str(), JVX_ERROR_WRONG_STATE);
			}
		}

	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in initial state", JVX_ERROR_WRONG_STATE);
		}
	}



#if 0
	// =======================================================================================================
	if(involvedComponents.theHost.hFHost == NULL)
	{
		res = jvxHJvx_init(&jvxhost.theObj);
		if(jvxhost.theObj)
		{
			res = jvxhost.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&involvedComponents.theHost.hFHost), &tp, NULL);
		}

		if(res == JVX_NO_ERROR)
		{
			if(tp.tp == JVX_COMPONENT_HOST)
			{
				mexPrintf("##--> Jvx JvxHost is ready.\n");
			}
			else
			{
				mexPrintf("##--> Jvx JvxHost is not of right component type.\n");
			}
		}
		else
		{
			mexPrintf("##--> Jvx DataLogReader is NOT ready.\n");
		}
	}

	if(involvedComponents.theHost.hFHost)
	{
		res = initialize_queue(st_process_msg_callback, reinterpret_cast<jvxHandle*>(this));
		assert(res == JVX_NO_ERROR);
		res = start_queue();
		assert(res == JVX_NO_ERROR);
	}

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_NONE)
		{

			// Store id of Matlab thread rto decide where to route messages etc.
			runtime.theMatlabThread = JVX_GET_CURRENT_THREAD_ID();

			std::vector<std::string> strList;

			for(i = 0; i < nrhs-1; i++)
			{
				const mxArray* arr= prhs[i+1];
				if(mxIsChar(arr))
				{
					strList.push_back(jvx_mex_2_cstring(arr));
				}
				else if(mxIsStruct(arr))
				{
					// Browse through all elements
					int numFieldsPassed = mxGetNumberOfFields(arr);
					for(int ii = 0; ii < numFieldsPassed; ii++)
					{
						// Get reference to current subfield
						mxArray* arrWork = mxGetFieldByNumber(arr, 0, ii);
						if(!mxIsChar(arrWork))
						{
							strList.push_back(jvx_mex_2_cstring(arrWork));
						}
					}
				}
				else if(mxIsCell(arr))
				{
					mexCellContentToStringList(arr, strList);
				}
				else
				{
					printText("Warning: invalid argument # " + jvx_size2String(i) + " in call of <jvxJvxHost::initialize>.");
				}
			}

			runtime.numConfigureTokens = (int)strList.size();
			runtime.ptrTokens_cchar = NULL;

			JVX_SAFE_NEW_FLD(runtime.ptrTokens_cchar, char*, runtime.numConfigureTokens);

			for(i = 0; i < runtime.numConfigureTokens; i++)
			{
				runtime.ptrTokens_cchar[i] = (char*)strList[i].c_str();
			}

			CjvxCommandline::reset_command_line();
			CjvxCommandline::parse_command_line(runtime.ptrTokens_cchar,runtime.numConfigureTokens);

			if(!_command_line_parameters.changeWorkDir.empty())
			{
				JVX_CHDIR(_command_line_parameters.changeWorkDir.c_str());
			}

			// Setting redirected output
			jvx_set_printf(reinterpret_cast<jvxHandle*>(this), myMexPrintfC);

			// Initialize the host..
			this->involvedComponents.theHost.hFHost->initialize(NULL);
			this->involvedComponents.theHost.hFHost->select();

			if (_command_line_parameters.out_cout)
			{
				cout_mess = c_true;
			}

			res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
			if ((res == JVX_NO_ERROR) && theProps)
			{
				
				// Set the predefined property for dll path for subcomponents
				if(jvx_findPropertyDescriptor("jvx_component_path_str_list", &id, NULL, NULL, NULL))
				{
					res = jvx_pushPullPropertyStringList(_command_line_parameters.dllsDirectories, this->jvxhost.theObj, theProps, id, JVX_PROPERTY_CATEGORY_PREDEFINED, &isValid);
				}
				jvx_set_property(theProps, &cout_mess, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/host_output_cout");
				res = this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
			}

			this->involvedComponents.theHost.hFHost->set_external_report_target(static_cast<IjvxReport*>(this));

			this->involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxPrintf*>(this)), JVX_INTERFACE_PRINTF);

			// Project specific initialization
			this->initialize_specific();

			this->externalCall.theObj = NULL;

#ifdef CONFIG_COMPILE_FOR_MATLAB
			res = jvxTExternalCall_matlab_init(&this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
			res = jvxTExternalCall_octave_init(&this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
			if((res == JVX_NO_ERROR) && this->externalCall.theObj)
			{
					res = this->involvedComponents.theHost.hFHost->add_external_component(this->externalCall.theObj, "external call static object");
			}
			if(res != JVX_NO_ERROR)
			{
#ifdef CONFIG_COMPILE_FOR_MATLAB
				jvxTExternalCall_matlab_terminate(this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
				jvxTExternalCall_octave_terminate(this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
				this->externalCall.theObj = NULL;
			}
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs, nlhs, 1);
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in initial state", JVX_ERROR_WRONG_STATE);
			}
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in initial state", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
#endif
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::info(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::info>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if(nlhs > 1)
			{
				this->mexReturnStructSetup(plhs[1]);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::write_config(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::write_config>");
	jvxCallManagerConfiguration callConf;

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			std::string& fName = this->_command_line_parameters.configFilename;

			if(nrhs >= 2)
			{
				res = CjvxMatlabToCConverter::mexArgument2String(fName, prhs, 1, nrhs);
			}

			if(res == JVX_NO_ERROR)
			{
				cfg_filename_in_use = jvx_makeFilePath(_command_line_parameters.userPath, _command_line_parameters.configFilename);
				callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
				res = configureToFile(&callConf, cfg_filename_in_use);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to safe configuration to config file " + fName, res);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to extract filename from input arguments)." + fName, res);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::activate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxApiString errMess;

	res = boot_activate(&errMess, NULL);
	if(res == JVX_NO_ERROR)
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		mexFillEmpty(plhs, nlhs, 1);
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Call to <boot_activate> failed, reason: " + errMess.std_str(), JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::read_config_file(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::activate");
	jvxApiString errMess;

	res = boot_prepare(&errMess, NULL);
	if(res == JVX_NO_ERROR)
	{
#if 0 
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			// Read config file
			if (!this->_command_line_parameters.configFilename.empty())
			{
				// Error reporting happens from within the host by callback.
				res = this->configureFromFile(this->_command_line_parameters.configFilename, (jvxFlagTag)0);
				/*
				if(res == JVX_NO_ERROR)
				{
				mexPrintf("Successfully opened config file %s\n", this->_command_line_parameters.configFilename.c_str());

				}
				else
				{
				mexPrintf("Warning: Opening config file %s failed.\n", this->_command_line_parameters.configFilename.c_str());
				}
				*/
			}
#endif
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		mexFillEmpty(plhs, nlhs, 1);
	}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Call to <boot_prepare> failed, reason: " + errMess.std_str(), JVX_ERROR_WRONG_STATE);
			}
		}

	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::deactivate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::deactivate");
	jvxApiString errMess;

	shutdown_deactivate(&errMess, NULL);
	if(res == JVX_NO_ERROR)
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		mexFillEmpty(plhs, nlhs, 1);
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Call to <shutdown_deactivate> failed, reason: " + errMess.std_str(), JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::terminate(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::terminate");

	jvxState theState = JVX_STATE_NONE;
	jvxApiString errMess;

	res = shutdown_terminate(&errMess, NULL);
	if (res == JVX_NO_ERROR)
	{
		commLine.unassign_args();
		if (runtime.ptrTokens_cchar)
		{
			JVX_SAFE_DELETE_FLD(runtime.ptrTokens_cchar, char*);
			runtime.numConfigureTokens = 0;
		}

		bootState = JVX_STATE_NONE; // Inverse to boot_configure...
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		mexFillEmpty(plhs, nlhs, 1);
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "<shutdown_terminate> failed, reason: " + errMess.std_str(), JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
#if 0
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState > JVX_STATE_NONE)
		{
			// Project specific termination
			this->terminate_specific();

			this->involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxPrintf*>(this)), JVX_INTERFACE_PRINTF);

			if(externalCall.theObj)
			{
				this->involvedComponents.theHost.hFHost->remove_external_component(externalCall.theObj);
#ifdef CONFIG_COMPILE_FOR_MATLAB
				jvxTExternalCall_matlab_terminate(this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
				jvxTExternalCall_octave_terminate(this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
				this->externalCall.theObj = NULL;
			}

			this->involvedComponents.theHost.hFHost->unselect();
			this->involvedComponents.theHost.hFHost->terminate();

			if(runtime.ptrTokens_cchar)
			{
				JVX_SAFE_DELETE_FLD(runtime.ptrTokens_cchar, char*);
				runtime.numConfigureTokens = 0;
			}
		}
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		if(nlhs > 1)
		{
			this->mexReturnStructSetup(plhs[1]);
		}


		if(involvedComponents.theHost.hFHost)
		{
			stop_queue();
			terminate_queue();
		}

		if(involvedComponents.theHost.hFHost)
		{
			jvxHJvx_terminate(jvxhost.theObj);
		}

		_command_line_parameters.configFilename = "";
		_command_line_parameters.changeWorkDir = "";
		_command_line_parameters.dllsDirectories.clear();

		involvedComponents.theHost.hFHost = NULL;
		jvxhost.theObj = NULL;
		jvxhost.hTools = NULL;

		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], true);
		}
		mexFillEmpty(plhs, nlhs, 1);
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
#endif
	

	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::lookup_type_id__name(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize idx_lookup;
	jvxSize idx_entry;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::jvx_lookup_type_id__name");
	jvxState theState = JVX_STATE_NONE;

	std::string type_category;
	std::string type_name;

	jvxInt32 paramId = 1;
	bool parametersOk = true;
	bool detected_name_error = false;
	bool detected_lookup_error = false;
	jvxInt32 valI = -1;
	jvxCBitField valC = 0;
	bool return_options_lookup = true; 
	bool return_options_names = true;
	std::vector<std::string> strLst;

	if (nrhs > 1)
	{
		return_options_lookup = false;

		res = CjvxMatlabToCConverter::mexArgument2String(type_category, prhs, paramId, nrhs);
		if (res != JVX_NO_ERROR)
		{
			parametersOk = false;
		}

		if (nrhs > 2)
		{
			return_options_names = false;
			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2String(type_name, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				parametersOk = false;
			}
		}
	}

	if(parametersOk)
	{
		if (return_options_lookup)
		{
			i = 0;
			while (jvx_external_datatype_lookup_category[i])
			{
				strLst.push_back(jvx_external_datatype_lookup_category[i]);
				i++;
			}
			lookup_type_id__name_specific(strLst, type_category, type_name, nlhs, plhs, return_options_lookup, return_options_names);

			if (nlhs > 0)
				this->mexReturnBool(plhs[0], true);
			if (nlhs > 1)
				this->mexReturnStringCell(plhs[1], strLst);
			mexFillEmpty(plhs, nlhs, 2);
		}
		else
		{
			idx_lookup = 0;
			while (jvx_external_datatype_lookup_category[idx_lookup])
			{
				if (type_category == jvx_external_datatype_lookup_category[idx_lookup])
				{
					break;
				}
				idx_lookup++;
			}

			if (return_options_names)
			{
				switch (idx_lookup)
				{
				case 0:
					//"jvxComponentType",
					for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
					{
						strLst.push_back(jvxComponentType_txtf(i));
					}
					break;
				case 1:

					// "jvxSequencerQueueType",
					for (i = 0; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
					{
						strLst.push_back(jvxSequencerQueueType_str[i].full);
					}
					break;

				case 2:
					// "jvxSequenceElementType",
					for (i = 0; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
					{
						strLst.push_back(jvxSequencerElementType_str[i].full);
					}
					break;
				case 3:

					//"jvxReportEventType",
					for (i = 0; i < JVX_REPORT_REQUEST_LIMIT; i++)
					{
						strLst.push_back(jvxCommandRequestFlag_str[i].full);
					}
					break;

				case 4:

					//"jvxFeatureClassType",
					for (i = 0; i < JVXFEATURE_CLASS_NUM; i++)
					{
						strLst.push_back(jvxFeatureClass_str[i].full);
					}
					break;
				case 5:
					//"jvxPropertyDecoderHintType",
					for (i = 0; i < JVX_PROPERTY_DECODER_LIMIT; i++)
					{
						strLst.push_back(jvxPropertyDecoderHintType_str[i].full);
					}
					break;
				case 6:
					//"jvxPropertyAccessType",
					for (i = 0; i < JVX_PROPERTY_ACCESS_LIMIT; i++)
					{
						strLst.push_back(jvxPropertyAccessType_str[i].full);
					}
					break;
				case 7:
					//"jvxPropertyCategoryType",					
					for (i = 0; i < JVX_PROPERTY_CATEGORY_LIMIT; i++)
					{
						strLst.push_back(jvxPropertyCategoryType_str[i].full);
					}
					break;
				case 8:
					// "jvxPropertySetType",
					for (i = 0; i < JVXPROPERRTY_SETTYPE_NUM; i++)
					{
						strLst.push_back(jvxPropertySetType_str[i].full);
					}
					break;

				case 9:
					//"jvxSequencerEvent",
					for (i = 0; i < JVXSEQUENCEREVENT_NUM; i++)
					{
						strLst.push_back(jvxSequencerEventType_str[i].full);
					}
					break;
				case 10:
					//"jvxSequencerStatus",
					for (i = 0; i < JVX_SEQUENCER_STATUS_LIMIT; i++)
					{
						strLst.push_back(jvxSequencerStatus_str[i].full);
					}
					break;

				case 11:
					// "jvxDataFormat",
					for (i = 0; i < JVX_DATAFORMAT_LIMIT; i++)
					{
						strLst.push_back(jvxDataFormat_str[i].full);
					}
					break;
				case 12:
					// "jvxState",
					for (i = 0; i < JVXSTATE_NUM; i++)
					{
						strLst.push_back(jvxState_str[i].friendly);
					}
					break;
				case 13:
					// "jvxConfigSectionTypes"
					for (i = 0; i < JVXCONFIGSECTION_NUM; i++)
					{
						strLst.push_back(jvxConfigSection_str[i].friendly);
					}
					break;
				case 14:
					// "jvxConfigSectionTypes"
					for (i = 0; i < JVX_ERROR_LIMIT; i++)
					{
						strLst.push_back(jvxErrorType_str[i].friendly);
					}
					break;
				default:
					resL = lookup_type_id__name_specific(strLst, type_category, type_name, nlhs, plhs, return_options_lookup, return_options_names);
					if (resL != JVX_NO_ERROR)
					{
						detected_lookup_error = true;
					}
				}

				if (detected_lookup_error)
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Unrecognized lookup type specifier <" + type_category + ">.", JVX_ERROR_ELEMENT_NOT_FOUND);
					}
				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					if (nlhs > 1)
						this->mexReturnStringCell(plhs[1], strLst);
				}
				mexFillEmpty(plhs, nlhs, 2);
			} // if (return_options_names)
			else
			{
				switch (idx_lookup)
				{
				case 0:
					//"jvxComponentType",
					idx_entry = 0;
					while (idx_entry < JVX_COMPONENT_ALL_LIMIT)
					{
						if (
							(jvxComponentType_txtf(idx_entry) == type_name)||
							(jvxComponentType_txt(idx_entry) == type_name) )
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_COMPONENT_ALL_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 1:

					// "jvxSequencerQueueType",
					idx_entry = 0;
					while (idx_entry < JVX_SEQUENCER_QUEUE_TYPE_LIMIT)
					{
						if (
							(jvxSequencerQueueType_str[idx_entry].full == type_name)||
							(jvxSequencerQueueType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_SEQUENCER_QUEUE_TYPE_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;

				case 2:
					// "jvxSequenceElementType",
					idx_entry = 0;
					while (idx_entry < JVX_SEQUENCER_TYPE_COMMAND_LIMIT)
					{
						if (
							(jvxSequencerElementType_str[idx_entry].full == type_name)||
							(jvxSequencerElementType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_SEQUENCER_TYPE_COMMAND_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 3:

					//"jvxReportEventFlag",
					idx_entry = 0;
					while (idx_entry < JVX_REPORT_REQUEST_LIMIT)
					{
						if (
							(jvxCommandRequestFlag_str[idx_entry].full == type_name)||
							(jvxCommandRequestFlag_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_REPORT_REQUEST_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;

				case 4:

					//"jvxFeatureClassType",

					idx_entry = 0;
					while (idx_entry < JVXFEATURE_CLASS_NUM)
					{
						if (
							(jvxFeatureClass_str[idx_entry].full == type_name)||
							(jvxFeatureClass_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVXFEATURE_CLASS_NUM)
					{
						detected_name_error = true;
					}
					else
					{
						jvx_bitZSet(valC, idx_entry);
						if (nlhs > 1)
							this->mexReturnCBitField(plhs[1], valC);
					}
					break;
				case 5:
					//"jvxPropertyDecoderHintType",

					idx_entry = 0;
					while (idx_entry < JVX_PROPERTY_DECODER_LIMIT)
					{
						if (
							(jvxPropertyDecoderHintType_str[idx_entry].full == type_name)||
							(jvxPropertyDecoderHintType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_PROPERTY_DECODER_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 6:
					//"jvxPropertyAccessType",
					idx_entry = 0;
					while (idx_entry < JVX_PROPERTY_ACCESS_LIMIT)
					{
						if (
							(jvxPropertyAccessType_str[idx_entry].full == type_name)||
							(jvxPropertyAccessType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_PROPERTY_ACCESS_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 7:
					//"jvxPropertyCategoryType",
					idx_entry = 0;
					while (idx_entry < JVX_PROPERTY_CATEGORY_LIMIT)
					{
						if (
							(jvxPropertyCategoryType_str[idx_entry].full == type_name)||
							(jvxPropertyCategoryType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_PROPERTY_CATEGORY_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 8:
					// "jvxPropertySetType",
					idx_entry = 0;
					while (idx_entry < JVXPROPERRTY_SETTYPE_NUM)
					{
						if (
							(jvxPropertySetType_str[idx_entry].full == type_name)||
							(jvxPropertySetType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVXPROPERRTY_SETTYPE_NUM)
					{
						detected_name_error = true;
					}
					else
					{
						valC = jvxPropertySetType_map[idx_entry];
						if (nlhs > 1)
							this->mexReturnCBitField(plhs[1], valC);
					}
					break;

				case 9:
					//"jvxSequencerEvent",
					idx_entry = 0;
					while (idx_entry < JVXSEQUENCEREVENT_NUM)
					{
						if (
							(jvxSequencerEventType_str[idx_entry].full == type_name)||
							(jvxSequencerEventType_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVXSEQUENCEREVENT_NUM)
					{
						detected_name_error = true;
					}
					else
					{
						valC = jvxSequencerEventType_map[idx_entry];
						if (nlhs > 1)
							this->mexReturnCBitField(plhs[1], valC);
					}
					break;
				case 10:
					//"jvxSequencerStatus",
					idx_entry = 0;
					while (idx_entry < JVX_SEQUENCER_STATUS_LIMIT)
					{
						if (
							(jvxSequencerStatus_str[idx_entry].full == type_name)||
							(jvxSequencerStatus_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_SEQUENCER_STATUS_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;

				case 11:
					// "jvxDataFormat",
					idx_entry = 0;
					while (idx_entry < JVX_DATAFORMAT_LIMIT)
					{
						if (
							(jvxDataFormat_str[idx_entry].full == type_name)||
							(jvxDataFormat_str[idx_entry].friendly == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_DATAFORMAT_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 12:
					// "jvxState",
					idx_entry = 0;
					while (idx_entry < JVXSTATE_NUM)
					{
						if (
							(jvxState_str[idx_entry].friendly == type_name) ||
							(jvxState_str[idx_entry].full == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVXSTATE_NUM)
					{
						detected_name_error = true;
					}
					else
					{
						valC = jvxState_map[idx_entry];
						if (nlhs > 1)
							this->mexReturnCBitField(plhs[1], valC);
					}
					break;
				case 13:
					// "jvxState",
					idx_entry = 0;
					while (idx_entry < JVXCONFIGSECTION_NUM)
					{
						if (
							(jvxConfigSection_str[idx_entry].friendly == type_name) ||
							(jvxConfigSection_str[idx_entry].full == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVXCONFIGSECTION_NUM)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				case 14:
					// "jvxErrorType",
					idx_entry = 0;
					while (idx_entry < JVX_ERROR_LIMIT)
					{
						if (
							(jvxErrorType_str[idx_entry].friendly == type_name) ||
							(jvxErrorType_str[idx_entry].full == type_name))
						{
							break;
						}
						idx_entry++;
					}
					if (idx_entry == JVX_ERROR_LIMIT)
					{
						detected_name_error = true;
					}
					else
					{
						valI = JVX_SIZE_INT32(idx_entry);
						if (nlhs > 1)
							this->mexReturnInt32(plhs[1], valI);
					}
					break;
				default:
					detected_lookup_error = true;
				}

				if (detected_lookup_error)
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Unrecognized lookup type specifier <" + type_category + ">.", JVX_ERROR_ELEMENT_NOT_FOUND);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
				else
				{
					if (detected_name_error)
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Unrecognized lookup name <" + type_name + "> for lookup category <" + type_category + ">.", JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					else
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], true);
						mexFillEmpty(plhs, nlhs, 2);

					}
				}
			} // else : if (return_options_names)
		} // else: if (return_options_lookup)
	} // if(parametersOk)
	else
	{
		if (nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR; 
}

jvxErrorType 
mexJvxHost::lookup_type_name__value(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize idx_lookup;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxState theState = JVX_STATE_NONE;

	std::string type_category;
	std::string type_name;

	jvxInt32 paramId = 1;
	jvxSize id = JVX_SIZE_UNSELECTED;

	bool parametersOk = true;
	bool detected_id_error = false;
	bool detected_type_error = false;
	bool detected_lookup_error = false;
	std::string retVal = "not-a-unique-id";
	std::string bfld;
	bool inputIsSize = false;
	bool inputIsBitField = false;
	jvxBitField valC;
	jvxBitField cmpBtfld;

	res = CjvxMatlabToCConverter::mexArgument2String(type_category, prhs, paramId, nrhs);
	if (res != JVX_NO_ERROR)
	{
		parametersOk = false;
	}

	paramId = 2;
	res = CjvxMatlabToCConverter::mexArgument2Index(id, prhs, paramId, nrhs);
	if (res == JVX_NO_ERROR)
	{
		inputIsSize = true;
	}
	else
	{
		res = CjvxMatlabToCConverter::mexArgument2String(bfld, prhs, paramId, nrhs);
		{
			if (res == JVX_NO_ERROR)
			{
				jvxBool err = false;
				valC = jvx_string2BitField(bfld, err);
				if (err)
				{
					parametersOk = false;
					res = JVX_ERROR_PARSE_ERROR;
				}
				inputIsBitField = true;
			}
			else
			{
				parametersOk = false;
			}
		}
	}

	if (parametersOk)
	{
		idx_lookup = 0;
		while (jvx_external_datatype_lookup_category[idx_lookup])
		{
			if (type_category == jvx_external_datatype_lookup_category[idx_lookup])
			{
				break;
			}
			idx_lookup++;
		}

		switch (idx_lookup)
		{
		case 0:
			//"jvxComponentType",
			if (inputIsSize)
			{
				if (id < JVX_COMPONENT_ALL_LIMIT)
				{
					retVal = jvxComponentType_txtf(id);
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;
		case 1:

			// "jvxSequencerQueueType",
			if (inputIsSize)
			{
				if (id < JVX_SEQUENCER_QUEUE_TYPE_LIMIT)
				{
					retVal = jvxSequencerQueueType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 2:
			// "jvxSequenceElementType",
			if (inputIsSize)
			{
				if (id < JVX_SEQUENCER_TYPE_COMMAND_LIMIT)
				{
					retVal = jvxSequencerElementType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 3:

			//"jvxReportEventFlag",
			if (inputIsSize)
			{
				if (id < JVX_REPORT_REQUEST_LIMIT)
				{
					retVal = jvxCommandRequestFlag_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 4:

			//"jvxFeatureClassType",
			if (inputIsBitField)
			{
				detected_id_error = true;
				for (i = 0; i < JVXFEATURE_CLASS_NUM; i++)
				{
					jvxCBitField bf;
					jvx_bitZSet(bf, i);
					cmpBtfld.setValue(bf);
					if (cmpBtfld == valC)
					{
						detected_id_error = false;
						retVal = jvxFeatureClass_str[i].full;
						break;
					}
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 5:
			//"jvxPropertyDecoderHintType",
			if (inputIsSize)
			{
				if (id < JVX_PROPERTY_DECODER_LIMIT)
				{
					retVal = jvxPropertyDecoderHintType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;
			
		case 6:
			//"jvxPropertyAccessType",
			if (inputIsSize)
			{
				if (id < JVX_PROPERTY_ACCESS_LIMIT)
				{
					retVal = jvxPropertyAccessType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;
		case 7:
			//"jvxPropertyCategoryType",
			if (inputIsSize)
			{
				if (id < JVX_PROPERTY_CATEGORY_LIMIT)
				{
					retVal = jvxPropertyCategoryType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;
		case 8:
			// "jvxPropertySetType",
			if (inputIsBitField)
			{
				detected_id_error = true;
				for (i = 0; i < JVXPROPERRTY_SETTYPE_NUM; i++)
				{
					cmpBtfld.setValue(jvxPropertySetType_map[i]);
					if (cmpBtfld == valC)
					{
						detected_id_error = false;
						retVal = jvxPropertySetType_str[i].full;
						break;
					}
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 9:
			//"jvxSequencerEvent",
			if (inputIsBitField)
			{
				detected_id_error = true;
				for (i = 0; i < JVXSEQUENCEREVENT_NUM; i++)
				{
					cmpBtfld.setValue(jvxSequencerEventType_map[i]);
					if (cmpBtfld == valC)
					{
						detected_id_error = false;
						retVal = jvxSequencerEventType_str[i].full;
						break;
					}
				}
			}
			else
			{
				detected_type_error = true;
			}
		
			break;
		case 10:
			//"jvxSequencerStatus",
			if (inputIsSize)
			{
				if (id < JVX_SEQUENCER_STATUS_LIMIT)
				{
					retVal = jvxSequencerStatus_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 11:
			// "jvxDataFormat",
			if (inputIsSize)
			{
				if (id < JVX_DATAFORMAT_LIMIT)
				{
					retVal = jvxDataFormat_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 12:
			// "jvxState",
			if (inputIsBitField)
			{
				detected_id_error = true;
				for (i = 0; i < JVXSTATE_NUM; i++)
				{
					cmpBtfld.setValue(jvxState_map[i]);
					if (cmpBtfld == valC)
					{
						detected_id_error = false;
						retVal = jvxState_str[i].friendly;
						break;
					}
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;
		case 13:
			// "jvxConfigSectionType",
			if (inputIsSize)
			{
				if (id < JVXCONFIGSECTION_NUM)
				{
					retVal = jvxConfigSection_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		case 14:
			// "jvxErrorType"
			if (inputIsSize)
			{
				if (id < JVX_ERROR_LIMIT)
				{
					retVal = jvxErrorType_str[id].full;
				}
				else
				{
					detected_id_error = true;
				}
			}
			else
			{
				detected_type_error = true;
			}
			break;

		default:
			detected_lookup_error = true;
		}

		if (detected_lookup_error)
		{
			if (nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Unrecognized lookup type specifier <" + type_category + ">.", JVX_ERROR_ELEMENT_NOT_FOUND);
			}
		}
		else
		{
			if (detected_id_error)
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					if (inputIsBitField)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified bitfield is not among the recognized values.", JVX_ERROR_ELEMENT_NOT_FOUND);
					}
					else
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified value is out of limits of valid entries.", JVX_ERROR_ELEMENT_NOT_FOUND);
					}
				}
			}
			else
			{
				if (detected_type_error)
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						if (inputIsBitField)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "For the selected lookup specifier, a numeric value is expected.", JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						else
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "For the selected lookup specifier, a bitfield value (a string) is expected", JVX_ERROR_ELEMENT_NOT_FOUND);
						}
					}
				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					if (nlhs > 1)
						this->mexReturnString(plhs[1], retVal);

				}
			}
		}
		mexFillEmpty(plhs, nlhs, 2);

	} // if(parametersOk)
	else
	{
		if (nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::lookup_const__name(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize idx_lookup;
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::lookup_const__id");
	jvxState theState = JVX_STATE_NONE;

	std::string type_category;
	std::string type_name;

	jvxInt32 paramId = 1;
	bool parametersOk = true;
	bool detected_lookup_error = false;
	std::vector<std::string> strLst;

	if (nrhs == 1)
	{
		i = 0;
		while (jvx_external_datatype_lookup_const[i])
		{
			strLst.push_back(jvx_external_datatype_lookup_const[i]);
			i++;
		}

		lookup_const__name_specific(strLst, type_category, nlhs, plhs, true);

		if (nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if (nlhs > 1)
			this->mexReturnStringCell(plhs[1], strLst);
		mexFillEmpty(plhs, nlhs, 2);
	}
	else
	{
		res = CjvxMatlabToCConverter::mexArgument2String(type_category, prhs, paramId, nrhs);
		if (res != JVX_NO_ERROR)
		{
			parametersOk = false;
		}

		if (parametersOk)
		{
			idx_lookup = 0;
			while (jvx_external_datatype_lookup_const[idx_lookup] != NULL)
			{
				if (type_category == jvx_external_datatype_lookup_const[idx_lookup])
				{
					break;
				}
				idx_lookup++;
			}

			// ==============================================================

			switch (idx_lookup)
			{
			case 0:
				// jvxCompiledForDoublePrecision
				if (nlhs > 1)
				{
					this->mexReturnBool(plhs[1], (JVX_FLOAT_DATAFORMAT_ID == 1));
				}

				break;
			case 1:
				if (nlhs > 1)
				{
					this->mexReturnString(plhs[1], jvxDataFormat_str[JVX_DATAFORMAT_DATA].friendly);
				}
				break;
			default:
				resL = lookup_const__name_specific(strLst, type_category, nlhs, plhs, false);
				if (resL != JVX_NO_ERROR)
				{
					detected_lookup_error = true;
				}
			}

			if (detected_lookup_error)
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Unrecognized lookup type specifier <" + type_category + ">.", JVX_ERROR_ELEMENT_NOT_FOUND);
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				mexFillEmpty(plhs, nlhs, 2);
			}
		} // if (parametersOk)
		else
		{
			if (nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	} // else: if (nrhs == 1)
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::set_callback_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_callback_msgq");

	if(nrhs == 2)
	{
		if(mxIsChar(prhs[1]))
		{
			this->msg_queue.callback_prefix = CjvxMatlabToCConverter::jvx_mex_2_cstring(prhs[1]);
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs, nlhs, 1);
		}
		else
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Input argument is of wrong type, it should be <string>.", JVX_ERROR_INVALID_ARGUMENT);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Number of input argument is wrong, it should be 2.", JVX_ERROR_INVALID_ARGUMENT);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::get_callback_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::get_callback_msgq");
	if(nlhs > 0)
	{
		this->mexReturnBool(plhs[0], false);
	}
	if(nlhs > 1)
	{
		this->mexReturnString(plhs[1], msg_queue.callback_prefix);
	}
	mexFillEmpty(plhs, nlhs, 2);
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::trigger_next_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxBdxHost::trigger_next_msgq");

	if(involvedComponents.theHost.hFHost)
	{
		res = trigger_process_element_remove_queue();
		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs, nlhs, 1);
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call of exposed C function failed. Review console output for more information.", res);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::add_message_msgq(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxBdxHost::add_message_msgq");
	std::string err;
	if(involvedComponents.theHost.hFHost)
	{
		res = add_element_to_queue(prhs + 1, nrhs - 1, err);

		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs, nlhs, 1);
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call of exposed C function failed. Review console output for more information.", res);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::external_call(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxBdxHost::external_call");

	jvxState theState = JVX_STATE_NONE;

	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState > JVX_STATE_NONE)
		{
			if(externalCall.fromHost.theHdl)
			{
				int nlhsloc = JVX_MAX(nlhs - 1, 0);
				int nrhsloc = JVX_MAX(nrhs - 1, 0);
				res = ((IjvxExternalCall*)externalCall.fromHost.theHdl)->external_call(nlhsloc, (jvxExternalDataType**)(&plhs[1]), nrhsloc, (const jvxExternalDataType**)(&prhs[1]), 1, 1);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					// no mex fill empty here since the following arguments are filled from sub module
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call of exposed C function failed. Review console output for more information.", res);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "no valid reference to matlab call handler available.", JVX_ERROR_NOT_READY);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return JVX_NO_ERROR;
}


// ==========================================================================================
// ==========================================================================================

jvxErrorType
mexJvxHost::report_simple_text_message(const char* txt, jvxReportPriority prio)
{
	JVX_THREAD_ID idT = JVX_GET_CURRENT_THREAD_ID();
	if(idT == runtime.theMatlabThread)
	{
		std::string printme;
		if (prio > JVX_REPORT_PRIORITY_NONE)
		{
			printme = jvxReportPriority_txt(prio);
			printme += ": ";
		}
		printme += txt;
		this->printText(printme);
	}
	else
	{
		// create a message queue list
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize off, jvxSize num,  const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose)
{
		// Check config lines..
	jvxErrorType res = JVX_NO_ERROR;

	// Follow deployment of properties as specified in the configuration lines
	IjvxConfigurationLine* theConfigLine = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		// Config parameters to config line
		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLine));

		if((res == JVX_NO_ERROR) && theConfigLine)
		{
			theConfigLine->report_property_was_set(thisismytype, cat, propId, onlyContent, tpTo);

			// Report to update all UIs since important stuff has changed
			//report_internals_have_changed_outThread(tpFrom, theObj,  cat,  propId, onlyContent, tpTo);

			involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle*>(theConfigLine));
		}
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
	}
	return res;
}

jvxErrorType
mexJvxHost::report_take_over_property(const jvxComponentIdentification& tpFrom, IjvxObject* objRef,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpTo);
	jvxCallManagerProperties callGate;
	if(jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		res = theTriple.theProps->request_takeover_property(callGate, tpFrom, objRef,
			fld, numElements, format, offsetStart, onlyContent,
			cat, propId);
		jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpTo);
	}
	return res;
}

jvxErrorType
mexJvxHost::report_command_request(
	jvxCBitField request, 
	jvxHandle* caseSpecificData, 
	jvxSize szSpecificData)
{
	add_element_queue(request, caseSpecificData, szSpecificData, NULL, NULL);
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::request_command(const CjvxReportCommandRequest& request)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
mexJvxHost::interface_sub_report(IjvxSubReport** subReport) 
{
	return JVX_ERROR_UNSUPPORTED;
}


jvxErrorType
mexJvxHost::report_os_specific(jvxSize commandId, void* context)
{
	return JVX_ERROR_UNSUPPORTED;
}

// ==========================================================================================
// ==========================================================================================

jvxErrorType
mexJvxHost::report_event(jvxSequencerStatus stat, jvxCBitField event_mask, const char* description, jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp,
	jvxSequencerElementType stp, jvxSize fId, jvxSize current_state, jvxBool indicateFirstError)
{
	jvxBool reportMe = true;
	if(event_mask == JVX_SEQUENCER_EVENT_DEBUG_MESSAGE)
	{
		reportMe = false;
	}

	std::string eventDescr;
	jvxSize i;
	jvxCBitField checkme = 0;
	jvxBool warnOut = false;

	for(i = 0; i < JVXSEQUENCEREVENT_NUM; i++)
	{
		checkme = (jvxCBitField)1 << i;
		if(event_mask & checkme)
		{
			if(eventDescr.empty())
			{
				eventDescr = jvxSequencerEventType_str[i].full;
			}
			else
			{
				eventDescr += ", ";
				eventDescr += jvxSequencerEventType_str[i].full;
			}
		}
	}		

	if (
		(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_STEP_ERROR) ||
		(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ERROR) ||
		(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_SEQUENCE_ABORT) ||
		(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ERROR) ||
		(event_mask & JVX_SEQUENCER_EVENT_INCOMPLETE_COMPLETION_PROCESS_ABORT))
	{
		warnOut = true;
	}

	if (reportMe)
	{
		if (warnOut)
		{
			std::string txt;
			if (strlen(description))
				txt = "--> [SEQ<" + eventDescr + ">] " + description +"\n";
			else
				txt = "--> [SEQ<" + eventDescr + ">] <unknown reason>\n";

			//mexWarnMsgTxt(txt.c_str());
			mexPrintf(txt.c_str());
			
		}
		else
		{
			if (strlen(description))
				mexPrintf("[SEQ<%s>] %s\n", eventDescr.c_str(), description);
			else
				mexPrintf("[SEQ<%s>] <unknown reason>\n", eventDescr.c_str());
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::sequencer_callback(jvxSize sequenceId, jvxSize stepId, jvxSequencerQueueType tp, jvxSize functionId)
{
	mexPrintf("Sequencer callback\n");
	return(JVX_NO_ERROR);
}

//==========================================================================================
// MEX RETURN GENERIC RUNTIME CONFIGURATION MEX RETURN GENERIC RUNTIME CONFIGURATION MEX RETURN GENERIC RUNTIME CONFIGURATION
//==========================================================================================
// ================================================================================
// ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR
// ================================================================================

/**
 * Destructor: Set all values to initial, unload the library for fileReading and leave.
 *///======================================================================================
mexJvxHost::~mexJvxHost(void)
{
	this->terminate(0, NULL, 0,NULL);
}







jvxErrorType 
mexJvxHost::boot_initialize_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;

	// ===================================================================================================
	// Start queue
	res = initialize_queue(st_process_msg_callback, reinterpret_cast<jvxHandle*>(this));
	assert(res == JVX_NO_ERROR);
	res = start_queue();
	assert(res == JVX_NO_ERROR);

	// Setting redirected output
	jvx_set_printf(reinterpret_cast<jvxHandle*>(this), myMexPrintfC);

	this->involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxPrintf*>(this)), JVX_INTERFACE_PRINTF);

	// Configure slots
	JVX_START_SLOTS_BASE(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);
	JVX_START_SLOTS_SUBPRODUCT(theHostFeatures.numSlotsComponents, _command_line_parameters_hosttype.num_slots_max, _command_line_parameters_hosttype.num_subslots_max);

	jvx_configure_factoryhost_features(&theHostFeatures);

	// Project specific initialization via base init
	res = CjvxAppHostProduct::boot_initialize_base(theHostFeatures.numSlotsComponents);
	assert(res == JVX_NO_ERROR);

	res = this->initialize_specific(); 
	assert(res == JVX_NO_ERROR);

	// ========================================================================================
	// Some additional pre-configurations
	res = jvx_preconfigure_host(this->involvedHost.hHost, true);
	assert(res == JVX_NO_ERROR);

	// ===================================================================================================

	this->externalCall.theObj = NULL;

#ifdef CONFIG_COMPILE_FOR_MATLAB
	res = jvxTExternalCall_matlab_init(&this->externalCall.theObj, &this->externalCall.theGLobal, NULL);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
	res = jvxTExternalCall_octave_init(&this->externalCall.theObj, NULL);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
	if ((res == JVX_NO_ERROR) && this->externalCall.theObj)
	{
		res = this->involvedComponents.theHost.hFHost->add_external_component(this->externalCall.theObj, 
			this->externalCall.theGLobal, "external call static object");
	}
	if (res != JVX_NO_ERROR)
	{
#ifdef CONFIG_COMPILE_FOR_MATLAB
		jvxTExternalCall_matlab_terminate(this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
		jvxTExternalCall_octave_terminate(this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
		this->externalCall.theObj = NULL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::boot_specify_slots_specific()
{
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::boot_prepare_specific(jvxApiString* errloc) 
{
	activate_default_components_host(theHostFeatures.lst_ModulesOnStart, involvedHost.hHost, false, theHostFeatures.verbose_ModulesOnStart);
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::boot_start_specific(jvxApiString* errloc) 
{
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::boot_activate_specific(jvxApiString* errloc) 
{
	jvxErrorType res = JVX_NO_ERROR;
	// Tools interfaces
	externalCall.fromHost.theObj = NULL;
	externalCall.fromHost.theHdl = NULL;

	assert(involvedComponents.theTools.hTools);

	// Obtain referecne to external call tool for Testing
	res = involvedComponents.theTools.hTools->reference_tool(JVX_COMPONENT_EXTERNAL_CALL, &externalCall.fromHost.theObj, 0, NULL);
	if ((res == JVX_NO_ERROR) && externalCall.fromHost.theObj)
	{
		res = externalCall.fromHost.theObj->request_specialization(reinterpret_cast<jvxHandle**>(&externalCall.fromHost.theHdl), NULL, NULL);
		if ((res != JVX_NO_ERROR) || (externalCall.fromHost.theHdl == NULL))
		{
			involvedComponents.theTools.hTools->return_reference_tool(JVX_COMPONENT_EXTERNAL_CALL, externalCall.fromHost.theObj);
			externalCall.fromHost.theObj = NULL;
			externalCall.fromHost.theHdl = NULL;
		}
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::shutdown_terminate_specific(jvxApiString* errloc) 
{
	jvxErrorType res = JVX_NO_ERROR;
	if (this->externalCall.theObj)
	{
		this->involvedComponents.theHost.hFHost->remove_external_component(this->externalCall.theObj);

#ifdef CONFIG_COMPILE_FOR_MATLAB
		jvxTExternalCall_matlab_terminate(this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
		jvxTExternalCall_octave_terminate(this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
	}
	this->externalCall.theObj = NULL;

	res = jvx_preconfigure_host(this->involvedHost.hHost, false);
	assert(res == JVX_NO_ERROR);

	// Terminate specific
	terminate_specific();

	shutdown_terminate_base();

	this->involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxPrintf*>(this)), JVX_INTERFACE_PRINTF);

	if (externalCall.theObj)
	{
		this->involvedComponents.theHost.hFHost->remove_external_component(externalCall.theObj);
#ifdef CONFIG_COMPILE_FOR_MATLAB
		jvxTExternalCall_matlab_terminate(this->externalCall.theObj);
#elif defined CONFIG_COMPILE_FOR_OCTAVE
		jvxTExternalCall_octave_terminate(this->externalCall.theObj);
#else
#error "must define CONFIG_COMPILE_FOR_OCTAVE or CONFIG_COMPILE_FOR_MATLAB"
#endif
		this->externalCall.theObj = NULL;
	}

	
	stop_queue();
	terminate_queue();
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::shutdown_deactivate_specific(jvxApiString* errloc) 
{
	// Obtain referecne to external call tool for Testing
	if(externalCall.fromHost.theHdl)
	{
		 involvedComponents.theTools.hTools->return_reference_tool(JVX_COMPONENT_EXTERNAL_CALL, externalCall.fromHost.theObj);
	}
	externalCall.fromHost.theObj = NULL;
	externalCall.fromHost.theHdl = NULL;

	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::shutdown_postprocess_specific(jvxApiString* errloc) 
{
	deactivate_default_components_host(theHostFeatures.lst_ModulesOnStart, involvedHost.hHost, false);
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::shutdown_stop_specific(jvxApiString* errloc) 
{
	return JVX_NO_ERROR;
}

void 
mexJvxHost::report_text_output(const char* txt, jvxReportPriority prio, jvxHandle* context)
{
    this->report_simple_text_message(txt, prio);
}

void 
mexJvxHost::inform_config_read_start(const char* nmfile, jvxHandle* context) 
{
	std::string txt = "Reading configuration file ";
	txt += nmfile;
	txt += "\n";
	mexPrintf("%s", txt.c_str());
}

void 
mexJvxHost::inform_config_read_stop(jvxErrorType resRead, const char* nmFile, jvxHandle* context) 
{
	if (resRead != JVX_NO_ERROR)
	{
		std::string txt = "--> failed, reason: ";
		txt += jvxErrorType_descr(resRead);
		txt += ".\n";
		mexPrintf("%s", txt.c_str());
	}
}

void 
mexJvxHost::get_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
}

void
mexJvxHost::get_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfgProc, jvxConfigData* datTmp1)
{
	viewer_plots._get_configuration(callConf, cfgProc, datTmp1, involvedHost.hHost, static_getConfiguration, "plots");
	viewer_props._get_configuration(callConf, cfgProc, datTmp1, involvedHost.hHost, static_getConfiguration, "props");
}

void
mexJvxHost::put_configuration_specific(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* cfgProc, jvxConfigData* datTmp1, const char* fName)
{
	/*
 * Realtime viewer does not make sense in Matlab host. However, we handle it here to keep those entries in the
 * configuration file.*/
	std::vector<std::string> warnings;

	viewer_plots._clear();
	viewer_plots._put_configuration(callConf,cfgProc, datTmp1, involvedHost.hHost, fName, 0, warnings, static_putConfiguration, "plots");
	viewer_props._clear();
	viewer_props._put_configuration(callConf, cfgProc, datTmp1, involvedHost.hHost, fName, 0, warnings, static_putConfiguration, "props");
}

void
mexJvxHost::put_configuration_specific_ext(jvxCallManagerConfiguration* callConf, IjvxConfigurationExtender* ext)
{
}

void 
mexJvxHost::report_config_file_read_successful(jvxCallManagerConfiguration* callConf, const char* fName)
{
}

jvxErrorType 
mexJvxHost::pre_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
mexJvxHost::post_hook_stateswitch(jvxStateSwitch ss, const jvxComponentIdentification& tp, jvxErrorType suc)
{
	return JVX_NO_ERROR;
}

