
#include "CjvxIosAudioDevice.h"
#include "jvx_config.h"
#include "jvxAudioProcessor_cpp.h"
#include "CjvxIosAudioTechnology.h"

void process_buffer(void* priv)
{
    CjvxIosAudioDevice* this_is_me = reinterpret_cast<CjvxIosAudioDevice*>(priv);
    if(this_is_me)
    {
        this_is_me->processBuffer();
    }
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

CjvxIosAudioDevice::CjvxIosAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));

	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

CjvxIosAudioDevice::~CjvxIosAudioDevice()
{
}

jvxErrorType
CjvxIosAudioDevice::init(jvxSize idPort, CjvxIosAudioTechnology* par)
{
    device.inputPortId = idPort;
    device.theParent = par;
    return(JVX_NO_ERROR);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxIosAudioDevice::select(IjvxObject* theOwner)
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
CjvxIosAudioDevice::unselect()
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
CjvxIosAudioDevice::activate()
{
	jvxSize i,j;
    jvxSize num = 0;
	jvxErrorType res = CjvxAudioDevice::activate();
    std::string tt;
    jvxSize bsize;
    jvxInt32 srate;
    jvxDataFormat form;
    std::string nm;
    jvxBool isActive = false;
    jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

	if (res == JVX_NO_ERROR)
	{
		genIosAudio_device::init__properties_active();
		genIosAudio_device::allocate__properties_active();
		genIosAudio_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genIosAudio_device::init__properties_active_higher();
		genIosAudio_device::allocate__properties_active_higher();
		genIosAudio_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));

        // Activate this device
        theProcHandle->initializeDevice(device.inputPortId);

        /*
         * Handle the found infos
         */

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
        CjvxAudioDevice::properties_active.inputchannelselection.value.selection = 0;
        CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;

        num = 0;

        res = theProcHandle->get_number_input_channels(&num);
        assert(res == JVX_NO_ERROR);

        jvx_bitFClear(CjvxAudioDevice::properties_active.inputchannelselection.value.selection);
        for(i = 0; i < num; i++)
        {
            theProcHandle->get_description_input_channel(i, tt);
            assert(res == JVX_NO_ERROR);
            CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(tt.c_str());
            jvx_bitSet(CjvxAudioDevice::properties_active.inputchannelselection.value.selection, i);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
        CjvxAudioDevice::properties_active.outputchannelselection.value.selection = 0;
        CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;

        num = 0;

        res = theProcHandle->get_number_output_channels(&num);
        assert(res == JVX_NO_ERROR);
        jvx_bitFClear(CjvxAudioDevice::properties_active.outputchannelselection.value.selection);
        for(i = 0; i < num; i++)
        {
            theProcHandle->get_description_output_channel(i, tt);
            assert(res == JVX_NO_ERROR);
            CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(tt.c_str());
            jvx_bitSet(CjvxAudioDevice::properties_active.outputchannelselection.value.selection, i);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        genIosAudio_device::properties_active.sizesselection.value.selection = 0;
        genIosAudio_device::properties_active.sizesselection.value.entries.clear();
        shortcuts.bsizes.clear();

        num = 0;

        res = theProcHandle->get_number_buffersizes(&num);
        assert(res == JVX_NO_ERROR);

        for(i = 0; i < num; i++)
        {
            theProcHandle->get_buffersize(i,bsize);
            assert(res == JVX_NO_ERROR);
            genIosAudio_device::properties_active.sizesselection.value.entries.push_back(jvx_size2String(bsize).c_str());
            shortcuts.bsizes.push_back((jvxInt32)bsize);
        }
        jvx_bitZSet(genIosAudio_device::properties_active.sizesselection.value.selection ,(num-1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        genIosAudio_device::properties_active.ratesselection.value.selection = 0;
        genIosAudio_device::properties_active.ratesselection.value.entries.clear();
        shortcuts.srates.clear();

        num = 0;

        res = theProcHandle->get_number_samplerates(&num);
        assert(res == JVX_NO_ERROR);

        for(i = 0; i < num; i++)
        {
            theProcHandle->get_samplerate(i, srate);
            assert(res == JVX_NO_ERROR);
            genIosAudio_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(srate).c_str());
            shortcuts.srates.push_back(srate);
        }
        jvx_bitZSet(genIosAudio_device::properties_active.sizesselection.value.selection,(num-1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        CjvxAudioDevice::properties_active.formatselection.value.entries.clear();
        CjvxAudioDevice::properties_active.formatselection.value.selection = 0;
        CjvxAudioDevice::properties_active.format.value = JVX_DATAFORMAT_NONE;
        shortcuts.formats.clear();
        num = 0;

        res = theProcHandle->get_number_dataformats(&num);
        assert(res == JVX_NO_ERROR);

        for(i = 0; i < num; i++)
        {
            theProcHandle->get_dataformat(i, form);
            assert(form > JVX_DATAFORMAT_NONE);
            CjvxAudioDevice::properties_active.formatselection.value.entries.push_back(jvxDataFormat_txt(form));
            if(i == 0)
            {
                CjvxAudioDevice::properties_active.format.value = (jvxInt16) form;
                CjvxAudioDevice::properties_active.formatselection.value.selection = ((jvxBitField)1 << 0);
                CjvxAudioDevice::properties_active.formatselection.value.exclusive = (jvxBitField) -1;
                shortcuts.formats.push_back(form);
            }
        }


        CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.entries.clear();
        CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.selection = 0;
        num = 0;

        res = theProcHandle->get_number_input_sources(&num);
        assert(res == JVX_NO_ERROR);
        jvx_bitFClear(CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.selection);
        for(i = 0; i < num; i++)
        {
            isActive = false;
            theProcHandle->get_input_source(i, nm, &isActive);
            CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.entries.push_back(nm);
            if(isActive)
            {
               jvx_bitSet(CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.selection, i);
            }
        }
        CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.entries.clear();
        CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.selection = 0;
        num = 0;

        res = theProcHandle->get_number_output_sources(&num);
        assert(res == JVX_NO_ERROR);
        jvx_bitFClear(CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.selection);
        for(i = 0; i < num; i++)
        {
            isActive = false;
            theProcHandle->get_output_source(i, nm, &isActive);
            CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.entries.push_back(nm);
            if(isActive)
            {
                jvx_bitSet(CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.selection, i);
            }
        }


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

		// _report_property_has_changed(JVX_PROPERTY_CATEGORY_UNKNOWN, -1, false,);
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
CjvxIosAudioDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;
    jvxSize idSRate = 0;
    jvxSize idBSize = 0;
    jvxSize idDFormat = 0;
    int checkVal = 0;
    jvxDataFormat checkForm = JVX_DATAFORMAT_NONE;
    jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genIosAudio_device::properties_active.ratesselection.cat, genIosAudio_device::properties_active.ratesselection.id);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genIosAudio_device::properties_active.sizesselection.cat, genIosAudio_device::properties_active.sizesselection.id);
		//CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, genIosAudio_device::properties_active.controlThreads.cat, genIosAudio_device::properties_active.controlThreads.id);

		CjvxProperties::_unlock_properties_local();

	}
	return (res);
}

jvxErrorType
CjvxIosAudioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

    /*
      res = prepareCoreAudio();
      if(res != JVX_NO_ERROR)
      {
      _postprocess_chain_master(NULL);
      goto leave_error;
      }
    */

    JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
    inProcessing.timestamp_previous = -1;
    inProcessing.deltaT_theory_us = (jvxData) _common_set_ldslave.theData_out.con_params.buffersize /
	(jvxData) _common_set_ldslave.theData_out.con_params.rate * 1000.0 * 1000.0;
    inProcessing.deltaT_average_us = 0;

    return(res);
leave_error:
		
    return res;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxIosAudioDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
  
	if (res == JVX_NO_ERROR)
	{

		CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(genIosAudio_device::properties_active.ratesselection.cat, genIosAudio_device::properties_active.ratesselection.id);
		CjvxProperties::_undo_update_property_access_type(genIosAudio_device::properties_active.sizesselection.cat, genIosAudio_device::properties_active.sizesselection.id);
		//CjvxProperties::_undo_update_property_access_type(genIosAudio_device::properties_active.controlThreads.cat, genIosAudio_device::properties_active.controlThreads.id);

		CjvxProperties::_unlock_properties_local();
	}
	return (res);
}

jvxErrorType
CjvxIosAudioDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
  jvxErrorType res = JVX_NO_ERROR;

  //res = postprocessCoreAudio();
  //assert(res == JVX_NO_ERROR);
 
  _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
  assert(res == JVX_NO_ERROR);
  
  return res;

}
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================



jvxErrorType
CjvxIosAudioDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
   jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}
	
	genIosAudio_device::properties_active_higher.loadpercent.value = 0;
	genIosAudio_device::properties_active_higher.loadpercent.valid = true;

	res = theProcHandle->start(process_buffer, this, 
        _common_set_ldslave.theData_in, &_common_set_ldslave.theData_out,
        _inproc.inChannelsSelectionMask,
                                       _inproc.outChannelsSelectionMask,
                                       _inproc.maxChanCntInput,
                                       _inproc.maxChanCntOutput);
	if(res == JVX_NO_ERROR)
	  {
	    if(jvx_bitTest(genIosAudio_device::properties_active_higher.outputSpeaker.value.selection, 0))
            {
                theProcHandle->setSpeakerOption(0);
            }
            else
            {
                theProcHandle->setSpeakerOption(1);
            }
	  }
	
	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}
	genIosAudio_device::properties_active_higher.loadpercent.value = 0;
	genIosAudio_device::properties_active_higher.loadpercent.valid = true;

	return res;
leave_error:
	return res;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxIosAudioDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

	res = theProcHandle->stop();

	genIosAudio_device::properties_active_higher.loadpercent.value = 0;
	genIosAudio_device::properties_active_higher.loadpercent.valid = false;

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxIosAudioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
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

    _common_set_ldslave.theData_in->con_compat.user_hints = NULL;

	return res;
}

jvxErrorType
CjvxIosAudioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxIosAudioDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxIosAudioDevice::process_start_icon(jvxSize pipeline_offset , 
        jvxSize* idx_stage,
	    jvxSize tobeAccessedByStage,
	    callback_process_start_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = shift_buffer_pipeline_idx_on_start(
        pipeline_offset,  
        idx_stage,
	    tobeAccessedByStage,
	    clbk, priv_ptr);

	return res;
}

jvxErrorType
CjvxIosAudioDevice::process_stop_icon(
        jvxSize idx_stage ,
	    jvxBool shift_fwd,
	    jvxSize tobeAccessedByStage,
	    callback_process_stop_in_lock clbk,
	    jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Unlock the buffer for this pipeline
	res = shift_buffer_pipeline_idx_on_stop(
        idx_stage,  shift_fwd,
	    tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

jvxErrorType 
CjvxIosAudioDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxAudioDevice::ext_transfer_backward_ocon(tp, data, &genIosAudio_device::properties_active.ratesselection, &genIosAudio_device::properties_active.sizesselection JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxIosAudioDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::deactivate();
    jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

	if (res == JVX_NO_ERROR)
	{
        CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
        CjvxAudioDevice::properties_active.inputchannelselection.value.selection = 0;
        CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
        CjvxAudioDevice::properties_active.outputchannelselection.value.selection = 0;
        CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        genIosAudio_device::properties_active.sizesselection.value.selection = 0;
        genIosAudio_device::properties_active.sizesselection.value.entries.clear();
        shortcuts.bsizes.clear();

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        genIosAudio_device::properties_active.ratesselection.value.selection = 0;
        genIosAudio_device::properties_active.ratesselection.value.entries.clear();
        shortcuts.srates.clear();

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        CjvxAudioDevice::properties_active.formatselection.value.entries.clear();
        CjvxAudioDevice::properties_active.formatselection.value.selection = 0;
        CjvxAudioDevice::properties_active.format.value = JVX_DATAFORMAT_NONE;
        shortcuts.formats.clear();

        CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.entries.clear();
        CjvxIosAudioDevice::properties_active_higher.inputDataSource.value.selection = 0;

        CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.entries.clear();
        CjvxIosAudioDevice::properties_active_higher.outputDataSource.value.selection = 0;

        theProcHandle->terminateDevice();

		updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true);

		genIosAudio_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genIosAudio_device::deallocate__properties_active_higher();

		genIosAudio_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genIosAudio_device::deallocate__properties_active();
	}
	return (res);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

jvxErrorType
CjvxIosAudioDevice::set_property(jvxCallManagerProperties* callGate,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize propId, 
		jvxPropertyCategoryType category, jvxSize offsetStart, jvxBool contentOnly)
{
	jvxSize i;
	jvxBool report_update = false;
    Boolean locRes = false;
    jvxAudioProcessor_cpp* theProcHandle = &device.theParent->theAudioLink;

    jvxErrorType res = CjvxAudioDevice::set_property(callGate,
		fld, numElements, format, propId, category, 
        offsetStart, contentOnly);

    jvxErrorType resLocal = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
        if(
           (category == genIosAudio_device::properties_active.ratesselection.cat)&&
           (propId == genIosAudio_device::properties_active.ratesselection.id))
        {
            UInt32 newVal = -1;
            for(i = 0; i < genIosAudio_device::properties_active.ratesselection.value.entries.size(); i++)
            {
                if(jvx_bitTest(genIosAudio_device::properties_active.ratesselection.value.selection , i))
                {
                    // TODO
                    //newVal = this->runtime.samplerates[i];
                    break;
                }
            }
            assert(newVal > 0);
						/* TODO
            resLocal = this->setNominalSamplerate((Float64)newVal); */
            assert(resLocal == JVX_NO_ERROR);
        }

        if(
           (category == genIosAudio_device::properties_active.sizesselection.cat)&&
           (propId == genIosAudio_device::properties_active.sizesselection.id))
        {
            UInt32 newVal = -1;
            for(i = 0; i < genIosAudio_device::properties_active.sizesselection.value.entries.size(); i++)
            {
                if(jvx_bitTest(genIosAudio_device::properties_active.sizesselection.value.selection, i))
                {
                    // TODO
                    //newVal = this->runtime.sizesBuffers[i];
                    break;
                }
            }
            assert(newVal > 0);
						/* TODO
            resLocal = this->setDeviceFramesize(newVal);
						*/
            assert(resLocal == JVX_NO_ERROR);
        }

		//updateDependentVariables(propId, category, false, callPurpose);

        if(
            (category == genIosAudio_device::properties_active_higher.inputDataSource.cat) &&
           (propId == genIosAudio_device::properties_active_higher.inputDataSource.id))
        {
            jvxSize id = jvx_bitfieldSelection2Id(genIosAudio_device::properties_active_higher.inputDataSource.value.selection,
                                     genIosAudio_device::properties_active_higher.inputDataSource.value.entries.size());
            assert(JVX_CHECK_SIZE_SELECTED(id));
            res = theProcHandle->setInputOption(id);
            assert(res == JVX_NO_ERROR);
        }
        if(
           (category == genIosAudio_device::properties_active_higher.outputDataSource.cat) &&
           (propId == genIosAudio_device::properties_active_higher.outputDataSource.id))
        {
            jvxSize id = jvx_bitfieldSelection2Id(genIosAudio_device::properties_active_higher.outputDataSource.value.selection,
                                              genIosAudio_device::properties_active_higher.outputDataSource.value.entries.size());
            assert(JVX_CHECK_SIZE_SELECTED(id));
            res = theProcHandle->setOutputOption(id);
            assert(res == JVX_NO_ERROR);
        }
        if(
           (category == genIosAudio_device::properties_active_higher.outputSpeaker.cat) &&
           (propId == genIosAudio_device::properties_active_higher.outputSpeaker.id))
        {
            jvxSize id = jvx_bitfieldSelection2Id(genIosAudio_device::properties_active_higher.outputSpeaker.value.selection,
                                              genIosAudio_device::properties_active_higher.outputSpeaker.value.entries.size());
            assert(JVX_CHECK_SIZE_SELECTED(id));
            res = theProcHandle->setSpeakerOption(id);
            assert(res == JVX_NO_ERROR);
        }

	}
	return (res);
}


jvxErrorType
CjvxIosAudioDevice::put_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno )
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

// Read all parameters for base class
    CjvxAudioDevice::put_configuration(callConf, 
		processor, sectionToContainAllSubsectionsForMe, 
		filename, lineno);

	if (this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		genIosAudio_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, warnings);
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
        
        // Read all parameters from this class
        genIosAudio_device::put_configuration__properties_active_higher(callConf, processor, sectionToContainAllSubsectionsForMe, warnings);
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

		//this->updateDependentVariables(-1, JVX_PROPERTY_CATEGORY_PREDEFINED, true, );

	}
	return (res);
}

jvxErrorType
CjvxIosAudioDevice::get_configuration(jvxCallManagerConfiguration* callConf, 
    	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
    // Write all parameters from base class
	CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

    // Write all parameters from this class
	genIosAudio_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

    // Write all parameters from this class
    genIosAudio_device::get_configuration__properties_active_higher(callConf, processor, sectionWhereToAddAllSubsections);

    return (JVX_NO_ERROR);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================
// ===============================================================================

void
CjvxIosAudioDevice::updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxPropertyCategoryType catTp;
	jvxInt32 newValue = 0;

// Update the variables in base class
//CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll); <- this happens already in the base class

	if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if ((propId == genIosAudio_device::properties_active.ratesselection.id) || (propId == CjvxAudioDevice::properties_active.samplerate.id) || updateAll)
		{
			for (i = 0; i < (int) genIosAudio_device::properties_active.ratesselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(genIosAudio_device::properties_active.ratesselection.value.selection, i))
				{
					newValue = shortcuts.srates[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.samplerate.value = newValue;

		}

		// =================================================================================================
		if ((propId == genIosAudio_device::properties_active.sizesselection.id) || (propId == CjvxAudioDevice::properties_active.buffersize.id) || updateAll)
		{
			for (i = 0; i < (int) genIosAudio_device::properties_active.sizesselection.value.entries.size(); i++)
			{
				if (jvx_bitTest(genIosAudio_device::properties_active.sizesselection.value.selection, i))
				{
                    // TODO
					newValue = shortcuts.bsizes[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.buffersize.value = newValue;


		}
	}
}

void
CjvxIosAudioDevice::processBuffer()
{
    int ii;
    /*
     if(_common_set_audio_device.referene_audio_node)
    {
        _common_set_audio_device.referene_audio_node->process_st(&inProcessing.theData, 0, 0);
    }
    else
    {
        jvxSize maxChans = JVX_MAX(inProcessing.theData.con_params.number_channels,inProcessing.theData.con_compat.number_channels);
        for(ii = 0; ii < maxChans; ii++)
        {
            int idxIn = ii % inProcessing.theData.con_params.number_channels;
            int idxOut = ii % inProcessing.theData.con_compat.number_channels;

            assert(inProcessing.theData.con_params.format == inProcessing.theData.con_compat.format);
            jvx_convertSamples_memcpy(
                                      inProcessing.theData.con_data.buffers[0][idxIn],
                                      inProcessing.theData.con_compat.from_receiver_buffer_allocated_by_sender[0][idxOut],
                                      jvxDataFormat_size[inProcessing.theData.con_params.format],
                                      inProcessing.theData.con_params.buffersize);
        }
	}*/
}
