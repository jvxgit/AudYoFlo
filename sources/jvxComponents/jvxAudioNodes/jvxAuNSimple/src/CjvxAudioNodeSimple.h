#ifndef __CJVXAUDIONODESIMPLE_H__
#define __CJVXAUDIONODESIMPLE_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"

class CjvxAudioNodeSimple: public CjvxAudioNode, public genSimple_node
{
	
public:

	JVX_CALLINGCONVENTION CjvxAudioNodeSimple(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioNodeSimple();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender);

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffer( jvxHandle** buffers_input,jvxHandle** buffers_output,
		jvxSize bSize, jvxSize numChans_input, jvxSize numChans_output, jvxDataFormat format);

};

#endif
