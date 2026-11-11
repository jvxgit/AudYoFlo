#ifndef __CJVXSPNMULTIPLEXER_H__
#define __CJVXSPNMULTIPLEXER_H__

/*
 * This module forwards one input stream with M channels to N output streams, each carrying a subset
 * (a bundle) of the input channels. The counterpart to recombine the streams is <CjvxSpNDemultiplexer>.
 *
 *                               /--> out#0 [channels 0 .. W-1]
 *   M channels --> "default" --X---> out#1 [channels W .. 2W-1]
 *                               \--> out#N-1 [channels (N-1)W .. NW-1]
 *
 * The input connector is the connector "default" of the base class. On the output side, the connector
 * "default" of the base class is not exposed. Instead, N connectors out#0 .. out#N-1 are available.
 * The number N is specified by property </config/number_output_connectors> in state ACTIVE. All connectors
 * are part of the SAME connection (process) as the input connector: all connection calls arriving at the
 * input connector are forwarded to all output connectors which were connected when the chain was connected.
 * Output connectors may remain unconnected. Once the chain has been connected, unconnected connectors can not
 * be connected until the chain was disconnected.
 *
 * The width W of the channel bundles is specified by property </config/channels_per_connector>. A value
 * of 0 yields W = M / N. Example: 8 input channels and 4 output connectors yield 4 streams with 2 channels each.
 *
 * Buffer handling: on prepare, the successors of the output connectors allocate their buffers first. If possible,
 * these buffers are then linked directly into the buffer field on the input side. In this case, the
 * predecessor writes directly into the buffers of the successors - no copy is required. During processing,
 * the module checks for each channel if source and target buffer are identical and copies only if that is not the case.
 */

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "CjvxSpNMultiplexer_connector.h"

#include "pcg_exports_node.h"

#define JVX_SPN_MULTIPLEXER_MAX_CONNECTORS 128

class CjvxSpNMultiplexer : public CjvxBareNode1ioRearrange,
	public CjvxSingleConnector_report<CjvxSingleOutputConnector>,
	public CjvxSingleConnectorTransfer_report<CjvxSingleOutputConnector>,
	public genMultiplexer_node
{
protected:

	//! Output connectors out#0 .. out#N-1, position in list is the connector index
	std::vector<CjvxSpNMultiplexerOutputConnector*> outputConnectors;

	//! Indicates that the input parameters were modified due to a request of one of the successors during test
	jvxBool renegotiatedOnTest = false;

	struct
	{
		//! Size of one channel buffer in bytes
		jvxSize szChannelBytes = 0;
	} runtime;

public:

	JVX_CALLINGCONVENTION CjvxSpNMultiplexer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNMultiplexer();

	// ===================================================================================
	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// ===================================================================================
	// Connect / disconnect
	// ===================================================================================
	jvxErrorType connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	// Negotiations
	// ===================================================================================
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void from_input_to_output() override;
	jvxErrorType transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ===================================================================================
	// Processing
	// ===================================================================================
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType process_start_icon(
		jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr) override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;
	jvxErrorType process_stop_icon(
		jvxSize idx_stage,
		jvxBool operate_first_call,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr) override;

	// ===================================================================================
	// Chain iterator: there is one "next" element for each connected output connector
	// ===================================================================================
	jvxErrorType number_next(jvxSize* num) override;
	jvxErrorType reference_next_handle(jvxSize idx, IjvxConnectionIterator** next, jvxApiString* nmOcon = nullptr, jvxApiString* nmIcon = nullptr) override;

	// ===================================================================================
	// Callbacks output connectors
	// ===================================================================================
	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* oconn) override;
	jvxErrorType report_test_connector(CjvxSingleOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void request_unique_id_start(CjvxSingleOutputConnector* oconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleOutputConnector* oconn) override;
	jvxErrorType report_stopped_connector(CjvxSingleOutputConnector* oconn) override;
	void release_unique_id_stop(CjvxSingleOutputConnector* oconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleOutputConnector* oconn) override;
	void report_process_buffers(CjvxSingleOutputConnector* oconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	//! All transfers arriving at one of the output connectors from the successor end up here
	jvxErrorType report_transfer_connector(CjvxSingleOutputConnector* oconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

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

	//! Entry for the output connectors: check if a connector can currently be involved in a connection
	jvxErrorType check_output_connector_available(CjvxSpNMultiplexerOutputConnector* oconn, IjvxDataConnectionCommon* ass_connection_common);

protected:

	//! Add or remove output connectors at the end of the list
	jvxErrorType update_output_connectors(jvxSize numConnectorsNew);

	//! Assign the channel bundles to the output connectors
	void update_channel_assignment();

	//! Derive the output parameters of one connector from the current input parameters
	void update_output_parameters(CjvxSpNMultiplexerOutputConnector* oconn);

	jvxErrorType test_one_output_connector(CjvxSpNMultiplexerOutputConnector* oconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType negotiate_one_output_connector(CjvxSpNMultiplexerOutputConnector* oconn, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	void link_buffers_output_connector(CjvxSpNMultiplexerOutputConnector* oconn, jvxBool linkAllowed);
	void unlink_buffers_output_connector(CjvxSpNMultiplexerOutputConnector* oconn);

	void update_monitor();
};

#endif
