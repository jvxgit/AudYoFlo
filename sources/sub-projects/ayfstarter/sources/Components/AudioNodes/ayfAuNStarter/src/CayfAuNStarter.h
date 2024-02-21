#ifndef __CAYFSTARTER_H__
#define __CAYFSTARTER_H__

#include "jvx.h"

#ifdef AYF_INVOLVE_CHANNEL_REARRANGE
#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#define AYF_AUDIO_NODE_BASE_CLASS CjvxBareNode1ioRearrange
#else
#include "jvxNodes/CjvxBareNode1io.h"
#define AYF_AUDIO_NODE_BASE_CLASS CjvxBareNode1io
#endif

extern "C"
{
#include "ayfstarterlib.h"
}

#include "pcg_exports_node.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CayfAuNStarter: public AYF_AUDIO_NODE_BASE_CLASS, public genStarter_node
{	
protected:
	struct ayf_starter processing_lib;

public:
	JVX_CALLINGCONVENTION CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfAuNStarter();	

	// Step II: Add activate and deactivate function
	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// Step II: Add prepare, postprocess and process function
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// Step III: Define a property-set callback
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_async_set);

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
