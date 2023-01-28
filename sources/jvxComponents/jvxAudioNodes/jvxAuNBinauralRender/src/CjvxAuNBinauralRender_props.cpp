#include "CjvxAuNBinauralRender.h"

jvxErrorType
CjvxAuNBinauralRender::get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxBareNode1io_rearrange::get_property(callGate,
		rawPtr, ident, trans);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBinauralRender, update_source_direction)
{
	/* Check if HRIR update is necessary by comparing state direction array to input porperty. */
	bool hrirs_dirty = false;

	if (this->input_source_direction_angles_deg[0] != this->source_direction_angles_deg[0]) {
		hrirs_dirty = true;
	}

	if (this->input_source_direction_angles_deg[1] != this->source_direction_angles_deg[1]) {
		hrirs_dirty = true;
	}

	if (hrirs_dirty) {
		const jvxData azimuth_deg = this->input_source_direction_angles_deg[0];
		const jvxData inclination_deg = this->input_source_direction_angles_deg[1];

		update_hrirs(this->idx_conv_sofa_current, azimuth_deg, inclination_deg);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBinauralRender, set_extend_ifx_reference)
{
	const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender>* ptrRawToInstall =
		castPropRawPointer< const jvx::propertyRawPointerType::CjvxRawPointerTypeExternalPointer<IjvxPropertyExtender> >(
			genBinauralRender_node::global.if_ext.interf_hrtf_data.rawPtrRegister,
			JVX_DATAFORMAT_INTERFACE);
	if (ptrRawToInstall)
	{
		IjvxPropertyExtender** instSpace = ptrRawToInstall->typedPointer();
		if (instSpace)
		{
			IjvxPropertyExtender* ptr = *instSpace;
			if (ptr)
			{
				ptr->prop_extender_specialization(reinterpret_cast<jvxHandle**>(&theHrtfDispenser), jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER);
				if (theHrtfDispenser)
				{
					theHrtfDispenser->register_change_listener(static_cast<IjvxPropertyExtenderHrtfDispenser_report*>(this));
				}
			}
			else
			{
				if (theHrtfDispenser)
				{
					theHrtfDispenser->unregister_change_listener(static_cast<IjvxPropertyExtenderHrtfDispenser_report*>(this));

				}
				theHrtfDispenser = nullptr;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuNBinauralRender::report_database_changed()
{	
	if (this->frame_advance == JVX_SIZE_UNSELECTED) return JVX_ERROR_NOT_READY;

	// This function callback indicates, that the active SOFA database in the ayfHrtfDispenser has been changed.
	JVX_LOCK_MUTEX(this->mutex_convolutions);

	jvxSize new_length_hrir;
	this->theHrtfDispenser->get_length_hrir(new_length_hrir);

	if (this->length_buffer_hrir != new_length_hrir) {
		this->length_buffer_hrir = new_length_hrir;

		this->allocate_hrir_buffers(this->length_buffer_hrir);
		jvxSize idx_conv_sofa_next = this->toggle_idx(this->idx_conv_sofa_current);
		this->convolutions.at(idx_conv_sofa_next) = ConvolutionsHrirCurrentNext();
		auto& new_convolution = this->convolutions.at(idx_conv_sofa_next);
		this->init_convolution_set(new_convolution, this->length_buffer_hrir, this->frame_advance);

		const jvxData azimuth_deg = this->source_direction_angles_deg[0];
		const jvxData inclination_deg = this->source_direction_angles_deg[1];

		this->update_hrirs(idx_conv_sofa_next, azimuth_deg, inclination_deg);

		this->sofa_db_dirty = true;
	}
	else {
		const jvxData azimuth_deg = this->source_direction_angles_deg[0];
		const jvxData inclination_deg = this->source_direction_angles_deg[1];

		this->update_hrirs(this->idx_conv_sofa_current, azimuth_deg, inclination_deg);
	}

	JVX_UNLOCK_MUTEX(this->mutex_convolutions);

	return JVX_NO_ERROR;
}
