#ifndef __CJVXSPNDEMULTIPLEXER_CONNECTOR_H__
#define __CJVXSPNDEMULTIPLEXER_CONNECTOR_H__

#include "common/CjvxSingleInputConnector.h"

class CjvxSpNDemultiplexer;

/*
 * One of the input connectors in#0 .. in#N-1 of the demultiplexer. In contrast to the
 * base class, this connector is not the end of a secondary chain: all calls are passed to
 * the demultiplexer which continues the chain on its output connector "default".
 */
class CjvxSpNDemultiplexerInputConnector : public CjvxSingleInputConnector
{
public:
	CjvxSpNDemultiplexer* parent = nullptr;

	//! Position of connector, yields name in#<idx>
	jvxSize idx = 0;

	//! Position of the first channel of this connector within the output channels
	jvxSize chanOffset = 0;

	//! Connector was connected when the chain was connected
	jvxBool inChain = false;

	jvxBool isPrepared = false;
	jvxBool isStarted = false;

	//! Buffers on the output side were linked into the input buffer field of this connector on prepare
	jvxBool isLinked = false;

	//! Flag to detect that all connected connectors were involved in a test run
	jvxBool seenTest = false;

	//! Flags to detect that all connectors in operation were involved in one processing cycle
	jvxBool seenProcessStart = false;
	jvxBool seenProcessBuffers = false;
	jvxBool seenProcessStop = false;

public:
	CjvxSpNDemultiplexerInputConnector(CjvxSpNDemultiplexer* parentArg, jvxSize idxArg);

	jvxErrorType available_to_connect_icon() override;
	jvxErrorType select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector) override;

	jvxErrorType connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType process_start_icon(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk = NULL,
		jvxHandle* priv_ptr = NULL) override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	jvxErrorType process_stop_icon(jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;

	// The chain is continued on the output connector "default" of the demultiplexer
	jvxErrorType number_next(jvxSize* num) override;
	jvxErrorType reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon, jvxApiString* nmIcon) override;
};

#endif
