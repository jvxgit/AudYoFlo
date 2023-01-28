#ifndef __CJVXAUDIONODE_H__
#define __CJVXAUDIONODE_H__

#include "CjvxAudioNodeBase.h"

class CjvxBareAudioNode: public CjvxAudioNodeBase
{
public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxBareAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;

#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

#ifdef ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_OUTPUT
#pragma error ACCEPT_ONLY_ACCEPT_MATCH_DATA_STRUCT_INPUT
#endif

	// ===================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset = 0, 
		jvxSize* idx_stage = NULL,
		jvxSize tobeAccessedByStage = 0,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage = JVX_SIZE_UNSELECTED, jvxBool shift_fwd = true,
		jvxSize tobeAccessedByStage = 0,
		callback_process_stop_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	
	// ===================================================================================================

#define JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#define JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#include "codeFragments/simplify/jvxInputOutputConnector_simplify.h"
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_STARTSTOP_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#undef JVX_CONNECTION_MASTER_SKIP_TRANSFER_BACKWARD_OCON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_OCON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_PREPARE_POSTPROCESS_TO

#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_LINKDATA_SLAVE

};

#endif
