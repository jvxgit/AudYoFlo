#include "jvx.h"
#include "CjvxAuNAnalyze.h"

CjvxAuNAnalyze::CjvxAuNAnalyze(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	// Initialize the internal pointer for correct state machine
	theSpectrumMatHdl.prv = NULL;
	jvx_spectral_plot_initCfg(&theSpectrumMatHdl);
	theSpectrumMatHdl.initParam.bufferSize = 1024; // _common_set_icon.theData_in->con_params.buffersize;
	theSpectrumMatHdl.initParam.channels = 1;
	theSpectrumMatHdl.initParam.fftSize = JVX_FFT_TOOLS_FFT_SIZE_1024;
	theSpectrumMatHdl.initParam.hopSize = 1024;
	theSpectrumMatHdl.asyncParam.highdB = 0;
	theSpectrumMatHdl.asyncParam.lowdB = -120;

	theWaveformMatHdl.prv = NULL;
	jvx_waveform_plot_initCfg(&theWaveformMatHdl);
	theWaveformMatHdl.initPrm.bsize = 1024;
	theWaveformMatHdl.initPrm.amplMin = -1.0;
	theWaveformMatHdl.initPrm.amplMax = 1.0;
	theWaveformMatHdl.asyncParam.gainFactor = 1.0;

	forward_complain = true;
}

CjvxAuNAnalyze::~CjvxAuNAnalyze()
{
}

jvxErrorType
CjvxAuNAnalyze::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if(res == JVX_NO_ERROR)
	{
		genAnalyzeAudio_node::init_all();
		genAnalyzeAudio_node::allocate_all();
		genAnalyzeAudio_node::register_all(static_cast<CjvxProperties*>(this));

		genAnalyzeAudio_node::associate__expose_plot__visual_data_plot(this, 

			&theSpectrumMatHdl.asyncParam.sensFactor, 1,
			&theSpectrumMatHdl.asyncParam.lowdB, 1,
			&theSpectrumMatHdl.asyncParam.highdB, 1,

			&theWaveformMatHdl.asyncParam.gainFactor, 1,
			& theWaveformMatHdl.asyncParam.smoothFactor, 1
		);

		theWaveformMatHdl.asyncParam.plotMode = genAnalyzeAudio_node::translate__expose_plot__visual_data_plot__plot_mode_from();

		genAnalyzeAudio_node::expose_plot.visual_data_plot.format.value = JVX_DATAFORMAT_BYTE;
		// only on init: genMf_device::expose_visual_if.visual_data_if.operation_mode = ...;
		// only on set_ext_ref: genMf_device::expose_visual_if.visual_data_if.rendering_external = ...;
		genAnalyzeAudio_node::expose_plot.visual_data_plot.segmentsize_x.value = 513;
		genAnalyzeAudio_node::expose_plot.visual_data_plot.segmentsize_y.value = 1024;
		genAnalyzeAudio_node::expose_plot.visual_data_plot.subformat.value = JVX_DATAFORMAT_GROUP_VIDEO_RGBA32;
		jvx_bitZSet(genAnalyzeAudio_node::expose_plot.visual_data_plot.operation_mode.value.selection(), 1);		
		genAnalyzeAudio_node::register_callbacks(this, cb_set_props_plotting_td, cb_set_props_plotting_fd, 
			/* cb_install_plot_matrix,*/
			cb_set_plot_params, this);
	}
	return(res);
};

jvxErrorType
CjvxAuNAnalyze::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::_pre_check_deactivate();
	if(res == JVX_NO_ERROR)
	{
		genAnalyzeAudio_node::deassociate__expose_plot__visual_data_plot(this);
		genAnalyzeAudio_node::unregister_callbacks(this);
		genAnalyzeAudio_node::unregister_all(static_cast<CjvxProperties*>(this));
		genAnalyzeAudio_node::deallocate_all();
		CjvxBareNode1io::deactivate();
	}
	return(res);
};

static jvxUInt64 diffMax = 0;

jvxErrorType
CjvxAuNAnalyze::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numFilled = 0;
	jvxBool allStopped = false;
	jvxSize i,ii, iii;
	jvxData deltaT = 0;
	std::string err;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool plotTd = false;
	jvxSize maxChans;
	jvxSize minChans;
	jvxSize idxIn;
	jvxSize idxOut;
	jvxData* outChan_data = NULL;
	jvxData* inChan_data = NULL;

	jvxData** buffers_in = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
	jvxData** buffers_out = jvx_process_icon_extract_output_buffers<jvxData>(_common_set_ocon.theData_out);

	// This default function does not tolerate a lot of unexpected settings
	assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);
	assert(_common_set_icon.theData_in->con_params.buffersize == _common_set_ocon.theData_out.con_params.buffersize);
	assert(_common_set_icon.theData_in->con_params.format == _common_set_ocon.theData_out.con_params.format);

	// ================================================================================================	
	// ================================================================================================
	// ================================================================================================

	jvxData* ptrShowMe = NULL;
	jvxExternalBuffer* myExternalBuffer = NULL;

	// ==============================================================
	// Select data buffer
	// ==============================================================

	jvxSize idSelect = jvx_bitfieldSelection2Id(genAnalyzeAudio_node::what_to_plot.select_channel.value.selection(),
		genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.size());
	if (idSelect < _common_set_icon.theData_in->con_params.number_channels)
	{
		ptrShowMe = (jvxData*)buffers_in[idSelect];
	}
	else
	{
		idSelect -= _common_set_icon.theData_in->con_params.number_channels;
		if (idSelect < _common_set_ocon.theData_out.con_params.number_channels)
		{
			ptrShowMe = (jvxData*)buffers_out[idSelect];
		}
	}

	// ==============================================================
	// Copy data to external buffer
	// ==============================================================

	if (ptrShowMe)
	{
		myExternalBuffer = genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target.ptr;
		if (myExternalBuffer)
		{
			plotTd = true;
			jvxSize idxWrite = 0;

			myExternalBuffer->safe_access.lockf(myExternalBuffer->safe_access.priv);
			idxWrite = (myExternalBuffer->idx_read + myExternalBuffer->fill_height);
			myExternalBuffer->safe_access.unlockf(myExternalBuffer->safe_access.priv);

			jvxBool writeRow = (runtime.sub_sample_cnt == 0);

			if (writeRow)
			{
				if (myExternalBuffer->fill_height < myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_y)
				{
					idxWrite = idxWrite % myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_y;

					jvxByte* ptrRow = myExternalBuffer->ptrFld;
					ptrRow += idxWrite * JVX_DATA2SIZE(myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x * myExternalBuffer->szElmFld);

					if (plotTd)
					{
						jvx_waveform_plot_process(&theWaveformMatHdl, ptrShowMe, ptrRow,
							myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
					} // if (plotTd)					
					else
					{
						jvx_spectral_plot_process(&theSpectrumMatHdl, ptrShowMe, ptrRow,
							myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
					}

					myExternalBuffer->safe_access.lockf(myExternalBuffer->safe_access.priv);
					myExternalBuffer->fill_height++;
					myExternalBuffer->safe_access.unlockf(myExternalBuffer->safe_access.priv);
				}
			} // if (writeRow)
			else
			{
				if (plotTd)
				{
					jvx_waveform_plot_process(&theWaveformMatHdl, ptrShowMe, NULL,
						myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
				}
				else
				{
					jvx_spectral_plot_process(&theSpectrumMatHdl, ptrShowMe, NULL,
						myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
				}
			}
		} // if (myExternalBuffer)

		myExternalBuffer = genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target_sec.ptr;
		if (myExternalBuffer)
		{
			jvxSize idxWrite = 0;
			plotTd = false;
			myExternalBuffer->safe_access.lockf(myExternalBuffer->safe_access.priv);
			idxWrite = (myExternalBuffer->idx_read + myExternalBuffer->fill_height);
			myExternalBuffer->safe_access.unlockf(myExternalBuffer->safe_access.priv);

			jvxBool writeRow = (runtime.sub_sample_cnt == 0);
			if (writeRow)
			{
				if (myExternalBuffer->fill_height < myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_y)
				{
					idxWrite = idxWrite % myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_y;

					jvxByte* ptrRow = myExternalBuffer->ptrFld;
					ptrRow += idxWrite * JVX_DATA2SIZE(myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x * myExternalBuffer->szElmFld);

					if (plotTd)
					{
						jvx_waveform_plot_process(&theWaveformMatHdl, ptrShowMe, ptrRow,
							myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
					} // if (plotTd)					
					else
					{
						jvx_spectral_plot_process(&theSpectrumMatHdl, ptrShowMe, ptrRow,
							myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
					}

					myExternalBuffer->safe_access.lockf(myExternalBuffer->safe_access.priv);
					myExternalBuffer->fill_height++;
					myExternalBuffer->safe_access.unlockf(myExternalBuffer->safe_access.priv);
				}
			} // if (writeRow)
			else
			{
				if (plotTd)
				{
					jvx_waveform_plot_process(&theWaveformMatHdl, ptrShowMe, NULL,
						myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
				}
				else
				{
					jvx_spectral_plot_process(&theSpectrumMatHdl, ptrShowMe, NULL,
						myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_x);
				}
			}
		}
		runtime.sub_sample_cnt = (runtime.sub_sample_cnt + 1) % runtime.sub_sample_factor;
	}

	res = fwd_process_buffers_icon(mt_mask, idx_stage);
	return(res);
};

jvxErrorType
CjvxAuNAnalyze::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
		theSpectrumMatHdl.initParam.hopSize = _common_set_icon.theData_in->con_params.buffersize;		
		jvx_spectral_plot_init(&theSpectrumMatHdl);
		
		theWaveformMatHdl.initPrm.bsize = _common_set_icon.theData_in->con_params.buffersize;
		jvx_waveform_plot_init(&theWaveformMatHdl);

		runtime.sub_sample_cnt = 0;
		runtime.sub_sample_factor = floor((jvxData)theSpectrumMatHdl.initParam.bufferSize / (jvxData)theSpectrumMatHdl.initParam.hopSize);
		runtime.sub_sample_factor = JVX_MAX(runtime.sub_sample_factor, 1);

	}
	return(res);
}

jvxErrorType
CjvxAuNAnalyze::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
		jvx_waveform_plot_terminate(&theWaveformMatHdl);
		jvx_spectral_plot_terminate(&theSpectrumMatHdl);

		runtime.sub_sample_cnt = 0;
		runtime.sub_sample_factor = 1;

	}
	return(res);
}

jvxErrorType 
CjvxAuNAnalyze::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = CjvxBareNode1io::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));
	if (res == JVX_NO_ERROR)
	{
		update_plot_part2();
	}
	return res;
}

jvxErrorType
CjvxAuNAnalyze::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize i;

	jvxErrorType res = CjvxBareNode1io::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		update_plot_part2();
	}
	return res;
}

void 
CjvxAuNAnalyze::update_plot_part2()
{
	jvxSize i;

	std::string selChannel;
	std::string newChannel;
	jvxSize idSelect = jvx_bitfieldSelection2Id(genAnalyzeAudio_node::what_to_plot.select_channel.value.selection(),
		genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.size());
	if (idSelect < genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.size())
	{
		selChannel = genAnalyzeAudio_node::what_to_plot.select_channel.value.entries[idSelect];
	}
	idSelect = JVX_SIZE_UNSELECTED;
	genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.clear();

		for (i = 0; i < _common_set_icon.theData_in->con_params.number_channels; i++)
	{
		newChannel = "Channel #" + jvx_size2String(i);
		genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.push_back(newChannel.c_str());
		if (newChannel == selChannel)
		{
			idSelect = i;
		}
	}
	
	if (idSelect < genAnalyzeAudio_node::what_to_plot.select_channel.value.entries.size())
	{
		jvx_bitZSet(genAnalyzeAudio_node::what_to_plot.select_channel.value.selection(), idSelect);
	}
	else
	{
		jvx_bitZSet(genAnalyzeAudio_node::what_to_plot.select_channel.value.selection(), 0);
	}

	// Compute some values for the axis descitpion in external plotting view
	jvxSize ssFac = floor((jvxData)theSpectrumMatHdl.initParam.bufferSize / (jvxData)_common_set_icon.theData_in->con_params.buffersize);
	jvxData tDiffFrame = (jvxData)_common_set_icon.theData_in->con_params.buffersize / (jvxData)_common_set_icon.theData_in->con_params.rate;
	jvxData tDiffShow = ssFac * tDiffFrame;
	genAnalyzeAudio_node::axis_description.unit_x.value = tDiffShow;
	// genAnalyzeAudio_node::axis_description.descr_x.value = "Time [secs]"; // <- set in pcg file

	genAnalyzeAudio_node::axis_description.max_y_pri.value = 1;
	genAnalyzeAudio_node::axis_description.min_y_pri.value = -1;

	genAnalyzeAudio_node::axis_description.max_y_sec.value = _common_set_icon.theData_in->con_params.rate/2;
	genAnalyzeAudio_node::axis_description.min_y_sec.value = 0;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNAnalyze, cb_set_plot_params)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNAnalyze, cb_set_props_plotting_td)
{
	jvxBool updateWaveformAsync = false;

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.gain_factor_waveform))
	{
		updateWaveformAsync = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.smooth_factor_waveform))
	{
		theWaveformMatHdl.asyncParam.smoothFactor = JVX_MIN(theWaveformMatHdl.asyncParam.smoothFactor, 0.99999);
		theWaveformMatHdl.asyncParam.smoothFactor = JVX_MAX(theWaveformMatHdl.asyncParam.smoothFactor, 0.0);
		updateWaveformAsync = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.plot_mode))
	{
		updateWaveformAsync = true;
		theWaveformMatHdl.asyncParam.plotMode = genAnalyzeAudio_node::translate__expose_plot__visual_data_plot__plot_mode_from();
	}

	if (updateWaveformAsync)
	{
		jvx_waveform_plot_update(&theWaveformMatHdl, JVX_DSP_UPDATE_ASYNC, c_true);
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNAnalyze, cb_set_props_plotting_fd)
{
	jvxBool updateSpectrumAsync = false;

	if (
		JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.sensitivity_spectrum) 
		)
	{
		updateSpectrumAsync = true;
	}

	if (updateSpectrumAsync)
	{
		jvx_spectral_plot_update(&theSpectrumMatHdl, JVX_DSP_UPDATE_ASYNC, c_true);
	}

	return JVX_NO_ERROR;
}

/*
JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNAnalyze, cb_install_plot_matrix)
{
	

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target))
	{
		myExternalBuffer = genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target.ptr;		
	}

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target_sec))
	{
		myExternalBuffer = genAnalyzeAudio_node::expose_plot.visual_data_plot.rendering_target_sec.ptr;
	}

	if (myExternalBuffer)
	{
		genAnalyzeAudio_node::axis_description.x_min.value = -tDiffShow * myExternalBuffer->specific.the2DFieldBuffer_inc.common.seg_y;
		genAnalyzeAudio_node::axis_description.x_max.value = 0;
	}
	else
	{
		genAnalyzeAudio_node::axis_description.x_min.value = 0;
		genAnalyzeAudio_node::axis_description.x_max.value = 1;
	}
	return JVX_NO_ERROR;
}
*/