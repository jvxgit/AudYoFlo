#include "CjvxAudioCoreAudioDevice.h"
#include "jvx_config.h"


// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

CjvxAudioCoreAudioDevice::CjvxAudioCoreAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);

	runtime.usefulBSizes.clear();
	jvxSize cnt = 0;
	while (1)
	{
		int oneBSize = jvxUsefulBSizes[cnt++];
		if (oneBSize > 0)
		{
			runtime.usefulBSizes.push_back(oneBSize);
		}
		else
		{
			break;
		}
	}
	this->clearPropertiesInit();
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

CjvxAudioCoreAudioDevice::~CjvxAudioCoreAudioDevice()
{
}

jvxErrorType
CjvxAudioCoreAudioDevice::initialize(std::string description, jvxSize idx, AudioDeviceID hdlDevice)
{
	runtime.deviceProps.description = description;
	runtime.deviceProps.idx = idx;
	runtime.deviceProps.hdlDevice = hdlDevice;
	return(JVX_NO_ERROR);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = CjvxAudioDevice::select(theOwner);
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::activate()
{
	jvxSize i,j;

	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genCoreAudio_device::init__properties_active();
		genCoreAudio_device::allocate__properties_active();
		genCoreAudio_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genCoreAudio_device::init__properties_active_higher();
		genCoreAudio_device::allocate__properties_active_higher();
		genCoreAudio_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));
		genCoreAudio_device::properties_active_higher.loadpercent.valid = false;

		res = activateCoreAudioProperties();

		if (res == JVX_NO_ERROR)
		{
			CjvxAudioDevice::updateDependentVariables_lock(1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
			updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);
		}
		else
		{
			jvxErrorType res2 = this->deactivate();
			assert(res2 == JVX_NO_ERROR);
		}

		_report_property_has_changed(JVX_PROPERTY_CATEGORY_UNKNOWN, -1, false, 0, 1, JVX_COMPONENT_UNKNOWN);
	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genCoreAudio_device::properties_active.ratesselection.cat, genCoreAudio_device::properties_active.ratesselection.id);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genCoreAudio_device::properties_active.sizesselection.cat, genCoreAudio_device::properties_active.sizesselection.id);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genCoreAudio_device::properties_active.controlThreads.cat, genCoreAudio_device::properties_active.controlThreads.id);

		CjvxProperties::_unlock_properties_local();

	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	//jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
	
        CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(genCoreAudio_device::properties_active.ratesselection.cat, genCoreAudio_device::properties_active.ratesselection.id);
		CjvxProperties::_undo_update_property_access_type(genCoreAudio_device::properties_active.sizesselection.cat, genCoreAudio_device::properties_active.sizesselection.id);
		CjvxProperties::_undo_update_property_access_type(genCoreAudio_device::properties_active.controlThreads.cat, genCoreAudio_device::properties_active.controlThreads.id);

		CjvxProperties::_unlock_properties_local();
	}
	return (res);
}


jvxErrorType
CjvxAudioCoreAudioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;
    

    // Prepare link data datatype
    _common_set_ldslave.theData_out.con_compat.buffersize = _inproc.buffersize;
    _common_set_ldslave.theData_out.con_params.buffersize = _inproc.buffersize;
    _common_set_ldslave.theData_out.con_params.format = _inproc.format;
    _common_set_ldslave.theData_out.con_data.buffers = NULL;
    _common_set_ldslave.theData_out.con_data.number_buffers = 1;
    _common_set_ldslave.theData_out.con_params.number_channels = _inproc.numInputs;
    _common_set_ldslave.theData_out.con_params.rate = CjvxAudioDevice::properties_active.samplerate.value;

    // New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
    if(res != JVX_NO_ERROR)
    {
	goto leave_error;
    }

    // =========================================================================
    // Start also Core Audio
    // =========================================================================
    
    res = prepareCoreAudio();
    if(res != JVX_NO_ERROR)
    {
	_postprocess_chain_master(NULL);
	goto leave_error;
    }

    JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
    inProcessing.timestamp_previous = -1;
    inProcessing.deltaT_theory_us = (jvxData) _common_set_ldslave.theData_out.con_params.buffersize /
	(jvxData) _common_set_ldslave.theData_out.con_params.rate * 1000.0 * 1000.0;
    inProcessing.deltaT_average_us = 0;

    return(res);
leave_error:
		
    return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
  jvxErrorType res = JVX_NO_ERROR;

  res = postprocessCoreAudio();
  assert(res == JVX_NO_ERROR);
 
  _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
  assert(res == JVX_NO_ERROR);
  
  return res;

}

jvxErrorType
CjvxAudioCoreAudioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}
	
	genCoreAudio_device::properties_active_higher.loadpercent.value = 0;
	genCoreAudio_device::properties_active_higher.loadpercent.valid = true;

	res = res = startCoreAudio();
	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}
	genCoreAudio_device::properties_active_higher.loadpercent.value = 0;
	genCoreAudio_device::properties_active_higher.loadpercent.valid = true;

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = stopCoreAudio();
	genCoreAudio_device::properties_active_higher.loadpercent.value = 0;
	genCoreAudio_device::properties_active_higher.loadpercent.valid = false;

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

	return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxAudioCoreAudioDevice::process_start_icon(
					jvxSize pipeline_offset, jvxSize* idx_stage,
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
CjvxAudioCoreAudioDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop( idx_stage,  shift_fwd,
	    tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

jvxErrorType 
CjvxAudioCoreAudioDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxAudioDevice::ext_transfer_backward_ocon(tp, data, &genCoreAudio_device::properties_active.ratesselection, &genCoreAudio_device::properties_active.sizesselection JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}


// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::deactivate();
	if (res == JVX_NO_ERROR)
	{
		this->deactivateCoreAudioProperties();

		updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genCoreAudio_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genCoreAudio_device::deallocate__properties_active_higher();

		genCoreAudio_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genCoreAudio_device::deallocate__properties_active();
	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxAudioCoreAudioDevice::set_property(jvxCallManagerProperties* callGate,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, 
		jvxSize propId, jvxPropertyCategoryType category, 
		jvxSize offsetStart, jvxBool contentOnly)
{
	jvxSize i;
	jvxBool report_update = false;
    Boolean locRes = false;
	jvxErrorType res = CjvxAudioDevice::set_property(callGate, 
							 fld, numElements, format, propId, 
							 category, offsetStart, contentOnly);

    jvxErrorType resLocal = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
        if(
           (category == genCoreAudio_device::properties_active.ratesselection.cat)&&
           (propId == genCoreAudio_device::properties_active.ratesselection.id))
        {
            UInt32 newVal = -1;
            for(i = 0; i < genCoreAudio_device::properties_active.ratesselection.value.entries.size(); i++)
            {
	      if(jvx_bitTest(genCoreAudio_device::properties_active.ratesselection.value.selection, i))
                {
                    newVal = this->runtime.samplerates[i];
                    break;
                }
            }
            assert(newVal > 0);
            resLocal = this->setNominalSamplerate((Float64)newVal);
            assert(resLocal == JVX_NO_ERROR);
        }



        if(
           (category == genCoreAudio_device::properties_active.sizesselection.cat)&&
           (propId == genCoreAudio_device::properties_active.sizesselection.id))
        {
            UInt32 newVal = -1;
            for(i = 0; i < genCoreAudio_device::properties_active.sizesselection.value.entries.size(); i++)
            {
	      if(jvx_bitTest(genCoreAudio_device::properties_active.sizesselection.value.selection, i))
                {
                    newVal = this->runtime.sizesBuffers[i];
                    break;
                }
            }
            assert(newVal > 0);
            resLocal = this->setDeviceFramesize(newVal);
            assert(resLocal == JVX_NO_ERROR);
        }

		updateDependentVariables(propId, category, false, callGate->call_purpose);
	}
	return (res);
}


jvxErrorType
CjvxAudioCoreAudioDevice::put_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);

	if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		genCoreAudio_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, warnings);
		if (!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for (i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}

			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}

		this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

	}
	return (res);
}

jvxErrorType
CjvxAudioCoreAudioDevice::get_configuration(jvxCallManagerConfiguration* callConf, 
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
// Write all parameters from base class
	CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

// Write all parameters from this class
	genCoreAudio_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

void
CjvxAudioCoreAudioDevice::updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxPropertyCategoryType catTp;
	jvxInt32 newValue = 0;

// Update the variables in base class
//CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll); <- this happens already in the base class

	if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if ((propId == genCoreAudio_device::properties_active.ratesselection.id) || (propId == CjvxAudioDevice::properties_active.samplerate.id) || updateAll)
		{
			for (i = 0; i < (int) genCoreAudio_device::properties_active.ratesselection.value.entries.size(); i++)
			{
			  if (jvx_bitTest(genCoreAudio_device::properties_active.ratesselection.value.selection, i))
				{
					newValue = runtime.samplerates[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.samplerate.value = newValue;

			_report_property_has_changed(JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.samplerate.id, true,0,1,
				JVX_COMPONENT_UNKNOWN, callPurp);
			_report_property_has_changed(JVX_PROPERTY_CATEGORY_PREDEFINED, genCoreAudio_device::properties_active.ratesselection.id, true, 0,1,
				JVX_COMPONENT_UNKNOWN, callPurp);
		}

		// =================================================================================================
		if ((propId == genCoreAudio_device::properties_active.sizesselection.id) || (propId == CjvxAudioDevice::properties_active.buffersize.id) || updateAll)
		{
			for (i = 0; i < (int) genCoreAudio_device::properties_active.sizesselection.value.entries.size(); i++)
			{
			  if (jvx_bitTest(genCoreAudio_device::properties_active.sizesselection.value.selection, i))
				{
					newValue = runtime.sizesBuffers[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.buffersize.value = newValue;

			this->_report_property_has_changed(JVX_PROPERTY_CATEGORY_PREDEFINED, genCoreAudio_device::properties_active.sizesselection.id, true, 0, 1,
				JVX_COMPONENT_UNKNOWN, callPurp);
			_report_property_has_changed(JVX_PROPERTY_CATEGORY_PREDEFINED, CjvxAudioDevice::properties_active.buffersize.id, true, 0, 1,
				JVX_COMPONENT_UNKNOWN, callPurp);

		}
	}
}



