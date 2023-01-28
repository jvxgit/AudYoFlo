#include "jvx.h"
#include "CjvxAudioNodeViewer.h"

static int resampleTable[] =
{
	1,	2,	5,	10,	20,	50,	100, 200, 500
};

CjvxAudioNodeViewer::CjvxAudioNodeViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);
	external.circbuffer_input = NULL;
	external.circbuffer_output = NULL;
	runtime.input.resamplingFactor_id = 0;
}

CjvxAudioNodeViewer::~CjvxAudioNodeViewer()
{
}

jvxErrorType
CjvxAudioNodeViewer::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		genViewer_node::init__properties_active_higher();
		genViewer_node::allocate__properties_active_higher();
		genViewer_node::register__properties_active_higher(static_cast<CjvxProperties*>(this));

		genViewer_node::register_callbacks(static_cast<CjvxProperties*>(this), set_buffer_post, set_buffer_pre, 
			reinterpret_cast<jvxHandle*>(this), nullptr);
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeViewer::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genViewer_node::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

		genViewer_node::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genViewer_node::deallocate__properties_active_higher();
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeViewer::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::prepare_sender_to_receiver(theData);

	if(res == JVX_NO_ERROR)
	{
		runtime.input.resamplers = NULL;
		if(theData->con_params.number_channels > 0)
		{
			runtime.input.numResamplers = theData->con_params.number_channels;
			JVX_SAFE_NEW_FLD(runtime.input.resamplers, resampler_plotter_private*, runtime.input.numResamplers);
			for(i = 0; i < runtime.input.numResamplers; i++)
			{
				runtime.input.resamplingFactor_id = jvx_bitfieldSelection2Id(
					genViewer_node::properties_active_higher.resamplingFactor.value.selection(),
					genViewer_node::properties_active_higher.resamplingFactor.value.entries.size());
				
				assert(JVX_CHECK_SIZE_SELECTED(runtime.input.resamplingFactor_id));

				runtime.input.resamplingFactor_time = resampleTable[runtime.input.resamplingFactor_id];
				runtime.input.resamplingFactor_value = 1.0;
				if(external.circbuffer_input)
				{
					jvxData rate = external.circbuffer_input->length / (2.0 * runtime.input.resamplingFactor_time);
					runtime.input.resamplingFactor_value = (jvxData) theData->con_params.rate / rate;
					prefill_buffers_input();
				}

				jvx_resampler_plotter_init(&runtime.input.resamplers[i], runtime.input.resamplingFactor_value);

			}

			runtime.input.deltaT = 1.0/theData->con_params.rate;
			runtime.input.t = 0;
			this->_lock_properties_local();

			genViewer_node::properties_active_higher.inputChannelSelection.value.entries.clear();
			for(i = 0; i < runtime.input.numResamplers; i++)
			{
				genViewer_node::properties_active_higher.inputChannelSelection.value.entries.push_back(("Channel #" + jvx_size2String(i)).c_str());
			}
			if(runtime.input.numResamplers > 0)
			{
				genViewer_node::properties_active_higher.inputChannelSelection.value.selection() = 0x1;
			}
			this->_unlock_properties_local();
		}
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeViewer::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::prepare_complete_receiver_to_sender(theData);

	if(res == JVX_NO_ERROR)
	{
		runtime.input.resamplers = NULL;
		if(theData->con_compat.number_channels > 0)
		{
			runtime.input.numResamplers = theData->con_compat.number_channels;
			JVX_SAFE_NEW_FLD(runtime.output.resamplers, resampler_plotter_private*, runtime.output.numResamplers);
			for(i = 0; i < runtime.output.numResamplers; i++)
			{
				runtime.output.resamplingFactor_id = jvx_bitfieldSelection2Id(
					genViewer_node::properties_active_higher.resamplingFactor.value.selection(),
					genViewer_node::properties_active_higher.resamplingFactor.value.entries.size());

				assert(JVX_CHECK_SIZE_SELECTED(runtime.input.resamplingFactor_id));

				runtime.output.resamplingFactor_value = resampleTable[runtime.output.resamplingFactor_id];

				jvx_resampler_plotter_init(&runtime.output.resamplers[i], runtime.output.resamplingFactor_value);

			}
		}
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeViewer::prepare()
{
	jvxErrorType res = _prepare_lock();
	if(res == JVX_NO_ERROR)
	{
		if(external.circbuffer_input)
		{
			external.circbuffer_input->safe_access.allowRemove = false;
		}
		_unlock_state();
	}
	return(res);
}

jvxErrorType
CjvxAudioNodeViewer::postprocess()
{
	jvxErrorType res = _postprocess_lock();
	if(res == JVX_NO_ERROR)
	{
		if(external.circbuffer_input)
		{
			external.circbuffer_input->safe_access.allowRemove = true;
		}
		_unlock_state();
	}
	return(res);
}

jvxErrorType
CjvxAudioNodeViewer::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;

	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);

	res = process_buffer(
		theData->con_data.buffers[idx_sender_to_receiver],
		theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
		theData->con_params.buffersize,
		theData->con_params.number_channels,
		theData->con_compat.number_channels,
		theData->con_params.format);

	return(res);
};

jvxErrorType
CjvxAudioNodeViewer::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
		jvxSize numChans_input,jvxSize numChans_output,jvxDataFormat format)
{
	jvxSize ii, j;

	jvxData* ptrInDbl = NULL, *ptrOutDbl = NULL;
	jvxData* ptrInFlt = NULL, *ptrOutFlt = NULL;
	jvxInt64* ptrInInt64 = NULL, *ptrOutInt64 = NULL;
	jvxInt32* ptrInInt32 = NULL, *ptrOutInt32 = NULL;
	jvxInt16* ptrInInt16 = NULL, *ptrOutInt16 = NULL;
	jvxInt8* ptrInInt8 = NULL, *ptrOutInt8 = NULL;
	jvxData tmp;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize maxChans = JVX_MAX(numChans_input, numChans_output);

	if(
		(numChans_input > 0) &&
		(numChans_output > 0))
	{
		for(ii = 0; ii < maxChans; ii++)
		{
			jvxSize idxIn = ii % numChans_input;
			jvxSize idxOut = ii % numChans_output;

			switch(format)
			{
			case JVX_DATAFORMAT_DATA:
				ptrInDbl = (jvxData*)buffers_input[idxIn];
				ptrOutDbl = (jvxData*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInDbl[j];
					tmp *= genViewer_node::properties_active_higher.volume.value;
					ptrOutDbl[j] = tmp;
				}
				break;

			case JVX_DATAFORMAT_64BIT_LE:
				ptrInInt64 = (jvxInt64*)buffers_input[idxIn];
				ptrOutInt64 = (jvxInt64*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = (jvxData)ptrInInt64[j];
					tmp *= genViewer_node::properties_active_higher.volume.value;
					ptrOutInt64[j] = (jvxInt64)tmp;
				}
				break;

			case JVX_DATAFORMAT_32BIT_LE:
				ptrInInt32 = (jvxInt32*)buffers_input[idxIn];
				ptrOutInt32 = (jvxInt32*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt32[j];
					tmp *= genViewer_node::properties_active_higher.volume.value;
					ptrOutInt32[j] = (jvxInt32)tmp;
				}
				break;

			case JVX_DATAFORMAT_16BIT_LE:
				ptrInInt16 = (jvxInt16*)buffers_input[idxIn];
				ptrOutInt16 = (jvxInt16*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt16[j];
					tmp *= genViewer_node::properties_active_higher.volume.value;
					ptrOutInt16[j] = (jvxInt16)tmp;
				}
				break;
			case JVX_DATAFORMAT_8BIT:
				ptrInInt8 = (jvxInt8*)buffers_input[idxIn];
				ptrOutInt8 = (jvxInt8*)buffers_output[idxOut];
				for(j = 0; j < bSize; j++)
				{
					tmp = ptrInInt8[j];
					tmp *= genViewer_node::properties_active_higher.volume.value;
					ptrOutInt8[j] = (jvxInt8)tmp;
				}
				break;
			default:
				assert(0);
			}
		}
	}
	for(ii = 0; ii < genViewer_node::properties_active_higher.numberdummyloop.value; ii++)
	{
		int a = 0;
		// Do nothing
	}

	jvxBool storeInBuffer = false;
	jvxSize idChannel = JVX_SIZE_UNSELECTED;
	jvxData tmpOut_min, tmpOut_max;
	jvxCBool outputProduced;
	jvxData t0 = 0;
	jvxData t1 = 0;
	if(genViewer_node::properties_active_higher.inputChannelSelection.value.selection() != 0)
	{
		idChannel = jvx_bitfieldSelection2Id(
			genViewer_node::properties_active_higher.inputChannelSelection.value.selection(),
			genViewer_node::properties_active_higher.inputChannelSelection.value.entries.size());
		if(JVX_CHECK_SIZE_SELECTED(idChannel))
		{
			if(idChannel < numChans_input)
			{
				if(external.circbuffer_input)
				{
					if(external.circbuffer_input->number_channels >= 2)
					{
						storeInBuffer = true;
					}
				}
			}
		}
	}

	if(storeInBuffer)
	{
		// If new rate was setup for plot resmpler, activate resampler module
		jvxSize id = JVX_SIZE_UNSELECTED;
		id = jvx_bitfieldSelection2Id(
					genViewer_node::properties_active_higher.resamplingFactor.value.selection(),
					genViewer_node::properties_active_higher.resamplingFactor.value.entries.size());
		if(JVX_CHECK_SIZE_SELECTED(id))
		{
			if(id != runtime.input.resamplingFactor_id)
			{
				for(ii = 0; ii < _common_set_node_params_1io.processing.number_input_channels; ii++)
				{
					runtime.input.resamplingFactor_id = id;
					runtime.input.resamplingFactor_time = resampleTable[runtime.input.resamplingFactor_id];
					runtime.input.resamplingFactor_value = 1.0;
					if(external.circbuffer_input)
					{
						jvxData rate = external.circbuffer_input->length / (2.0 * runtime.input.resamplingFactor_time);
						runtime.input.resamplingFactor_value = (jvxData) _common_set_node_params_1io.processing.samplerate / rate;
					}

					jvx_resampler_plotter_reconfigure(runtime.input.resamplers[ii], runtime.input.resamplingFactor_value);
				}
			}
		}

		// Lock access to circular buffer for synchronization
		external.circbuffer_input->safe_access.lockf(external.circbuffer_input->safe_access.priv);
		jvxSize idxWrite = external.circbuffer_input->idx_read + external.circbuffer_input->fill_height;
		jvxSize numCopyMax = external.circbuffer_input->length - external.circbuffer_input->fill_height;
		external.circbuffer_input->safe_access.unlockf(external.circbuffer_input->safe_access.priv);
		jvxSize numWritten = 0;

		idxWrite = idxWrite % external.circbuffer_input->length;

		// Fields
		jvxData* fldOutStartX =  (jvxData*)(external.circbuffer_input->ptrFld);
		fldOutStartX += 0;
		jvxData* fldOutStartY =  (jvxData*)(external.circbuffer_input ->ptrFld);
		fldOutStartY += external.circbuffer_input->length;

		switch(format)
		{
		case JVX_DATAFORMAT_DATA:
			ptrInDbl = (jvxData*)buffers_input[idChannel];
			for(j = 0; j < bSize; j++)
			{
				tmp = ptrInDbl[j];
				jvx_resampler_plotter_process(tmp, &tmpOut_min, &tmpOut_max, &outputProduced, runtime.input.resamplers[idChannel]);
				if(outputProduced)
				{
					if(numWritten + 2 <= numCopyMax)
					{
						fldOutStartX[idxWrite] = runtime.input.t;
						fldOutStartY[idxWrite] = tmpOut_min;
						idxWrite = (idxWrite + 1 )% external.circbuffer_input->length;

						fldOutStartX[idxWrite] = runtime.input.t;
						fldOutStartY[idxWrite] = tmpOut_max;
						idxWrite = (idxWrite + 1 )% external.circbuffer_input->length;
						numWritten += 2;
					}
					else
					{
						// Lost a sample
					}
				}
				runtime.input.t += runtime.input.deltaT;
			}

			break;

		case JVX_DATAFORMAT_64BIT_LE:
		case JVX_DATAFORMAT_32BIT_LE:
		case JVX_DATAFORMAT_16BIT_LE:
		case JVX_DATAFORMAT_8BIT:
			assert(0);
			break;
		default:
			assert(0);
		}


		external.circbuffer_input->safe_access.lockf(external.circbuffer_input->safe_access.priv);
		external.circbuffer_input->fill_height += numWritten;
		external.circbuffer_input->safe_access.unlockf(external.circbuffer_input->safe_access.priv);
	}
	else
	{
		for(j = 0; j < bSize; j++)
		{
			runtime.input.t += runtime.input.deltaT;
		}
	}
	return(res);
}

jvxErrorType
CjvxAudioNodeViewer::prefill_buffers_input()
{
	jvxSize j;
	jvxData tmp;
	// Lock access to circular buffer for synchronization
	external.circbuffer_input->safe_access.lockf(external.circbuffer_input->safe_access.priv);
	jvxSize idxWrite = external.circbuffer_input->idx_read + external.circbuffer_input->fill_height;
	jvxSize numCopyMax = external.circbuffer_input->length - external.circbuffer_input->fill_height;
	external.circbuffer_input->safe_access.unlockf(external.circbuffer_input->safe_access.priv);
	jvxSize numWritten = 0;

	idxWrite = idxWrite % external.circbuffer_input->length;

	// Fields
	jvxData* fldOutStartX =  (jvxData*)(external.circbuffer_input->ptrFld);
	fldOutStartX += 0;
	jvxData* fldOutStartY =  (jvxData*)(external.circbuffer_input->ptrFld);
	fldOutStartY += external.circbuffer_input->length;

	for(j = 0; j < numCopyMax; j++)
	{
		tmp = -runtime.input.deltaT/2 * numCopyMax + runtime.input.t;
		fldOutStartX[idxWrite] = runtime.input.t;
		fldOutStartY[idxWrite] = 0;
		idxWrite = (idxWrite + 1 )% external.circbuffer_input->length;
	}


	external.circbuffer_input->safe_access.lockf(external.circbuffer_input->safe_access.priv);
	external.circbuffer_input->fill_height += numCopyMax;
	external.circbuffer_input->safe_access.unlockf(external.circbuffer_input->safe_access.priv);
	return(JVX_NO_ERROR);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioNodeViewer, set_buffer_post)
{
	if (
		(genViewer_node::properties_active_higher.inputStreamPlot.globalIdx == ident.id) &&
		(genViewer_node::properties_active_higher.inputStreamPlot.category == ident.cat))
	{
		if (genViewer_node::properties_active_higher.inputStreamPlot.ptr)
		{
			_lock_state();
			if (_common_set_min.theState >= JVX_STATE_PREPARED)
			{
				genViewer_node::properties_active_higher.inputStreamPlot.ptr->safe_access.allowRemove = false;
			}
			_unlock_state();
		}
	}

	if (
		(genViewer_node::properties_active_higher.outputStreamPlot.globalIdx == ident.id) &&
		(genViewer_node::properties_active_higher.outputStreamPlot.category == ident.cat))
	{
		if (genViewer_node::properties_active_higher.outputStreamPlot.ptr)
		{
			_lock_state();
			if (_common_set_min.theState >= JVX_STATE_PREPARED)
			{
				genViewer_node::properties_active_higher.outputStreamPlot.ptr->safe_access.allowRemove = false;
			}
			_unlock_state();
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioNodeViewer, set_buffer_pre)
{
	if (
		(genViewer_node::properties_active_higher.inputStreamPlot.globalIdx == ident.id) &&
		(genViewer_node::properties_active_higher.inputStreamPlot.category == ident.cat))
	{
		if (genViewer_node::properties_active_higher.inputStreamPlot.ptr)
		{
			_lock_state();
			if (_common_set_min.theState >= JVX_STATE_PREPARED)
			{
				genViewer_node::properties_active_higher.inputStreamPlot.ptr->safe_access.allowRemove = true;
			}
			_unlock_state();
		}
	}

	if (
		(genViewer_node::properties_active_higher.outputStreamPlot.globalIdx == ident.id) &&
		(genViewer_node::properties_active_higher.outputStreamPlot.category == ident.cat))
	{
		if (genViewer_node::properties_active_higher.outputStreamPlot.ptr)
		{
			_lock_state();
			if (_common_set_min.theState >= JVX_STATE_PREPARED)
			{
				genViewer_node::properties_active_higher.outputStreamPlot.ptr->safe_access.allowRemove = true;
			}
			_unlock_state();
		}
	}
	return JVX_NO_ERROR;
}

