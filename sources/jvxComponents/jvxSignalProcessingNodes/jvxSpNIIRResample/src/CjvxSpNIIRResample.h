
#define JVX_NODE_TYPE_SPECIFIER_TYPE JVX_COMPONENT_SIGNAL_PROCESSING_NODE
#define JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR "signal_processing_node"

#include "jvxNodes/CjvxBareNode1ioRearrange.h"

#include "common/CjvxInputOutputSettings.h"
#include "CjvxDigitalFilter.h"

// Selectable IIR filter designs used for the anti-aliasing / anti-imaging lowpass filter.
// These map directly onto the technology tokens understood by CjvxDigitalFilter::initialize().
enum class jvxIIRResamplerFilterTechnology
{
	JVX_IIR_RESAMPLER_FILTER_BUTTERWORTH,
	JVX_IIR_RESAMPLER_FILTER_CHEBYSHEV1,
	JVX_IIR_RESAMPLER_FILTER_CHEBYSHEV2,
	JVX_IIR_RESAMPLER_FILTER_ELLIPTIC
};

#include "pcg_exports_node.h"

/**
 * Signal processing node implementing a low-latency sample rate converter based on a
 * cascaded IIR (biquad) lowpass filter instead of a linear-phase polyphase FIR structure.
 *
 * Scope: the node only supports the simple case of a pure integer up- or downsampling
 * factor between the negotiated input and output samplerate (e.g. 48 kHz <-> 16 kHz), and
 * requires that the negotiated buffersizes always relate by exactly that integer factor
 * (no variable-framesize rebuffering for fractional remainders). A rational conversion
 * that requires both interpolation and decimation (e.g. 44.1 kHz <-> 48 kHz) is out of
 * scope and rejected during connect.
 */
class CjvxSpNIIRResample : public CjvxBareNode1ioRearrange,
	public genIIRResample_node
{
private:

	struct
	{
		jvxBool active_resampling = false;
		jvxBool mode_decimate = false;
		jvxBool mode_interpolate = false;
		jvxSize numChannels = 0;
		jvxSize lenIntermediate = 0;
		jvxData** bufIntermediate = nullptr;
		CjvxDigitalFilter filterObj;
	} runtime;

	jvxIIRResamplerFilterTechnology filterTechnology = jvxIIRResamplerFilterTechnology::JVX_IIR_RESAMPLER_FILTER_BUTTERWORTH;

	//! Handles the generic rate/buffersize/format/channel negotiation with predecessor and successor
	CjvxInputOutputSettings currNegoStat;

	jvxErrorType setup_filter();

public:

	JVX_CALLINGCONVENTION CjvxSpNIIRResample(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNIIRResample();

	// ===================================================================================
	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	// ===================================================================================
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	jvxErrorType accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void from_input_to_output() override;
	jvxErrorType accept_input_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	void test_set_output_parameters() override;

	// ===================================================================================
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_config);

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

};
