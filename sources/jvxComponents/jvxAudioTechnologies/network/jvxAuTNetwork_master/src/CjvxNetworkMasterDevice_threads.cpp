#include "CjvxNetworkMasterDevice.h"

#define ALPHA 0.95

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif
	
jvxErrorType 
CjvxNetworkMasterDevice::st_callback_thread_startup(jvxHandle* privateData, jvxInt64 timestamp_us)
{
	if(privateData)
	{
		CjvxNetworkMasterDevice* this_ptr = reinterpret_cast<CjvxNetworkMasterDevice*>(privateData);

		return(this_ptr->ic_callback_thread_startup(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkMasterDevice::st_callback_thread_timer_expired(jvxHandle* privateData, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData)
	{
		CjvxNetworkMasterDevice* this_ptr = reinterpret_cast<CjvxNetworkMasterDevice*>(privateData);

		return(this_ptr->ic_callback_thread_timer_expired(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkMasterDevice::st_callback_thread_wokeup(jvxHandle* privateData, jvxInt64 timestamp_us, jvxSize* delta_ms)
{
	if(privateData)
	{
		CjvxNetworkMasterDevice* this_ptr = reinterpret_cast<CjvxNetworkMasterDevice*>(privateData);

		return(this_ptr->ic_callback_thread_wokeup(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType 
CjvxNetworkMasterDevice::st_callback_thread_stopped(jvxHandle* privateData, jvxInt64 timestamp_us)
{
	if(privateData)
	{
		CjvxNetworkMasterDevice* this_ptr = reinterpret_cast<CjvxNetworkMasterDevice*>(privateData);

		return(this_ptr->ic_callback_thread_stopped(timestamp_us));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType 
CjvxNetworkMasterDevice::ic_callback_thread_startup(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkMasterDevice::ic_callback_thread_timer_expired(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkMasterDevice::ic_callback_thread_wokeup(jvxInt64 timestamp_us)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxInt32 bufTimingInfo[JVX_MASTER_NETWORK_NUMBER_ENTRIES_TIMING] = {0};
	jvxInt32* theBufPtr = bufTimingInfo;
	
	while(inProcessing.fillHeight > 0)	
	{

		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
			_common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
		}		


		// Copy incoming data into logfile writer
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE rr = JVX_TRY_LOCK_MUTEX_SUCCESS;
		JVX_TRY_LOCK_MUTEX(rr, safeAccess_Otf);
		if (rr == JVX_TRY_LOCK_MUTEX_SUCCESS)
		{
			if (HjvxDataLogger_Otf::status == JVX_STATE_PROCESSING)
			{
				jvxData tmp = 0;
				// Write the frames to log file
				if (dataLogging.fldWrite_rawaudio)
				{
					for (i = 0; i < _inproc.numInputs; i++)
					{
						std::string errD;
						memcpy(dataLogging.fldWrite_rawaudio[i],
							 _common_set_ldslave.theData_out.con_data.buffers[
								 *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr][i],
							_common_set_ldslave.theData_out.con_params.buffersize *
							jvxDataFormat_size[_common_set_ldslave.theData_out.con_params.format]);
						HjvxDataLogger::_add_data_reference(0, 0, (jvxInt32)dataLogging.idxWrite_rawaudio[i], errD, false);
					}
					HjvxDataLogger::_fill_height(tmp);
					genNetworkMaster_device::timinglogtofile.properties_logtofile.fillHeightBuffer.value.val() = tmp * 100;
				}
			}
			else
			{
				genNetworkMaster_device::timinglogtofile.properties_logtofile.fillHeightBuffer.value.val() = 0;
			}
			JVX_UNLOCK_MUTEX(safeAccess_Otf);
		}

		// ===========================================================================================
		// Store timing info in logfile writer
		JVX_LOG_TO_FILE(JVX_PROFILE_PROCESS_BUFFER_START, 0, 0, inProcessing.logId, 0);
	
		jvxTick t0 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&converters.tStamp);
		if (inConnection.connectedPartner.subformat_in.use == JVX_DATAFORMAT_GROUP_AUDIO_PDM)
		{
			for (i = 0; i < _common_set_ldslave.theData_out.con_params.number_channels; i++)
			{
				jvx_pdm_fir_lut_process_ip(&converters.pdm2PcmConverter[i],
					(jvxInt16*)_common_set_ldslave.theData_out.con_data.buffers[
						*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr][i],
							_common_set_ldslave.theData_out.con_params.buffersize);
			}
		}
		jvxTick t1 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&converters.tStamp);

		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
			_common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
		}

		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
			_common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
		}

		// ==============================================================
		// Timing estimation!
		// ==============================================================
		jvxTick t2 = JVX_GET_TICKCOUNT_US_GET_CURRENT(&converters.tStamp);

		genNetworkMaster_device::profiling.deltaT_converters.value = (ALPHA)* genNetworkMaster_device::profiling.deltaT_converters.value +
			(1 - ALPHA)*((jvxData)(t1 - t0));

		genNetworkMaster_device::profiling.deltaT_processing.value = (ALPHA)* genNetworkMaster_device::profiling.deltaT_processing.value +
			(1 - ALPHA)*((jvxData)(t2 - t1));

		if (JVX_CHECK_SIZE_SELECTED(converters.timeOld))
		{
			genNetworkMaster_device::profiling.deltaT_buffers.value = (ALPHA)* genNetworkMaster_device::profiling.deltaT_buffers.value +
				(1 - ALPHA)*((jvxData)(t0 - converters.timeOld));

			genNetworkMaster_device::integrateiplink.properties_running.profile_load_convert.value =
				ALPHA * genNetworkMaster_device::integrateiplink.properties_running.profile_load_convert.value +
				(1 - ALPHA)* (genNetworkMaster_device::profiling.deltaT_converters.value / converters.deltaT_theo * 100.0);
			genNetworkMaster_device::integrateiplink.properties_running.profile_load_process.value =
				ALPHA * genNetworkMaster_device::integrateiplink.properties_running.profile_load_process.value +
				(1 - ALPHA)* (genNetworkMaster_device::profiling.deltaT_processing.value / converters.deltaT_theo * 100.0);
		}
		converters.timeOld = t0;

		// ==================================================================================
		JVX_LOG_TO_FILE(JVX_PROFILE_PROCESS_BUFFER_STOP, 0, 0, inProcessing.logId, 0);

	}

	return(JVX_NO_ERROR);
}

jvxErrorType 
CjvxNetworkMasterDevice::ic_callback_thread_stopped(jvxInt64 timestamp_us)
{
	return(JVX_NO_ERROR);
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
