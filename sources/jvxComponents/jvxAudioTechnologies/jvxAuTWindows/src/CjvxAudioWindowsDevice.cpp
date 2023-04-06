#include "CjvxAudioWindowsDevice.h"
#include "CjvxAudioWindowsTechnology.h"

CjvxAudioWindowsDevice::CjvxAudioWindowsDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);
}


CjvxAudioWindowsDevice::~CjvxAudioWindowsDevice()
{
	if (parentTech)
	{
		parentTech->deactivate_windows_audio_device(device);
		device = nullptr;
	}
}

jvxErrorType
CjvxAudioWindowsDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genWindows_device::init__properties_selected();
		genWindows_device::allocate__properties_selected();
		genWindows_device::register__properties_selected(static_cast<CjvxProperties*>(this));
		_common_set_device.caps.selectable = false;

		if (isInput)
		{
			audioMode = parentTech->translate__properties_selected__device_mode_input_from();
		}
		else
		{
			audioMode = parentTech->translate__properties_selected__device_mode_output_from();
		}
		genWindows_device::translate__properties_selected__device_mode_to(audioMode);

		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}


jvxErrorType
CjvxAudioWindowsDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
		genWindows_device::unregister__properties_selected(static_cast<CjvxProperties*>(this));
		genWindows_device::deallocate__properties_selected();
		_common_set_device.caps.selectable = true;
		if (_common_set_device.report)
		{
			_common_set_device.report->on_device_caps_changed(static_cast<IjvxDevice*>(this));
		}
	}
	return (res);
}



jvxErrorType
CjvxAudioWindowsDevice::activate()
{
	jvxSize i,j;

	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genWindows_device::init__properties_active();
		genWindows_device::allocate__properties_active();
		genWindows_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genWindows_device::init__properties_active_higher();
		genWindows_device::allocate__properties_active_higher();
		genWindows_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));
		genWindows_device::properties_active_higher.loadpercent.isValid = false;

		genWindows_device::register_callbacks(static_cast<CjvxProperties*>(this),
			set_new_rate, reinterpret_cast<jvxHandle*>(this), nullptr);

		// Start with 2 buffers on inputand 3 buffers on output side
		genWindows_device::properties_active.numBuffersFlushShared.value = 2;
		if(!isInput)
		{ 
			genWindows_device::properties_active.numBuffersFlushShared.value = 3;
		}
		// CjvxAudioDevice_genpcg::properties_active
		activate_wasapi_device();

		// Do not request chain test since we are in ctivate call
		updateDependentParameters(false);		
    }
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioWindowsDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{        
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genWindows_device::properties_active.ratesselection);

		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genAlsa_device::properties_active.accessType.cat, genAlsa_device::properties_active.accessType.id);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genAlsa_device::properties_active.formatType.cat, genAlsa_device::properties_active.accessType.id);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genAlsa_device::properties_active.periodsInput.cat, genAlsa_device::properties_active.accessType.id);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genAlsa_device::properties_active.periodsOutput.cat, genAlsa_device::properties_active.accessType.id);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genAlsa_device::properties_active.controlThreads.cat, genAlsa_device::properties_active.controlThreads.id);
	}
	return (res);
}


jvxErrorType
CjvxAudioWindowsDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_undo_update_property_access_type(genWindows_device::properties_active.ratesselection);
		/*
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.sizesselection.cat, genAlsa_device::properties_active.sizesselection.id);
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.accessType.cat, genAlsa_device::properties_active.accessType.id);
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.formatType.cat, genAlsa_device::properties_active.formatType.id);
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.periodsInput.cat, genAlsa_device::properties_active.accessType.id);
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.periodsOutput.cat, genAlsa_device::properties_active.accessType.id);
		CjvxProperties::_undo_update_property_access_type(genAlsa_device::properties_active.controlThreads.cat, genAlsa_device::properties_active.controlThreads.id);
		*/
	}
	return (res);
}

jvxErrorType
CjvxAudioWindowsDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		res = deactivate_wasapi_device();
		assert(res == JVX_NO_ERROR);

		genWindows_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genWindows_device::deallocate__properties_active_higher();

		genWindows_device::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);

		genWindows_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genWindows_device::deallocate__properties_active();

		res = CjvxAudioDevice::deactivate();
	}
	return (res);
}

jvxErrorType
CjvxAudioWindowsDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
    jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);

	if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		genWindows_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = __FUNCTION__;
				txt += ": Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}
		}
        this->updateDependentParameters(true);

	}
	return (res);
}

jvxErrorType
CjvxAudioWindowsDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

// Write all parameters from this class
     genWindows_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

void
CjvxAudioWindowsDevice::updateDependentParameters(jvxBool inform_chain)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxPropertyCategoryType catTp;
	jvxInt32 newValue = 0;
    jvxBool reportUpdate = false;
	
	auto elm = supportedRates.begin();
	for (i = 0; i < (int)genWindows_device::properties_active.ratesselection.value.entries.size(); i++, elm++)
	{
		if (jvx_bitTest(genWindows_device::properties_active.ratesselection.value.selection(), i))
		{
			newValue = *elm;
			break;
		}
	}
		
	CjvxAudioDevice::properties_active.samplerate.value = newValue;

	if (inform_chain)
	{
		if (_common_set_ld_master.refProc)
		{
			jvxSize uId = JVX_SIZE_UNSELECTED;
			_common_set_ld_master.refProc->unique_id_connections(&uId);
			this->_request_test_chain_master(uId);
		}
	}
}

jvxErrorType
CjvxAudioWindowsDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;
    
	// If the chain is prepared, the object itself should also be prepared if not done so before
	jvxErrorType resL = prepare_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

    // Prepare link data datatype
    _common_set_ldslave.theData_out.con_compat.buffersize = _inproc.buffersize;
    _common_set_ldslave.theData_out.con_params.buffersize = _inproc.buffersize;
    _common_set_ldslave.theData_out.con_params.format = _inproc.format;
    _common_set_ldslave.theData_out.con_data.buffers = NULL;
    _common_set_ldslave.theData_out.con_data.number_buffers = 1;
    _common_set_ldslave.theData_out.con_params.number_channels = _inproc.numInputs;
    _common_set_ldslave.theData_out.con_params.rate = CjvxAudioDevice::properties_active.samplerate.value;
    //_common_set_ldslave.theData_out.pipeline.idx_stage = 0;

    // New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
    if(res != JVX_NO_ERROR)
    {
	goto leave_error;
    }

    // =========================================================================
    // Prepare core function
    // =========================================================================
	res = prepare_wasapi_device();
   
	if(res != JVX_NO_ERROR)
    {
		_postprocess_chain_master(NULL);
		goto leave_error;
    }

    /*JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
    inProcessing.timestamp_previous = -1;
    inProcessing.deltaT_theory_us = (jvxData) _common_set_ldslave.theData_out.con_params.buffersize /
	(jvxData) _common_set_ldslave.theData_out.con_params.rate * 1000.0 * 1000.0;
    inProcessing.deltaT_average_us = 0;
	*/
    return(res);
leave_error:
		
    return res;
}

jvxErrorType
CjvxAudioWindowsDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
  jvxErrorType res = JVX_NO_ERROR;
  
  res = postprocess_wasapi_device();
  if (res != JVX_NO_ERROR)
  {
	  return res;
  }

  res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
  assert(res == JVX_NO_ERROR);
  
  // If the chain is postprocessed, the object itself should also be postprocessed if not done so before
  jvxErrorType resL = postprocess_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
  assert(resL == JVX_NO_ERROR);

  return res;

}

jvxErrorType
CjvxAudioWindowsDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// If the chain is started, the object itself should also be started if not done so before
	jvxErrorType resL = start_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}

	/*
	genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = true;
	*/

	res = start_wasapi_device();

	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}

	/*
	genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = true;
	*/

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = stop_wasapi_device();

    /*
	genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = false;

	*/
	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	// If the chain is stopped, the object itself should also be stopped if not done so before
	jvxErrorType resL = stop_chain_master_autostate(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	assert(resL == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_ldslave.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_ldslave.theData_in->con_params.format = _inproc.format;
	_common_set_ldslave.theData_in->con_data.buffers = NULL;
	_common_set_ldslave.theData_in->con_data.number_buffers = JVX_MAX(_common_set_ldslave.theData_in->con_data.number_buffers, 1);
	_common_set_ldslave.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_ldslave.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_ldslave.theData_in->con_compat.user_hints = NULL;
	// _common_set_ldslave.theData_in->pipeline.idx_stage = 0;

	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
					jvxSize tobeAccessedByStage,
					callback_process_start_in_lock clbk,
					jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage,
	    tobeAccessedByStage,
	    clbk, priv_ptr);
	return res;
}

jvxErrorType
CjvxAudioWindowsDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
	    tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

jvxErrorType 
CjvxAudioWindowsDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS)
	{
		res = CjvxAudioDevice::transfer_backward_ocon_match_setting(tp, data,
			&genWindows_device::properties_active.ratesselection,
			nullptr
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			return res;
		}
	}
	return  CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType 
CjvxAudioWindowsDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	return CjvxAudioDevice::set_property(callGate,
		rawPtr, ident, trans);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAudioWindowsDevice, set_new_rate)
{
	if (callGate.call_purpose != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
	{
		updateDependentParameters(true);
	}
	return JVX_NO_ERROR;
}