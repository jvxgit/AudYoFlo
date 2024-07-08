
#include "jvxNodes/CjvxBareNode1io.h"
#include "pcg_exports_node.h"

class CjvxSpNLevelControl : public CjvxBareNode1io, public genLevelMeter_node
{

private:
	/*
	jvxSize numChannels = 0;
	jvxData* ptrLevel = nullptr;
	jvxCBool* ptrMute = nullptr;
	jvxData* ptrGain = nullptr;
	*/
	class mixer_flds
	{
	public:
		jvxData* fldAvrg = nullptr;
		jvxData* fldMax = nullptr;
		jvxData* fldGain = nullptr;
		jvxSize* fldCntMax = nullptr;
		jvxBitField* fldMute = nullptr;
		jvxCBool* fldClip = nullptr;
		jvxSize lenField = 0;

		// This association is always true in case of the main thread:
		// The fields are reallocated only if a channel is added which 
		// always happens in the main thread. However, within the main 
		// processing thread as well as in any other thread, the association 
		// may be temporarily unavailable. Since the adding and removal of the 
		// channels is protected by the lock _common_set_nv_proc.safeAcces_proc_tasks,
		// also this variable should be checked and evaluated with the same lock!!
		jvxBool associationValid = false;
	};

	mixer_flds mixer;

public:

	JVX_CALLINGCONVENTION CjvxSpNLevelControl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNLevelControl();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_level_get);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_level_set);
};