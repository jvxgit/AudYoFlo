#ifndef __CJVX_AUN_BINAURAL_RENDER__
#define __CJVX_AUN_BINAURAL_RENDER__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "jvxText2Speech/CjvxTxt2Speech_mix.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvxFastConvolution/CjvxFastConvolution.h"
#include "pcg_exports_node.h"

#include "jvx_fft_tools/jvx_firfft_cf.h"

#define AYF_CTC_EFFICIENT_FILTER

enum class jvxRenderingUpdateStatus
{
	JVX_RENDERING_UPDATE_OFF,
	JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK,
	JVX_RENDERING_UPDATE_IN_OPERATION,
	JVX_RENDERING_UPDATE_READY
};

class jvxOneRenderCore
{
public:

	jvxData* buffer_hrir_left = nullptr;
	jvxData* buffer_hrir_right = nullptr;
	jvxSize length_buffer_hrir = JVX_SIZE_UNSELECTED;
	jvxSize idx_sofa = 0;

#ifdef AYF_CTC_EFFICIENT_FILTER
	jvx_firfft firfftcf_left_right;
	jvxHandle* firfftcf_cvrt;
#else
	jvx_firfft firfftcf_left;
	jvx_firfft firfftcf_right;
#endif

	// These fields are required to feed in new coefficients on filter changes
	jvxDataCplx* updatedWeightsLeft = nullptr;
	jvxDataCplx* updatedWeightsRight = nullptr;
	jvxSize lUpdatedWeights = 0;

	jvxSize loadId = JVX_SIZE_UNSELECTED;
};

class CjvxAuNBinauralRender : public CjvxBareNode1ioRearrange, 
	public IjvxPropertyExtender, public IjvxPropertyExtenderSpatialDirectionDirect,
	public IjvxPropertyExtenderHrtfDispenser_report, public IjvxThreads_report,
	public genBinauralRender_node
{
private:

	// This pointer reference should be non-null on state ACTIVE and higher - it is set in state SELECTED
	IjvxPropertyExtenderHrtfDispenser* theHrtfDispenser = nullptr;

	// Property containing azimuth (first array element) and inclination angle (second element) of rendered source.
	jvxData source_direction_angles_deg_set[2] = { 0.0, 90.0 };

	// State array containing azimuth (first array element) and inclination angle (second element) of rendered source.
	jvxData source_direction_angles_deg_inuse[2] = { 0.0, 90.0 };
	
	void allocate_hrir_buffers(jvxOneRenderCore* renderer);
	void deallocate_hrir_buffers(jvxOneRenderCore* renderer);	
	void update_hrirs(jvxOneRenderCore* renderer, jvxData azimuth_deg, jvxData inclination_deg);

	// =================================================================================
	jvxOneRenderCore* render_pri = nullptr;
	jvxOneRenderCore* render_sec = nullptr;

	refComp<IjvxThreads> threads;
	JVX_MUTEX_HANDLE safeAccessUpdateBgrd;
	
	jvxRenderingUpdateStatus updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF;
	jvxData newAzimuth = 0;
	jvxData newInclination = 0;

	jvxRenderingUpdateStatus updateDBase = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF;

	jvxSize missedUpdatesPosition = 0;	

public:

	JVX_CALLINGCONVENTION CjvxAuNBinauralRender(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNBinauralRender();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	// void from_input_to_output() override;

	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;


	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_extend_ifx_reference);

	// Callback is triggered to indicate updated direction in property input_source_direction_angles_deg.
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_source_direction);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(update_active_slot);

	// =========================================================================================================
	// IjvxPropertyExtenderHrtfDispenser_report
	virtual jvxErrorType report_database_changed(jvxSize slotId) override;
	// =========================================================================================================
	jvxErrorType supports_prop_extender_type(jvxPropertyExtenderType tp) override;
	jvxErrorType prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp)override;

	jvxErrorType set_spatial_position(jvxData valueAzimuth, jvxData valueInclination) override;

	jvxOneRenderCore* allocate_renderer(jvxSize bsize, jvxData startAz, jvxData startInc);
	void deallocate_renderer(jvxOneRenderCore*& render_inst);
	void try_trigger_update_position(jvxData azimuth, jvxData inclination);

	jvxErrorType startup(jvxInt64 timestamp_us) override;
	jvxErrorType expired(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	jvxErrorType wokeup(jvxInt64 timestamp_us, jvxSize* delta_ms)override;
	jvxErrorType stopped(jvxInt64 timestamp_us) override;


};
#endif

