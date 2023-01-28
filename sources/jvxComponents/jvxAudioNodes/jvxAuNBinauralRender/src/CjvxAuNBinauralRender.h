#ifndef __CJVX_AUN_BINAURAL_RENDER__
#define __CJVX_AUN_BINAURAL_RENDER__

#include "jvxNodes/CjvxBareNode1io_rearrange.h"
#include "jvxText2Speech/CjvxTxt2Speech_mix.h"
#include "jvx_circbuffer/jvx_circbuffer.h"
#include "jvxFastConvolution/CjvxFastConvolution.h"
#include "pcg_exports_node.h"

class CjvxAuNBinauralRender : public CjvxBareNode1io_rearrange, public IjvxPropertyExtenderHrtfDispenser_report,
	public genBinauralRender_node
{
private:
	
	typedef std::vector<CjvxFastConvolution> ConvolutionsLR;
	typedef std::vector<ConvolutionsLR> ConvolutionsHrirCurrentNext;
	typedef std::vector<ConvolutionsHrirCurrentNext> ConvolutionsSOFAdb;

	ConvolutionsSOFAdb convolutions;
	jvxSize idx_conv_hrir_current = 0;
	jvxSize idx_conv_sofa_current = 0;

	JVX_MUTEX_HANDLE mutex_convolutions;


	// Indicate if convolution switch happened.
	bool conv_hrir_dirty = false;
	bool sofa_db_dirty = false;

	// This pointer reference should be non-null on state ACTIVE and higher - it is set in state SELECTED
	IjvxPropertyExtenderHrtfDispenser* theHrtfDispenser = nullptr;

	// Property containing azimuth (first array element) and inclination angle (second element) of rendered source.
	jvxData input_source_direction_angles_deg[2] = { 0.0, 90.0 };

	// State array containing azimuth (first array element) and inclination angle (second element) of rendered source.
	jvxData source_direction_angles_deg[2] = { 0.0, 90.0 };

	jvxData* buffer_hrir_left = nullptr;
	jvxData* buffer_hrir_right = nullptr;

	jvxSize length_buffer_hrir = JVX_SIZE_UNSELECTED;

	// Helper buffer for mixing of output signals during convolution switch and interpolation.
	jvxData* buffer_out_temp = nullptr;

	/**
	 * Helper function for linear interpolation of signal output filtered with old HRIR and new HRIR.
	 * 
	 * \param inout Array on which in-place weighting will be done. Needs a length greater of equal num_weights.
	 * \param num_weights Number of weighting operations, which are performed.
	 * \param weight_start The first element of the array will be weighted with weight_start + weight_delta.
	 * \param weight_delta Amount by which the applied weight is increased for each weighted array element.
	 */
	void linear_weighting(jvxData* inout, jvxSize num_weights, jvxData weight_start, jvxData weight_delta);
	
	void allocate_hrir_buffers(jvxSize length_ir);
	void delete_hrir_buffers();

	void init_convolution_set(ConvolutionsHrirCurrentNext& convolutions, jvxSize length_ir, jvxSize frame_advance);

	
	void update_hrirs(jvxSize idx_conv_sofa, jvxData azimuth_deg, jvxData inclination_deg);

	jvxSize toggle_idx(jvxSize idx);

	jvxSize frame_advance = JVX_SIZE_UNSELECTED;

public:

	JVX_CALLINGCONVENTION CjvxAuNBinauralRender(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxAuNBinauralRender();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* owner)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

	// =========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION activate();
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate();

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

	// =========================================================================================================
	// IjvxPropertyExtenderHrtfDispenser_report
	virtual jvxErrorType report_database_changed() override;
};
#endif

