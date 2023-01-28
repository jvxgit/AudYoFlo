#ifndef __CjvxAudioNodeViewer_H__
#define __CjvxAudioNodeViewer_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"
#include "jvx_resampler/jvx_resampler_plotter.h"

class CjvxAudioNodeViewer: public CjvxAudioNode, public genViewer_node
{

private:
	struct
	{
		jvxExternalBuffer* circbuffer_input;
		jvxExternalBuffer* circbuffer_output;
	} external;


	struct
	{
		struct
		{
			jvxData deltaT;
			jvxData t;
			jvxSize resamplingFactor_id;
			jvxData resamplingFactor_time;
			jvxData resamplingFactor_value;
			resampler_plotter_private** resamplers;
			jvxSize numResamplers;
		} input;

		struct
		{
			jvxSize resamplingFactor_id;
			jvxData resamplingFactor_value;
			resampler_plotter_private** resamplers;
			jvxSize numResamplers;
		} output;
	} runtime;
public:

	JVX_CALLINGCONVENTION CjvxAudioNodeViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioNodeViewer();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffer( jvxHandle** buffers_input,jvxHandle** buffers_output,
		jvxSize bSize, jvxSize numChans_input, jvxSize numChans_output, jvxDataFormat format);

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_buffer_post);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_buffer_pre);

	jvxErrorType prefill_buffers_input();

};

#endif
