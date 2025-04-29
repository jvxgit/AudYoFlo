#ifndef __CAYFSTARTER_H__
#define __CAYFSTARTER_H__

#include "jvx.h"

#ifdef AYF_INVOLVE_CHANNEL_REARRANGE
#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#else
#include "jvxNodes/CjvxBareNode1io.h"
#endif

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCallsProfileTpl.h"
#define AYF_TEMPLATE_PARENT_CLASS CjvxMexCallsProfileTpl
#else
#include "CjvxMexCallsProfileNoMexTpl.h"
#define AYF_TEMPLATE_PARENT_CLASS CjvxMexCallsProfileNoMexTpl
#endif

#ifdef AYF_INVOLVE_CHANNEL_REARRANGE
#define AYF_AUDIO_NODE_NODE_CLASS CjvxBareNode1ioRearrange
#else
#define AYF_AUDIO_NODE_NODE_CLASS CjvxBareNode1io
#endif
#define AYF_AUDIO_NODE_BASE_CLASS AYF_TEMPLATE_PARENT_CLASS<AYF_AUDIO_NODE_NODE_CLASS>

extern "C"
{
#ifdef JVX_USE_RUST_CORE_LIB
#include "ayfstarterlibrs.h"
#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "ayfstarterlibdbgrs.h"
#endif
#else
#include "ayfstarterlib.h"
#endif
}

// Different pcg files for regular and matlab version of audio node
#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "pcg_exports_nodem.h"
#else
#include "pcg_exports_node.h"
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CayfAuNStarter: public AYF_AUDIO_NODE_BASE_CLASS, public genStarter_node
{	
protected:
	struct ayf_starter processing_lib;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	struct ayf_starter_data_debug* processing_lib_dbg = nullptr;
#endif

public:
	JVX_CALLINGCONVENTION CayfAuNStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfAuNStarter();	

	// Step II: Add activate and deactivate function
	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// Step II: Add prepare, postprocess and process function
	// STep IV: rename the main processing functions
	jvxErrorType local_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	jvxErrorType local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// Step III: Define a property-set callback
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_async_set);


#ifdef JVX_EXTERNAL_CALL_ENABLED

#include "mcg_exports_project_prototypes.h"	
	void initExternalCall() override;
	void terminateExternalCall() override;
	
	jvxErrorType local_allocate_profiling() override;
	jvxErrorType local_deallocate_profiling() override;

#include "pcg_exports_nodem__assoc__h.h"
#else
#include "pcg_exports_node__assoc__h.h"
#endif
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
