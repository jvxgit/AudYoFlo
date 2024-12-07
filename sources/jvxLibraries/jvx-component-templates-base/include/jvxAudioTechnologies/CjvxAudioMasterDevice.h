#ifndef __CJVXAUDIOMASTERDEVICE_H__
#define __CJVXAUDIOMASTERDEVICE_H__

#include "common/CjvxSimpleMasterDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "pcg_CjvxAudioDevice_pcg.h"
// #include "pcg_CjvxDeviceCaps_pcg.h"

class CjvxAudioMasterDevice : public CjvxSimpleMasterDevice,
	public IjvxProperties, public CjvxProperties,
	public IjvxConfiguration,
	public CjvxAudioDevice_genpcg
{
protected:

	struct
	{
		std::vector<jvxDataFormat> formats;
	} _common_set_audio_device;

	jvxAudioParams _inproc;

	struct
	{
		jvxSize last_selection_user_input_channels;
		jvxSize last_selection_user_output_channels;
	} last_user_interaction;

public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxAudioMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAudioMasterDevice();

	// =====================================================================
	// Default implementations state machine
	// =====================================================================
#define  JVX_STATE_MACHINE_DEFINE_STARTSTOP
#define  JVX_STATE_MACHINE_DEFINE_READY
#define JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATEMACHINE_FULL_SKIP_SELECT_UNSELECT
#undef JVX_STATE_MACHINE_DEFINE_READY
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	// =====================================================================
	// =====================================================================

	// =======================================================================
	// Interface properties
	// =======================================================================
#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_STOP_GROUP
#include "codeFragments/simplify/jvxProperties_simplify.h"
#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_STOP_GROUP
#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET


	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_group(jvxCallManagerProperties& callGate)override;

	// =====================================================================
	// =====================================================================


	// =======================================================================
	// Interface IjvxConfiguration
	// =======================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "",
		jvxInt32 lineno = -1)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;
	// =====================================================================
	// =====================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;


	// =====================================================================
	// Activate the relevant hidden interfaces
	// =====================================================================
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxSimpleMasterDevice
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

	// =======================================================================
	// =======================================================================

	// =====================================================================
	// For all non-object interfaces, return object reference
	// =====================================================================
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
	// =====================================================================
	// =====================================================================
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"
	// =======================================================================
	// =======================================================================
	void updateDependentVariables(jvxSize propId,
		jvxPropertyCategoryType category,
		jvxBool updateAll,
		jvxPropertyCallPurpose callPurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC,
		jvxBool suppress_update_chain = false);

#if 0
	virtual jvxErrorType activate_lock();
	virtual jvxErrorType deactivate_lock();
#endif

	jvxErrorType currentSetupAudioParams(jvxAudioParams& params);
	virtual void updateChainOutputParameter();

	// Special function to react on complain function during test: it should find a good match 
	// for rate and size. Only to be used in case tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS
	jvxErrorType transfer_backward_ocon_match_setting(jvxLinkDataTransferType tp, jvxHandle* data, jvxPropertyContainerSingle<jvxSelectionList_cpp>* rateselection,
		jvxPropertyContainerSingle<jvxSelectionList_cpp>* sizeselection JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual void activate_connectors_master();
	virtual void deactivate_connectors_master();

};

#endif
