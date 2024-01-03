#include "CjvxAuNCaptureFile.h"

CjvxAuNCaptureFile::CjvxAuNCaptureFile(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
		
	neg_input._update_parameters_fixed(JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED,
		JVX_SIZE_UNSELECTED, JVX_DATAFORMAT_DATA, JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);
}

CjvxAuNCaptureFile::~CjvxAuNCaptureFile()
{
	
}

// ===================================================================

jvxErrorType 
CjvxAuNCaptureFile::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1io::select(owner);
	if (res == JVX_NO_ERROR)
	{
		// Need to allocate properties on selected to allow modification of prefix
		genCaptureFile_node::init_all();
		genCaptureFile_node::allocate_all();
		genCaptureFile_node::register_all(static_cast<CjvxProperties*>(this));
		genCaptureFile_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_processing_props, get_processing_monitor,
			reinterpret_cast<jvxHandle*>(this), NULL);
	}
	return res;
}

jvxErrorType 
CjvxAuNCaptureFile::unselect()
{
	jvxErrorType res = CjvxBareNode1io::unselect();
	if (res == JVX_NO_ERROR)
	{
		genCaptureFile_node::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		genCaptureFile_node::unregister_all(static_cast<CjvxProperties*>(this));
		genCaptureFile_node::deallocate_all();
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNCaptureFile::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{
		wavFileResolution = genCaptureFile_node::translate__capture__wav_file_resolution_from();
		wavFileType = genCaptureFile_node::translate__capture__wav_file_type_from();

		capture_output.initialize(_common_set.theToolsHost, _common_set_min.theHostRef);
		capture_output.set_fname_prefix(genCaptureFile_node::capture.prefix.value);
		capture_output.set_wav_props(wavFileResolution, wavFileType, genCaptureFile_node::capture.num_buffers.value);
	}
	return res;
}

jvxErrorType
CjvxAuNCaptureFile::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::deactivate();
	if (res == JVX_NO_ERROR)
	{
		capture_output.terminate();
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNCaptureFile::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNCaptureFile::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		genCaptureFile_node::monitor.buffer_drops.value = 0;
		capture_output.set_parameters(
			_common_set_icon.theData_in->con_params.buffersize,
			_common_set_icon.theData_in->con_params.rate,
			_common_set_icon.theData_in->con_params.number_channels,
			_common_set_icon.theData_in->con_params.format);
	}
	return res;
}

jvxErrorType
CjvxAuNCaptureFile::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		capture_output.reset_parameters();

	}
	return res;
}

jvxErrorType
CjvxAuNCaptureFile::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle** in_bufs = NULL;
	jvxSize idx_stage_local = idx_stage;
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_local))
		idx_stage_local = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
	in_bufs = _common_set_icon.theData_in->con_data.buffers[idx_stage_local];

	res = capture_output.write_data(in_bufs);
	if (res != JVX_NO_ERROR)
	{
		if (res != JVX_ERROR_NOT_READY)
		{
			genCaptureFile_node::monitor.buffer_drops.value++;
		}
	}
	
	// Forward the processed data to next chain element
	res = CjvxBareNode1io::process_buffers_icon(mt_mask, idx_stage);
	return res;
}


// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNCaptureFile::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1io::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genCaptureFile_node::put_configuration_all(callMan,
				processor, sectionToContainAllSubsectionsForMe,
				&warns);
			for (i = 0; i < warns.size(); i++)
			{
				std::cout << __FUNCTION__ << ": Warning: " << warns[i] << std::endl;
			}

			capture_output.set_fname_prefix(genCaptureFile_node::capture.prefix.value);
			if (genCaptureFile_node::capture.toggle.value == c_true)
			{
				capture_output.trigger_start();
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuNCaptureFile::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1io::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genCaptureFile_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNCaptureFile, set_processing_props)
{
	if (ident.id == genCaptureFile_node::capture.prefix.globalIdx)
	{
		capture_output.set_fname_prefix(genCaptureFile_node::capture.prefix.value);
	}
	else if (ident.id == genCaptureFile_node::capture.toggle.globalIdx)
	{
		if (genCaptureFile_node::capture.toggle.value == c_true)
		{
			capture_output.trigger_start();
		}
		else
		{
			capture_output.trigger_stop();
		}
	}
	else if (ident.id == genCaptureFile_node::capture.wav_file_resolution.globalIdx)
	{
		wavFileResolution = genCaptureFile_node::translate__capture__wav_file_resolution_from();
	}
	else if (ident.id == genCaptureFile_node::capture.wav_file_type.globalIdx)
	{
		wavFileType = genCaptureFile_node::translate__capture__wav_file_type_from();
	}
	else if (ident.id == genCaptureFile_node::capture.num_buffers.globalIdx)
	{
		num_bufs = genCaptureFile_node::capture.num_buffers.value;
	}
	
	capture_output.set_wav_props(wavFileResolution, wavFileType, num_bufs);

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNCaptureFile, get_processing_monitor)
{
	return JVX_NO_ERROR;
}