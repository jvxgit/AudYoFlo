#include "jvxNodes/CjvxNVTasks.h"

/* This thread only for master connection */
jvxErrorType
CjvxNVTasks::process_buffers_icon_nvtask_master(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize ii;
	jvxSize maxChans = 0;
	jvxSize minChans = 0;

	/* =======================================================================
	 * This is the location at which the master thread delivers and receives the audio samples 
	 * =======================================================================
	 */

#ifdef JVX_CJVXNVTASKS_UPDATE_MASTER_RATE

	jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStampRates);
	if (JVX_CHECK_SIZE_SELECTED(tStampRates_last_out))
	{
		jvxTick deltat = tt - tStampRates_last_out;
		jvxData deltaf = (jvxData)deltat / (jvxSize)theData_out->con_params.buffersize;
		if (delta_t_smoothed < 0)
		{
			delta_t_smoothed = deltaf;
		}
		else
		{
			delta_t_smoothed = delta_t_smoothed * JVX_ALPHA_RATE + deltaf * (1.0 - JVX_ALPHA_RATE);
		}
		jvxData rate_current = 1000000.0 / delta_t_smoothed; 
		CjvxNVTasks_pcg::monitor.rate.value = rate_current;
	}
	tStampRates_last_out = tt;
#endif

	/* =======================================================================
	 * Reset output buffers to zero to collect data from all sources
	 * =======================================================================
     */

	jvxSize idx_stage_out = *theData_out->con_pipeline.idx_stage_ptr;
	for (ii = 0; ii < theData_out->con_params.number_channels; ii++)
	{
		jvxByte* ptrZ = (jvxByte*)theData_out->con_data.buffers[idx_stage_out][ii];
		jvxSize bSz = theData_out->con_params.buffersize * jvxDataFormat_getsize(theData_out->con_params.format);
		memset(ptrZ, 0, bSz);
	}

	/* =======================================================================
	 * Forward this request to the Ntask function to pull in all ntask data
	 * =======================================================================
	 */

	res = CjvxBareNtask::process_buffers_icon_nvtask_master(
		theData_in,
		theData_out,
		mt_mask, idx_stage);

	/* =======================================================================
	 * Pull in all vtask connections
	 * =======================================================================
	 */

	JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

	// In this list we keep the "active" vtask connections
	auto elm = _common_set_nv_proc.lst_in_proc_tasks.begin();
	for (; elm != _common_set_nv_proc.lst_in_proc_tasks.end();elm++)
	{
		jvxErrorType resL = JVX_NO_ERROR;
		resL = elm->second.icon->con->transfer_backward_icon(JVX_LINKDATA_TRANSFER_REQUEST_DATA, NULL, 0);
	}
	JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	return res;
}

/* This callback comes from the N-task connectors */
jvxErrorType 
CjvxNVTasks::process_buffers_icon_ntask_attached(
	jvxSize ctxtId,
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize mt_mask, jvxSize idx_stage)
{
	/* =======================================================================
	 * In this callback the attached ntask chains are handled individually
	 * =======================================================================
	 */

	jvxErrorType res = JVX_NO_ERROR;
	return res;
}
	
jvxErrorType
CjvxNVTasks::process_buffers_icon_vtask_attached(
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>*& icon,
	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* >& ocon,
	jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	jvxSize maxChans = 0;
	jvxSize minChans = 0;
	jvxSize ii;

	/* =======================================================================
	 * This function is called from within the 0task thread. Hence, the audio i/o buffers
	 * are available and it is safe to access them via a local variable.
	 * =======================================================================
	 */

	jvxLinkDataDescriptor* data_in_master = _common_set_nvtask_proc.theData_in_master;
	jvxLinkDataDescriptor* data_out_master = _common_set_nvtask_proc.theData_out_master;
	jvxSize idx_stage_in_master = _common_set_nvtask_proc.idx_stage_in_master;
	jvxSize idx_stage_out_master = _common_set_nvtask_proc.idx_stage_out_master;

	/* =======================================================================
	 * Obtain the data from the current vtask
	 * =======================================================================
	 */
	jvxLinkDataDescriptor* data_in_att = icon->con->_common_set_icon_nvtask.theData_in;
	jvxSize idx_stage_in_att = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_in_att))
	{
		idx_stage_in_att = *data_in_att->con_pipeline.idx_stage_ptr;
	}

	maxChans = JVX_MAX(data_in_att->con_params.number_channels, data_out_master->con_params.number_channels);
	minChans = JVX_MIN(data_in_att->con_params.number_channels, data_out_master->con_params.number_channels);

	// This default function does not tolerate a lot of unexpected settings
	if (data_in_att->con_params.format_group != JVX_DATAFORMAT_GROUP_TRIGGER_ONLY)
	{
		assert(data_in_att->con_params.format == data_out_master->con_params.format);
		assert(data_in_att->con_params.buffersize == data_out_master->con_params.buffersize);

		// Cross copy mode: Copy input from attached task to master task!
		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % data_in_att->con_params.number_channels;
				jvxSize idxOut = ii % data_out_master->con_params.number_channels;

				switch (combinSignalMode)
				{
				case jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_REPLACE:
					jvx_convertSamples_memcpy(
						data_in_att->con_data.buffers[idx_stage_in_att][idxIn],
						data_out_master->con_data.buffers[idx_stage_out_master][idxOut],
						jvxDataFormat_size[data_in_att->con_params.format],
						data_in_att->con_params.buffersize);
					break;
				case jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_MIX:
					jvx_mixSamples_flp(
						(jvxData*)data_in_att->con_data.buffers[idx_stage_in_att][idxIn],
						(jvxData*)data_out_master->con_data.buffers[idx_stage_out_master][idxOut],
						data_in_att->con_params.buffersize);
					break;
				default:
					assert(0);
				}
			}
		}
	}

	// Here we approach the output connectors
	auto elm = ocon.begin();
	for(; elm != ocon.end(); elm++)
	{
		
		if ((*elm)->con)
		{
			jvxLinkDataDescriptor* data_out_vtask = &(*elm)->con->_common_set_ocon_nvtask.theData_out;
			jvxSize idx_stage_out_vtask = *data_out_vtask->con_pipeline.idx_stage_ptr;

			// What to do here?
			maxChans = JVX_MAX(data_out_vtask->con_params.number_channels, data_in_master->con_params.number_channels);
			minChans = JVX_MIN(data_out_vtask->con_params.number_channels, data_in_master->con_params.number_channels);

			assert(data_out_vtask->con_params.format == data_in_master->con_params.format);
			assert(data_out_vtask->con_params.buffersize == data_in_master->con_params.buffersize);

			// Cross copy mode: Copy input from attached task to master task!
			if (minChans)
			{
				for (ii = 0; ii < maxChans; ii++)
				{
					jvxSize idxIn = ii % data_in_master->con_params.number_channels;
					jvxSize idxOut = ii % data_out_vtask->con_params.number_channels;

					switch (combinSignalMode)
					{
					case jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_REPLACE:
						jvx_convertSamples_memcpy(
							data_in_master->con_data.buffers[idx_stage_in_master][idxIn],
							data_out_vtask->con_data.buffers[idx_stage_out_vtask][idxOut],
							jvxDataFormat_size[data_in_master->con_params.format],
							data_in_att->con_params.buffersize);
						break;
					case jvxCombineSignalsMode::JVX_COMBINE_SIGNALS_MODE_MIX:
						jvx_mixSamples_flp(
							(jvxData*)data_in_master->con_data.buffers[idx_stage_in_master][idxIn],
							(jvxData*)data_out_vtask->con_data.buffers[idx_stage_out_vtask][idxOut],
							data_in_master->con_params.buffersize);
						break;
					default:
						assert(0);
					}
				}
			}
		}
	}
	return res;
}

/** 
 * Enter the vtask access in the main processing callback!
 */
jvxErrorType
CjvxNVTasks::process_buffers_icon_vtask(
	jvxSize mt_mask,
	jvxSize idx_stage,
	jvxSize procId)
{
	jvxErrorType res = JVX_NO_ERROR;

	auto elm = _common_set_nv_proc.lst_in_proc_tasks.find(procId);

	/* =======================================================================
	 * Do the core processing for one vtask
	 * =======================================================================
	 */

	res = this->process_buffers_icon_vtask_attached(
		elm->second.icon, elm->second.ocon, mt_mask, idx_stage);

	/* =======================================================================
	 * Forward the output chain routes 
	 * =======================================================================
	 */
	auto elmO = elm->second.ocon.begin();
	for (; elmO != elm->second.ocon.end(); elmO++)
	{
		// This may also work without any output connectors
		if ((*elmO)->con)
		{
			res = (*elmO)->con->process_buffers_ocon();

			if (res != JVX_NO_ERROR)
				break;
		}
	}
	return res;
}
