#include "CjvxAuNBinauralRender.h"
#include "jvx-helpers-cpp.h"
#include "jvx_fft_tools/jvx_firfft.h"

CjvxAuNBinauralRender::CjvxAuNBinauralRender(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	// Following already set in componentEntry
	//_common_set.theComponentType.tp = JVX_NODE_TYPE_SPECIFIER_TYPE;
	//_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;

	// Set the processing parameters that will be accepted in the input connector.
	// Note: if no constraint is set the input and output format entries must be 
	// identical.

	neg_input._set_parameters_fixed(
		1 /* 1 input channel required*/,
		JVX_SIZE_UNSELECTED, /* buffersize is not constrained */
		JVX_SIZE_UNSELECTED, /* samplerate is not constrained */
		JVX_DATAFORMAT_DATA, /* data format */
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED); /* data format group */

	neg_output._set_parameters_fixed(
		2 /* 2 output channel required*/,
		JVX_SIZE_UNSELECTED, /* buffersize is not constrained */
		JVX_SIZE_UNSELECTED, /* samplerate is not constrained */
		JVX_DATAFORMAT_DATA, /* data format */
		JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED); /* data format group */

	//outputArgsFromOutputParams = true;
}

CjvxAuNBinauralRender::~CjvxAuNBinauralRender()
{
}

// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1ioRearrange::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::init__global__if_ext();
		genBinauralRender_node::allocate__global__if_ext();
		genBinauralRender_node::register__global__if_ext(static_cast<CjvxProperties*>(this));

		genBinauralRender_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_extend_ifx_reference, update_source_direction,
			reinterpret_cast<jvxHandle*>(this), NULL);
	}
	return res;
}

jvxErrorType
CjvxAuNBinauralRender::unselect()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);
		genBinauralRender_node::unregister_all(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::deallocate_all();

		res = CjvxBareNode1ioRearrange::unselect();
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::init__local__source_direction();
		genBinauralRender_node::allocate__local__source_direction();
		genBinauralRender_node::register__local__source_direction(static_cast<CjvxProperties*>(this));

		// register property
		genBinauralRender_node::associate__local__source_direction(
			static_cast<CjvxProperties*>(this),
			input_source_direction_angles_deg, 2);
	}
	return res;
}

jvxErrorType
CjvxAuNBinauralRender::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genBinauralRender_node::deassociate__local__source_direction(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::unregister__local__source_direction(static_cast<CjvxProperties*>(this));
		genBinauralRender_node::deallocate__local__source_direction();

		res = CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

/*
void
CjvxAuNBinauralRender::from_input_to_output()
{	
	// Copy output properties from input properties
	update_output_params_from_input_params();

	// Copy output params (properties) to dataOut field - channels excluded
	update_ldesc_from_output_params_on_test();

	// Apply the neg_output constraints
	constrain_ldesc_from_neg_params(neg_output);
}
*/
// ===================================================================

jvxErrorType
CjvxAuNBinauralRender::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvxErrorType resL = JVX_NO_ERROR;
	if (res != JVX_NO_ERROR) return res;

	// From this point the processing parameters are fixed. You need to allocate all buffers
	// for your algorithm now. The processing parameters can be found here:
	// _common_set_icon.theData_in->con_params

	JVX_INITIALIZE_MUTEX(this->mutex_convolutions);

	jvxSize samplerate = _common_set_icon.theData_in->con_params.rate;
	theHrtfDispenser->init(samplerate); // TODO: is called per input channel
	
	res = theHrtfDispenser->get_length_hrir(this->length_buffer_hrir);
	assert(res == JVX_NO_ERROR);
	
	this->allocate_hrir_buffers(this->length_buffer_hrir);

	// init convolution modules
	this->frame_advance = _common_set_icon.theData_in->con_params.buffersize;
	
	this->convolutions.reserve(2);
	this->convolutions.push_back(ConvolutionsHrirCurrentNext());
	this->convolutions.push_back(ConvolutionsHrirCurrentNext());

	this->init_convolution_set(this->convolutions.at(0), this->length_buffer_hrir, this->frame_advance);

	this->update_hrirs(this->idx_conv_sofa_current, this->source_direction_angles_deg[0], this->source_direction_angles_deg[1]);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(this->buffer_out_temp, jvxData, _common_set_icon.theData_in->con_params.buffersize);

	return res;
}

// ===================================================================

jvxErrorType JVX_CALLINGCONVENTION CjvxAuNBinauralRender::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io::start_connect_icon(fdb);

	return res;
}

jvxErrorType JVX_CALLINGCONVENTION CjvxAuNBinauralRender::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1io::stop_connect_icon(fdb);
}

jvxErrorType
CjvxAuNBinauralRender::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR) return res;

	// At this point the processing is completely done. You need to deallocate all buffers
	// for your algorithm now. The processing parameters can be found here:
	// _common_set_icon.theData_in->con_params

	this->delete_hrir_buffers();

	JVX_SAFE_DELETE_FIELD(this->buffer_out_temp);

	JVX_TERMINATE_MUTEX(this->mutex_convolutions);

	return res;
}

jvxErrorType
CjvxAuNBinauralRender::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Extract the input buffers. You will find all processing parameters at
	// _common_set_icon.theData_in->con_params
	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData* in = buffers_in[0];

	// Extract the output buffers. You will find the processing parameters at
	// _common_set_ocon.theData_out.con_params
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);
	jvxData* out_left = buffers_out[0];
	jvxData* out_right = buffers_out[1];

	const jvxSize num_channels_in = _common_set_icon.theData_in->con_params.number_channels;
	const jvxSize num_channels_out = _common_set_ocon.theData_out.con_params.number_channels;

	JVX_TRY_LOCK_MUTEX_RESULT_TYPE lock_result = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
	JVX_TRY_LOCK_MUTEX(lock_result, this->mutex_convolutions);

	if (this->sofa_db_dirty) {
		this->idx_conv_sofa_current = this->toggle_idx(this->idx_conv_sofa_current);
		this->sofa_db_dirty = false;
	}

	auto& conv_hrir_current = this->convolutions.at(this->idx_conv_sofa_current).at(this->idx_conv_hrir_current);
	auto& convolution_left_current = conv_hrir_current.at(0);
	auto& convolution_right_current = conv_hrir_current.at(1);

	if (!this->conv_hrir_dirty) {
		convolution_left_current.process(in, out_left);
		convolution_right_current.process(in, out_right);

		jvxSize idx_conv_hrir_inactive = this->toggle_idx(this->idx_conv_hrir_current);
		auto& conv_hrir_inactive = this->convolutions.at(this->idx_conv_sofa_current).at(idx_conv_hrir_inactive);
		conv_hrir_inactive.at(0).process(in, this->buffer_out_temp);
		conv_hrir_inactive.at(1).process(in, this->buffer_out_temp);

	}
	else {
		// Switch of convolution objects with linear interpolation necessary.

		
		jvxSize idx_conv_hrir_next = this->toggle_idx(this->idx_conv_hrir_current);

		auto& convolution_left_next = this->convolutions.at(this->idx_conv_sofa_current).at(idx_conv_hrir_next).at(0);
		auto& convolution_right_next = this->convolutions.at(this->idx_conv_sofa_current).at(idx_conv_hrir_next).at(1);

		const jvxSize num_samples = _common_set_icon.theData_in->con_params.buffersize;
		jvxData const delta_interpolation_weight = 1.0 / ((jvxData)(num_samples + 1));

		// Input to left output channel using old HRIR and linear falling weighting function.
		convolution_left_current.process(in, this->buffer_out_temp);
		linear_weighting(this->buffer_out_temp, num_samples, 1.0, -delta_interpolation_weight);
		std::copy(this->buffer_out_temp, this->buffer_out_temp + num_samples, out_left);

		// Input to right output channel using old HRIR and linear falling weighting function.
		convolution_right_current.process(in, this->buffer_out_temp);
		linear_weighting(this->buffer_out_temp, num_samples, 1.0, -delta_interpolation_weight);
		std::copy(this->buffer_out_temp, this->buffer_out_temp + num_samples, out_right);


		// Input to left output channel using new HRIR and linear rising weighting function.
		convolution_left_next.process(in, this->buffer_out_temp);
		linear_weighting(this->buffer_out_temp, num_samples, 0.0, delta_interpolation_weight);
		for (jvxSize idx_sample = 0; idx_sample < num_samples; idx_sample++) {
			out_left[idx_sample] += this->buffer_out_temp[idx_sample];
		}

		// Input to right output channel using new HRIR and linear rising weighting function.
		convolution_right_next.process(in, this->buffer_out_temp);
		linear_weighting(this->buffer_out_temp, num_samples, 0.0, delta_interpolation_weight);
		for (jvxSize idx_sample = 0; idx_sample < num_samples; idx_sample++) {
			out_right[idx_sample] += this->buffer_out_temp[idx_sample];
		}


		convolution_left_current.set_ir(this->buffer_hrir_left);
		convolution_right_current.set_ir(this->buffer_hrir_right);

		this->idx_conv_hrir_current = idx_conv_hrir_next;
		this->conv_hrir_dirty = false;

	}

	if (lock_result == JVX_TRY_LOCK_MUTEX_SUCCESS)
		JVX_UNLOCK_MUTEX(this->mutex_convolutions);

	
	// Once you are done forward the processing flow to the ext stage. We can do this as follows:
	// 1) Only pass the flow forward to next stage without any functionality
	return _process_buffers_icon(mt_mask, idx_stage);


	// 2) Run the default implementation: Do a circular buffer copy. Deactivate this option if you do your own processing
	// res = CjvxBareNode1io::process_buffers_icon(mt_mask, idx_stage);
	// return res;
}


void
CjvxAuNBinauralRender::init_convolution_set(ConvolutionsHrirCurrentNext& convolutions, jvxSize length_ir, jvxSize frame_advance) {
	
	const jvxSize num_hrirs_per_sofa_db = 2;
	const jvxSize num_convs_per_hrir = 2;
	convolutions.clear();

	convolutions.reserve(num_hrirs_per_sofa_db);
	for (int idx_sets = 0; idx_sets < num_hrirs_per_sofa_db; idx_sets++) {
		convolutions.push_back(ConvolutionsLR());
		auto& new_hrir_pair = convolutions.back();
		new_hrir_pair.reserve(num_convs_per_hrir);
		for (int idx_conv = 0; idx_conv < num_convs_per_hrir; idx_conv++) {
			new_hrir_pair.push_back(CjvxFastConvolution());
			new_hrir_pair.back().init(length_ir, frame_advance);
		}
	}
}


void
CjvxAuNBinauralRender::update_hrirs(jvxSize idx_conv_sofa, jvxData azimuth_deg, jvxData inclination_deg) {
	
	// std::cout << "New angles: azimuth: " << azimuth_deg << ", inclination: " << inclination_deg << std::endl;

	// Update state array.
	this->source_direction_angles_deg[0] = azimuth_deg;
	this->source_direction_angles_deg[1] = inclination_deg;

	if (this->length_buffer_hrir != JVX_SIZE_UNSELECTED) {
		
		jvxSize length_hrir;
		this->theHrtfDispenser->get_length_hrir(length_hrir);
		assert(this->length_buffer_hrir == length_hrir);

	    this->theHrtfDispenser->copy_closest_hrir_pair(azimuth_deg, inclination_deg, this->buffer_hrir_left, this->buffer_hrir_right, this->length_buffer_hrir);

		jvxSize const idx_hrir_next = this->toggle_idx(this->idx_conv_hrir_current);
		
		auto& conv_hrir_next = this->convolutions.at(idx_conv_sofa).at(idx_hrir_next);
		conv_hrir_next.at(0).set_ir(this->buffer_hrir_left);
		conv_hrir_next.at(1).set_ir(this->buffer_hrir_right);
		
		this->conv_hrir_dirty = true;
	}
	
}

jvxSize CjvxAuNBinauralRender::toggle_idx(jvxSize idx)
{
	return 1 - idx;
}


void 
CjvxAuNBinauralRender::linear_weighting(jvxData *inout, jvxSize num_weights, jvxData weight_start, jvxData weight_delta)
{
	jvxData interpolation_weight = weight_start;
	for (jvxSize idx_weight = 0; idx_weight < num_weights; idx_weight++) {
		interpolation_weight += weight_delta;
		inout[idx_weight] *= interpolation_weight;
	}
}

void CjvxAuNBinauralRender::allocate_hrir_buffers(jvxSize length_ir)
{
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_left);
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_right);

	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(this->buffer_hrir_left, jvxData, length_ir);
	JVX_SAFE_ALLOCATE_FIELD_CPP_Z(this->buffer_hrir_right, jvxData, length_ir);
}

void CjvxAuNBinauralRender::delete_hrir_buffers()
{
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_left);
	JVX_SAFE_DELETE_FIELD(this->buffer_hrir_right);
}

/*
void
CjvxAuNPlayChannelId::test_set_output_parameters()
{

}
*/

// ===================================================================
// ===================================================================
