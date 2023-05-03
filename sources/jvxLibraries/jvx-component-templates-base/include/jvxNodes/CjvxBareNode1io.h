#ifndef __CJVXBARENODE1IO_H__
#define __CJVXBARENODE1IO_H__

#include "CjvxNodeBase1io.h"
	
class CjvxBareNode1io: public CjvxNodeBase1io
{
protected:
	struct _common_set_node_base_1io_t
	{
		//! Setup if prepare is invoked automatically on prepare_chain_icon
		jvxBool prepareOnChainPrepare = true;

		//! Setup if start is invoked automatically on start_chain_icon
		jvxBool startOnChainStart = true;		
	};
	
	/**
	 * Struct to hold the flags to call the prepare function when the chaion is prepared. 
	 */
	_common_set_node_base_1io_t _common_set_node_base_1io;

	/**
	 * Variable to activate to check if input and output parameters are mostly identical. 
	 * Mostly identical means that all parameters are identical except for the number of channels
	 * on the input and output side.
	 */
	jvxBool checkInputOutputMostlyIdentical = true;

private:

	//! State variable to hold the status if prepare was called automatically
	jvxBool impPrepareOnChainPrepare = false;

	//! State variable to hold the status if start was called automatically
	jvxBool impStartOnChainStart = false;

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	

#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_OUTPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool operate_first_call,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock ,
		jvxHandle* priv_ptr ) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	
	// ===================================================================================================

#define JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_ICON
//#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#define JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
//#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#undef JVX_INPUT_OUTPUT_CONNECTOR_SET_OUTPUT_PARAMETERS
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#undef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#define JVX_INTERFACE_SUPPORT_MANIPULATE
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_LINKDATA_SLAVE
#undef JVX_INTERFACE_SUPPORT_MANIPULATE

	void prepare_autostart();
	void postprocess_autostart();

	void start_autostart();
	void stop_autostart();

};

#endif
