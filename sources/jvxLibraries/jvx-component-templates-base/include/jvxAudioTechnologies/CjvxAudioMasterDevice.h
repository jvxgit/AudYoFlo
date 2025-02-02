#ifndef __CJVXAUDIOMASTERDEVICE_H__
#define __CJVXAUDIOMASTERDEVICE_H__

#include "common/CjvxSimpleMasterDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"

#include "pcg_CjvxAudioMasterDevice_pcg.h"
#include "common/CjvxSimplePropsParsPlusOutChannel.h"

// #include "pcg_CjvxDeviceCaps_pcg.h"

class CjvxAudioMasterDevice : public CjvxSimpleMasterDevice,
	public IjvxProperties, public CjvxProperties,
	public IjvxConfiguration,
	public CjvxAudioMasterDevice_genpcg
{
protected:

	CjvxSimplePropsParsPlusOutChannel inout_params;
	
	CjvxNegotiate_input neg_input;
	CjvxNegotiate_output neg_output;
	
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
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_READY
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP

	//! Involve specific technology checks and parameters
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	//! Prepre and postprocess to lock the properties on start!!
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	// =====================================================================
	// =====================================================================

	// =======================================================================
	// Interface properties
	// =======================================================================
//#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET
//#define JVX_PROPERTY_SIMPLIFY_OVERWRITE_STOP_GROUP
#include "codeFragments/simplify/jvxProperties_simplify.h"
//#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_STOP_GROUP
//#undef JVX_PROPERTY_SIMPLIFY_OVERWRITE_SET

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_property_group(jvxCallManagerProperties& callGate)override;
	*/

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

	virtual jvxErrorType JVX_CALLINGCONVENTION try_match_settings_backward_ocon(jvxLinkDataDescriptor* ld_con JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

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
	void updateDependentVariables(jvxBool suppress_update_chain = false);

#if 0
	virtual jvxErrorType activate_lock();
	virtual jvxErrorType deactivate_lock();
#endif

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(base_params_set_update);

	virtual void activate_connectors_master();
	virtual void deactivate_connectors_master();

};

#endif
