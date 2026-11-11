#ifndef __CJVXSPNDEMULTIPLEXER_H__
#define __CJVXSPNDEMULTIPLEXER_H__

/*
 * This module is the counterpart of <CjvxSpNMultiplexer>: it combines the channels of N input streams
 * to a single output stream.
 *
 *   in#0   [n0 channels] --\
 *   in#1   [n1 channels] ---X--> "default" [n0 + n1 + .. channels]
 *   in#N-1 [..         ] --/
 *
 * The output connector is the connector "default" of the base class. On the input side, the connector
 * "default" of the base class is not exposed. Instead, N connectors in#0 .. in#N-1 are available.
 * The number N is specified by property </config/number_input_connectors> in state ACTIVE. All connectors
 * must be part of the SAME connection (process) as the output connector. Input connectors may remain unconnected.
 * The output channels are composed of the channels of all connected input connectors in the order of the
 * connector index. All input streams must be operated with identical buffersize, samplerate and format.
 *
 * Chaining: in a chain with a multiplexer, each call (connect, test, prepare, ..) arrives once for each connected
 * input connector. The chain towards the output is continued exactly once:
 * - connect, prepare, start and process_start: on the arrival at the first connector,
 * - test, process_buffers and process_stop: when all connectors have been involved,
 * - stop, postprocess and disconnect: when the last connector leaves the corresponding state.
 *
 * Buffer handling: on prepare, the successor on the output side allocates its buffers first. If possible,
 * these buffers are then linked directly into the buffer fields of the input connectors. In this case, the
 * predecessors write directly into the buffers of the successor - no copy is required. During processing,
 * the module checks for each channel if source and target buffer are identical and copies only if that is not the case.
 */

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "CjvxSpNDemultiplexer_connector.h"

#include "pcg_exports_node.h"

#define JVX_SPN_DEMULTIPLEXER_MAX_CONNECTORS 128

class CjvxSpNDemultiplexer : public CjvxBareNode1ioRearrange,
	public CjvxSingleConnector_report<CjvxSingleInputConnector>,
	public CjvxSingleConnectorTransfer_report<CjvxSingleInputConnector>,
	public genDemultiplexer_node
{
protected:

	//! Input connectors in#0 .. in#N-1, position in list is the connector index
	std::vector<CjvxSpNDemultiplexerInputConnector*> inputConnectors;

	//! Processing parameters which are common for all input connectors, taken from the first connector in a test run
	jvxLinkDataDescriptor_con_params paramsReference;

	//! Status of the chain on the output side
	struct
	{
		jvxBool isConnected = false;
		jvxBool isPrepared = false;
		jvxBool isStarted = false;

		//! process_start was forwarded and requires a process_stop
		jvxBool inCycle = false;

		//! process_buffers was forwarded in the current cycle
		jvxBool buffersForwarded = false;
	} output;

	struct
	{
		//! Size of one channel buffer in bytes
		jvxSize szChannelBytes = 0;
	} runtime;

public:

	JVX_CALLINGCONVENTION CjvxSpNDemultiplexer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNDemultiplexer();

	// ===================================================================================
	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// ===================================================================================
	// Output connector "default": requests from the successor
	// ===================================================================================
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	// Callbacks input connectors
	// ===================================================================================
	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleInputConnector* iconn) override;
	jvxErrorType report_stopped_connector(CjvxSingleInputConnector* iconn) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn) override;
	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	//! All forward transfers arriving at one of the input connectors end up here
	jvxErrorType report_transfer_connector(CjvxSingleInputConnector* iconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	// Configuration
	// ===================================================================================
	jvxErrorType put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno) override;

	jvxErrorType get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);

	// ===================================================================================
	// Entries for the input connectors
	// ===================================================================================
	jvxErrorType check_input_connector_available(CjvxSpNDemultiplexerInputConnector* iconn, IjvxDataConnectionCommon* ass_connection_common);

	jvxErrorType icon_connect(CjvxSpNDemultiplexerInputConnector* iconn, jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_disconnect(CjvxSpNDemultiplexerInputConnector* iconn, jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_test(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_prepare(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_postprocess(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_start(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType icon_stop(CjvxSpNDemultiplexerInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	jvxErrorType icon_process_start(CjvxSpNDemultiplexerInputConnector* iconn,
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr);
	jvxErrorType icon_process_buffers(CjvxSpNDemultiplexerInputConnector* iconn, jvxSize mt_mask, jvxSize idx_stage);
	jvxErrorType icon_process_stop(CjvxSpNDemultiplexerInputConnector* iconn,
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr);

	jvxErrorType icon_reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon, jvxApiString* nmIcon);

protected:

	//! Add or remove input connectors at the end of the list
	jvxErrorType update_input_connectors(jvxSize numConnectorsNew);

	//! Derive the output parameters from the parameters of all connected input connectors
	void update_output_parameters();

	//! Run the test on the output side once all input connectors have been tested
	jvxErrorType test_output(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	//! The successor requests other processing parameters
	jvxErrorType negotiate_output(jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	void link_buffers_input_connector(CjvxSpNDemultiplexerInputConnector* iconn, jvxBool linkAllowed);
	void unlink_buffers_input_connector(CjvxSpNDemultiplexerInputConnector* iconn);

	//! Close a processing cycle on the output side
	void finalize_output_cycle();

	void update_monitor();
};

#endif
