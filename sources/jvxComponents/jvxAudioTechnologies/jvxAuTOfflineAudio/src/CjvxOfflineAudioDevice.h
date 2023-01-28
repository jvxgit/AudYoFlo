#ifndef __CJVXOFFLINEAUDIODEVICE_H__
#define __CJVXOFFLINEAUDIODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"

#include "pcg_exports_device.h"

#ifdef JVX_EXTERNAL_CALL_ENABLED
#include "CjvxMexCalls.h"
#endif

class CjvxOfflineAudioDevice: public CjvxAudioDevice
#ifdef JVX_EXTERNAL_CALL_ENABLED
	, public CjvxMexCallsMin
#endif
	, public IjvxExternalCallTarget
	, public genOfflineAudio_device
{
private:
public:

	JVX_CALLINGCONVENTION CjvxOfflineAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxOfflineAudioDevice();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	// Interface IjvxConfiguration

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override;

	jvxErrorType set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	void alignChannelsExposed();

	virtual jvxErrorType JVX_CALLINGCONVENTION extcall_lasterror(jvxApiError* theErr)override{return(this->lasterror(theErr));};

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(
		jvxSize pipeline_offset, 
		jvxSize* idx_stageL,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(
		jvxSize mt_mask, 
		jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(
		jvxSize idx_stage, 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)override;

	// ===================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	// ===================================================================================

	jvxErrorType transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#include "mcg_exports_matlab_prototypes.h"
};

#endif
