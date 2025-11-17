#ifndef __CAYFMIXER_H__
#define __CAYFMIXER_H__

#include "jvx.h"

#define AYF_INVOLVE_CHANNEL_REARRANGE

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
#include "ayfmixerlib.h"
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

class CayfAuNMixer: public AYF_AUDIO_NODE_BASE_CLASS, public genMixer_node
{	
protected:
	struct ayf_mixer processing_lib;

#ifdef JVX_EXTERNAL_CALL_ENABLED
	struct ayf_mixer_data_debug* processing_lib_dbg = nullptr;
#endif

	class oneEntryChannel
	{
	public:
		jvxSize chanNr = JVX_SIZE_UNSELECTED;
		jvxData level = 1.0;
	};
	 
	jvxLockWithVariable< std::map<int, std::list<oneEntryChannel> > > safeMixer;

public:
	JVX_CALLINGCONVENTION CayfAuNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CayfAuNMixer();

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// Step II: Add prepare, postprocess and process function
	// STep IV: rename the main processing functions
	jvxErrorType local_process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	jvxErrorType local_prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType local_postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;


	// Step III: Define a property-set callback
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_async_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_mixing);

	std::map<int, std::list<oneEntryChannel>> updateMixMatrix_inLock(const std::string& token);
	void updateMixMatrixProperty();

#ifdef JVX_EXTERNAL_CALL_ENABLED

#include "mcg_exports_project_prototypes.h"	
	void initExternalCall() override;
	void terminateExternalCall() override;
	
	jvxErrorType local_allocate_profiling() override;
	jvxErrorType local_deallocate_profiling() override;
#endif

	// JVX_LINKDATA_TRANSFER_REQUEST_GET_PROPERTIES
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
