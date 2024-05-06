#include "CjvxAuNBinauralRender.h"

jvxErrorType
CjvxAuNBinauralRender::get_property(
	jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxBareNode1ioRearrange::get_property(callGate,
		rawPtr, ident, trans);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBinauralRender, update_source_direction)
{
	jvxData azimuth = this->source_direction_angles_deg_set[0];
	jvxData inclination = this->source_direction_angles_deg_set[1];
	try_trigger_update_position(azimuth, inclination);
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
					jvxSize numSlots = 0;
					theHrtfDispenser->number_slots(numSlots);
					genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf.value.entries.clear();
					for (jvxSize i = 0; i < numSlots; i++)
					{
						jvxApiString astr;
						theHrtfDispenser->slot_descrption(i, astr);
						genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf.value.entries.push_back(astr.std_str());
					}
					assert(numSlots > 0);
					jvx_bitZSet(genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf.value.selection(0), 0);				
				}
			}
			else
			{
				if (theHrtfDispenser)
				{
					theHrtfDispenser->unregister_change_listener(static_cast<IjvxPropertyExtenderHrtfDispenser_report*>(this));

				}
				theHrtfDispenser = nullptr;
				genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf.value.entries.clear();
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNBinauralRender, update_active_slot)
{
	jvxSize slotIdHrtfs = jvx_bitfieldSelection2Id(genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf);
	report_database_changed(slotIdHrtfs);
	return JVX_NO_ERROR;
}


jvxErrorType
CjvxAuNBinauralRender::report_database_changed(jvxSize slotId)
{	
	jvxSize slotIdHrtfs = jvx_bitfieldSelection2Id(genBinauralRender_node::local.hrtf_rendering.active_slot_hrtf);

	// Only if our slot is in use
	if (slotId == slotIdHrtfs)
	{
		// Here, we need to be careful: the fftw does not allow threaded creation of plans,
		//
		// https://www.fftw.org/fftw3_doc/Thread-safety.html
		//
		// The consequence is that we are not allowed to allocate fftws in parallel threads.
		// Therefore, all fftw allocation must be in the main thread.
		// The update of the position, in contrast, is relocated into another thread since this 
		// update may occur in the main thread but also in the processing thread for high speed 
		// modification.
		jvxBool triggerThread = false;
		JVX_LOCK_MUTEX(safeAccessUpdateBgrd);

		// Cancel all current position updates
		updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF;

		// jvxOneRenderCore* removedThis = render_sec;

		if (render_sec)
		{
			deallocate_renderer(render_sec);
		}

		// Allocate new renderer but as "secondary" renderer for now
		render_sec = allocate_renderer(_common_set_icon.theData_in->con_params.buffersize,
			this->source_direction_angles_deg_inuse[0], this->source_direction_angles_deg_inuse[1]);
		updateDBase = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_READY;
		JVX_UNLOCK_MUTEX(safeAccessUpdateBgrd);
	}
	return JVX_NO_ERROR;
}

void
CjvxAuNBinauralRender::try_trigger_update_position(jvxData azimuth, jvxData inclination)
{
	if (
		(azimuth != this->source_direction_angles_deg_inuse[0]) ||
		(inclination != this->source_direction_angles_deg_inuse[1]))
	{
		jvxBool triggerThread = false;
		JVX_LOCK_MUTEX(safeAccessUpdateBgrd);
		switch (updateHRir)
		{
		case jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_ACCEPT_NEW_TASK:
			// Here, all previous updates had been completed		
			newAzimuth = azimuth;
			newInclination = inclination;
			updateHRir = jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_IN_OPERATION;
			triggerThread = true;
			break;
		case jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_IN_OPERATION:

			// Latest update is pending. We can therefore update the target angle
			newAzimuth = azimuth;
			newInclination = inclination;
			break;

		case jvxRenderingUpdateStatus::JVX_RENDERING_UPDATE_OFF:

			// No realtime processing yet, we can immediately set the target angles
			// No need to trigger any on-the-fly update
			this->source_direction_angles_deg_inuse[0] = this->source_direction_angles_deg_set[0];
			this->source_direction_angles_deg_inuse[1] = this->source_direction_angles_deg_set[1];
			break;

		default:

			// Here, the update can not be performed
			missedUpdatesPosition++;
		}
		JVX_UNLOCK_MUTEX(safeAccessUpdateBgrd);

		if (triggerThread)
		{
			if (threads.cpPtr)
			{
				threads.cpPtr->trigger_wakeup();
			}
		}
	}
}
