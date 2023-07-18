#include  "CjvxAudioAlsaDevice.h"
#include "jvx_config.h"


//! Listing of all available samplerates according to AM_SAMPLERATES
#define NUM_SAMPLERATES 11
static long samplerates[NUM_SAMPLERATES] =
{
		8000,
		11025,
		16000,
		22050,
		32000,
		44100,
		48000,
		64000,
		88200,
		96000,
		192000
};

#define NUM_FRAMES_LOG 100
#define TIMOUT_MECS_LOG 5000

CjvxAudioAlsaDevice::CjvxAudioAlsaDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	resetAlsa();

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
	this->resetAlsa();

}


CjvxAudioAlsaDevice::~CjvxAudioAlsaDevice()
{
}


jvxErrorType
CjvxAudioAlsaDevice::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAudioDevice::select(owner);
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}


jvxErrorType
CjvxAudioAlsaDevice::unselect()
{
	jvxErrorType res = CjvxAudioDevice::unselect();
	if (res == JVX_NO_ERROR)
	{
	}
	return (res);
}


jvxErrorType
CjvxAudioAlsaDevice::activate()
{
	jvxSize i,j;

	jvxErrorType res = CjvxAudioDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		genAlsa_device::init__properties_active();
		genAlsa_device::allocate__properties_active();
		genAlsa_device::register__properties_active(static_cast<CjvxProperties*>(this));

		genAlsa_device::init__properties_active_higher();
		genAlsa_device::allocate__properties_active_higher();
		genAlsa_device::register__properties_active_higher(static_cast<CjvxProperties*>(this));
		genAlsa_device::properties_active_higher.loadpercent.isValid = false;

		res = activateAlsaProperties();

		this->_lock_properties_local();

		genAlsa_device::properties_active.accessType.value.entries.clear();
		genAlsa_device::properties_active.accessType.value.selection() = 0;
        genAlsa_device::properties_active.accessType.value.exclusive = 0;

		for(i = 0; i < detectedProps.availableAccessTypes.size(); i++)
		{
			switch (detectedProps.availableAccessTypes[i])
			{
			case SND_PCM_ACCESS_RW_INTERLEAVED:
				genAlsa_device::properties_active.accessType.value.entries.push_back("SND_PCM_ACCESS_RW_INTERLEAVED");
				break;
			case SND_PCM_ACCESS_RW_NONINTERLEAVED:
				genAlsa_device::properties_active.accessType.value.entries.push_back("SND_PCM_ACCESS_RW_NONINTERLEAVED");
				break;
			case SND_PCM_ACCESS_MMAP_INTERLEAVED:
				genAlsa_device::properties_active.accessType.value.entries.push_back("SND_PCM_ACCESS_MMAP_INTERLEAVED");
				break;
			case SND_PCM_ACCESS_MMAP_NONINTERLEAVED:
				genAlsa_device::properties_active.accessType.value.entries.push_back("SND_PCM_ACCESS_MMAP_NONINTERLEAVED");
				break;
			case SND_PCM_ACCESS_MMAP_COMPLEX:
				genAlsa_device::properties_active.accessType.value.entries.push_back("SND_PCM_ACCESS_MMAP_COMPLEX");
				break;
			default:
				assert(0);
			}
		}

		for(i = 0; i <  numberPriorityAccesses; i++)
		{
			snd_pcm_access_t accessTp = priorityAccesses[i];
			for(j = 0; j < detectedProps.availableAccessTypes.size(); j++)
			{
				if(accessTp == detectedProps.availableAccessTypes[j])
				{
					genAlsa_device::properties_active.accessType.value.selection() = (jvxBitField)1 << j;
					break;
				}
			}
			if (JVX_EVALUATE_BITFIELD(genAlsa_device::properties_active.accessType.value.selection()))
			{
				break;
			}
		}

		genAlsa_device::properties_active.formatType.value.entries.clear();
		genAlsa_device::properties_active.formatType.value.selection() = 0;
        genAlsa_device::properties_active.formatType.value.exclusive = 0;

		for(i = 0; i < detectedProps.availableFormatTypes.size(); i++)
		{
			switch (detectedProps.availableFormatTypes[i])
			{
			case SND_PCM_FORMAT_UNKNOWN:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_UNKNOWN");
				break;
			case SND_PCM_FORMAT_S8:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S8");
				break;
			case SND_PCM_FORMAT_U8:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U8");
				break;
			case SND_PCM_FORMAT_S16_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S16_LE");
				break;
			case SND_PCM_FORMAT_S16_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S16_BE");
				break;
			case SND_PCM_FORMAT_U16_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U16_LE");
				break;
			case SND_PCM_FORMAT_U16_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U16_BE");
				break;
			case SND_PCM_FORMAT_S24_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S24_LE");
				break;
			case SND_PCM_FORMAT_S24_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S24_BE");
				break;
			case SND_PCM_FORMAT_U24_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U24_LE");
				break;
			case SND_PCM_FORMAT_U24_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U24_BE");
				break;
			case SND_PCM_FORMAT_S32_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S32_LE");
				break;
			case SND_PCM_FORMAT_S32_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S32_BE");
				break;
			case SND_PCM_FORMAT_U32_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U32_LE");
				break;
			case SND_PCM_FORMAT_U32_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U32_BE");
				break;
			case SND_PCM_FORMAT_FLOAT_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_FLOAT_LE");
				break;
			case SND_PCM_FORMAT_FLOAT_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_FLOAT_BE");
				break;
			case SND_PCM_FORMAT_FLOAT64_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_FLOAT64_LE");
				break;
			case SND_PCM_FORMAT_FLOAT64_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_FLOAT64_BE");
				break;
			case SND_PCM_FORMAT_IEC958_SUBFRAME_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_IEC958_SUBFRAME_LE");
				break;
			case SND_PCM_FORMAT_IEC958_SUBFRAME_BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_IEC958_SUBFRAME_BE");
				break;
			case SND_PCM_FORMAT_MU_LAW:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_MU_LAW");
				break;
			case SND_PCM_FORMAT_A_LAW:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_A_LAW");
				break;
			case SND_PCM_FORMAT_IMA_ADPCM:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_IMA_ADPCM");
				break;
			case SND_PCM_FORMAT_MPEG:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_MPEG");
				break;
			case SND_PCM_FORMAT_GSM:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_GSM");
				break;
			case SND_PCM_FORMAT_SPECIAL:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_SPECIAL");
				break;
			case SND_PCM_FORMAT_S24_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S24_3LE");
				break;
			case SND_PCM_FORMAT_S24_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S24_3BE");
				break;
			case SND_PCM_FORMAT_U24_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U24_3LE");
				break;
			case SND_PCM_FORMAT_U24_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U24_3BE");
				break;
			case SND_PCM_FORMAT_S20_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S20_3LE");
				break;
			case SND_PCM_FORMAT_S20_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S20_3BE");
				break;
			case SND_PCM_FORMAT_U20_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U20_3LE");
				break;
			case SND_PCM_FORMAT_U20_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U20_3BE");
				break;
			case SND_PCM_FORMAT_S18_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S18_3LE");
				break;
			case SND_PCM_FORMAT_S18_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_S18_3BE");
				break;
			case SND_PCM_FORMAT_U18_3LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U18_3LE");
				break;
			case SND_PCM_FORMAT_U18_3BE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_U18_3BE");
				break;
			case SND_PCM_FORMAT_G723_24:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_G723_24");
				break;
			case SND_PCM_FORMAT_G723_24_1B:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_G723_24_1B");
				break;
			case SND_PCM_FORMAT_G723_40:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_G723_40");
				break;
			case SND_PCM_FORMAT_G723_40_1B:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_G723_40_1B");
				break;
			case SND_PCM_FORMAT_DSD_U8:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_DSD_U8");
				break;
			case SND_PCM_FORMAT_DSD_U16_LE:
				genAlsa_device::properties_active.formatType.value.entries.push_back("SND_PCM_FORMAT_DSD_U16_LE");
				break;
			default:
				assert(0);
			}
		}

		for(i = 0; i <  numberPriorityFormats; i++)
		{
			snd_pcm_format_t formatTp = priorityFormats[i];
			for(j = 0; j < detectedProps.availableFormatTypes.size(); j++)
			{
				if(formatTp == detectedProps.availableFormatTypes[j])
				{
					genAlsa_device::properties_active.formatType.value.selection() = (jvxBitField)1 << j;
					break;
				}
			}
			if (JVX_EVALUATE_BITFIELD(genAlsa_device::properties_active.formatType.value.selection()))
			{
				break;
			}
		}

		genAlsa_device::properties_active.periodsOutput.value.val() = detectedProps.output.period.valMin;
		genAlsa_device::properties_active.periodsOutput.value.maxVal = detectedProps.output.period.valMax;
		genAlsa_device::properties_active.periodsOutput.value.minVal = detectedProps.output.period.valMin;

		genAlsa_device::properties_active.periodsInput.value.val() = detectedProps.input.period.valMin;
		genAlsa_device::properties_active.periodsInput.value.maxVal = detectedProps.input.period.valMax;
		genAlsa_device::properties_active.periodsInput.value.minVal = detectedProps.input.period.valMin;

		this->_unlock_properties_local();

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

#ifdef JVX_ALSA_LOGTOFILE
        jvxErrorType resL = theLogger.init_logging(_common_set_min.theHostRef, NUM_FRAMES_LOG, TIMOUT_MECS_LOG);
        if(resL != JVX_NO_ERROR)
        {
            std::cout << "Failed to initialize data logger in module <CjvxAudioAlsaDevice>." << std::endl;
        }
#endif
    }
	return (res);
}

// ===============================================================================

jvxErrorType
CjvxAudioAlsaDevice::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioDevice::prepare();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{        
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.ratesselection);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.sizesselection);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.accessType);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.formatType);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.periodsInput);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.periodsOutput);
		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, 
				genAlsa_device::properties_active.controlThreads);

		CjvxProperties::_unlock_properties_local();
	}
	return (res);
}


jvxErrorType
CjvxAudioAlsaDevice::postprocess()
{
	jvxErrorType res = CjvxAudioDevice::postprocess();
	jvxSize numNotDeallocated = 0;
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		CjvxProperties::_lock_properties_local();

		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.ratesselection);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.sizesselection);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.accessType);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.formatType);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.periodsInput);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.periodsOutput);
		CjvxProperties::_undo_update_property_access_type(
				genAlsa_device::properties_active.controlThreads);

		CjvxProperties::_unlock_properties_local();
	}
	return (res);
}

jvxErrorType
CjvxAudioAlsaDevice::deactivate()
{
	jvxErrorType res = CjvxAudioDevice::deactivate();
	if (res == JVX_NO_ERROR)
	{
#ifdef JVX_ALSA_LOGTOFILE
        theLogger.terminate_logging();
#endif

		this->deactivateAlsaProperties();

		genAlsa_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genAlsa_device::deallocate__properties_active_higher();

		genAlsa_device::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genAlsa_device::deallocate__properties_active();
	}
	return (res);
}


jvxErrorType
CjvxAudioAlsaDevice::set_property(jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr, 
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& detail)
{
	jvxSize i;
	jvxBool report_update = false;

	jvxErrorType res = CjvxAudioDevice::set_property(callGate, rawPtr, ident, detail);
	if (res == JVX_NO_ERROR)
	{
	  JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		updateDependentVariables(propId, category, false, callGate.call_purpose);
	}
	return (res);
}


jvxErrorType
CjvxAudioAlsaDevice::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, 
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
	  genAlsa_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
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
CjvxAudioAlsaDevice::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
// Write all parameters from base class
    CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);

// Write all parameters from this class
    genAlsa_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return (JVX_NO_ERROR);
}


void
CjvxAudioAlsaDevice::updateDependentVariables(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callPurp)
{
	jvxSize i;
	jvxBool report_update = false;
	jvxPropertyCategoryType catTp;
	jvxInt32 newValue = 0;
    jvxBool reportUpdate = false;

// Update the variables in base class
//CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll); <- this happens already in the base class

	if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
	{
		if ((propId == genAlsa_device::properties_active.ratesselection.globalIdx) || (propId == CjvxAudioDevice::properties_active.samplerate.globalIdx) || updateAll)
		{
			for (i = 0; i < (int) genAlsa_device::properties_active.ratesselection.value.entries.size(); i++)
			{
			  if (jvx_bitTest(genAlsa_device::properties_active.ratesselection.value.selection(), i))
				{
					newValue = runtime.samplerates[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.samplerate.value = newValue;
			setSamplerate_alsa(CjvxAudioDevice::properties_active.samplerate.value);
            reportUpdate = true;
		}

		// =================================================================================================
		if ((propId == genAlsa_device::properties_active.sizesselection.globalIdx) || (propId == CjvxAudioDevice::properties_active.buffersize.globalIdx) || updateAll)
		{
			for (i = 0; i < (int) genAlsa_device::properties_active.sizesselection.value.entries.size(); i++)
			{
			  if (jvx_bitTest(genAlsa_device::properties_active.sizesselection.value.selection(), i))
				{
					newValue = runtime.sizesBuffers[i];
					break;
				}
			}
			CjvxAudioDevice::properties_active.buffersize.value = newValue;
            reportUpdate = true;
		}
	}

    if(reportUpdate)
    {
        if(callPurp != JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS)
        {
            if(_common_set_ld_master.refProc)
            {
                jvxSize uId = JVX_SIZE_UNSELECTED;
                _common_set_ld_master.refProc->unique_id_connections(&uId);
                this->_request_test_chain_master(uId);
            }
        }
    }
}

jvxErrorType
CjvxAudioAlsaDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxErrorType res = JVX_NO_ERROR;
    

    // Prepare link data datatype
    _common_set_ocon.theData_out.con_compat.buffersize = _inproc.buffersize;
    _common_set_ocon.theData_out.con_params.buffersize = _inproc.buffersize;
    _common_set_ocon.theData_out.con_params.format = _inproc.format;
    _common_set_ocon.theData_out.con_data.buffers = NULL;
    _common_set_ocon.theData_out.con_data.number_buffers = 1;
    _common_set_ocon.theData_out.con_params.number_channels = _inproc.numInputs;
    _common_set_ocon.theData_out.con_params.rate = CjvxAudioDevice::properties_active.samplerate.value;
    //_common_set_ocon.theData_out.pipeline.idx_stage = 0;

    // New type of connection by propagating through linked elements
    res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
    if(res != JVX_NO_ERROR)
    {
	goto leave_error;
    }

    // =========================================================================
    // Start also ALSA
    // =========================================================================
    res = prepareAlsa();
    if(res != JVX_NO_ERROR)
    {
	_postprocess_chain_master(NULL);
	goto leave_error;
    }

    JVX_GET_TICKCOUNT_US_SETREF(&inProcessing.theTimestamp);
    inProcessing.timestamp_previous = -1;
    inProcessing.deltaT_theory_us = (jvxData) _common_set_ocon.theData_out.con_params.buffersize /
	(jvxData) _common_set_ocon.theData_out.con_params.rate * 1000.0 * 1000.0;
    inProcessing.deltaT_average_us = 0;

    return(res);
leave_error:
		
    return res;
}

jvxErrorType
CjvxAudioAlsaDevice::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
  jvxErrorType res = JVX_NO_ERROR;
  postProcessAlsa();

  _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
  assert(res == JVX_NO_ERROR);
  
  return res;

}

jvxErrorType
CjvxAudioAlsaDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res != JVX_NO_ERROR)
	{
	    goto leave_error;
	}

	genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = true;

#ifdef JVX_ALSA_LOGTOFILE
    res = theLogger.start_processing();
#endif

	res = startAlsa();
	if (res != JVX_NO_ERROR)
	{
	    _stop_chain_master(NULL);
	    goto leave_error;
	}
	genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = true;

	return res;
leave_error:
	return res;
}

jvxErrorType
CjvxAudioAlsaDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = stopAlsa();

    genAlsa_device::properties_active_higher.loadpercent.value = 0;
	genAlsa_device::properties_active_higher.loadpercent.isValid = false;

#ifdef JVX_ALSA_LOGTOFILE
    res = theLogger.stop_processing();
#endif

	res = _stop_chain_master(NULL);
	assert(res == JVX_NO_ERROR);

	return res;
}

jvxErrorType
CjvxAudioAlsaDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(_common_set_icon.theData_in->con_data.number_buffers, 1);
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	// Connect the output side and start this link
	res = allocate_pipeline_and_buffers_prepare_to();

	// Do not attach any user hint into backward direction
	_common_set_icon.theData_in->con_compat.user_hints = NULL;
	// _common_set_icon.theData_in->pipeline.idx_stage = 0;

	return res;
}

jvxErrorType
CjvxAudioAlsaDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxAudioAlsaDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list
	return res;
}

jvxErrorType
CjvxAudioAlsaDevice::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
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
CjvxAudioAlsaDevice::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
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
CjvxAudioAlsaDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return CjvxAudioDevice::CjvxAudioDevice::transfer_backward_ocon_match_setting(tp, data, &genAlsa_device::properties_active.ratesselection,
							   &genAlsa_device::properties_active.sizesselection JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

jvxErrorType
CjvxAudioAlsaDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
    jvxSize idx = JVX_SIZE_UNSELECTED;
    jvxSize i;
    jvxErrorType res = CjvxAudioDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
    if(res == JVX_NO_ERROR)
    {
        if(
                (_common_set_ocon.theData_out.con_params.format == JVX_DATAFORMAT_16BIT_LE) &&
                (_common_set_icon.theData_in->con_params.format == JVX_DATAFORMAT_16BIT_LE))
        {
            std::cout << "Trying to auto-switch access mode in ALSA Driver to <SND_PCM_FORMAT_S16_LE>." << std::endl;
            for(i = 0; i < genAlsa_device::properties_active.formatType.value.entries.size(); i++)
            {
                if(genAlsa_device::properties_active.formatType.value.entries[i] == "SND_PCM_FORMAT_S16_LE")
                {
                    idx = i;
                    break;
                }
            }
            if(JVX_CHECK_SIZE_SELECTED(idx))
            {
                jvx_bitZSet(genAlsa_device::properties_active.formatType.value.selection(), idx);
                std::cout << "TAuto-switch access mode in ALSA Driver has been succeful." << std::endl;

            }
        }
        if(
                (_common_set_ocon.theData_out.con_params.format == JVX_DATAFORMAT_32BIT_LE) &&
                (_common_set_icon.theData_in->con_params.format == JVX_DATAFORMAT_32BIT_LE))
        {
            std::cout << "Trying to auto-switch access mode in ALSA Driver to <SND_PCM_FORMAT_S32_LE>." << std::endl;
            for(i = 0; i < genAlsa_device::properties_active.formatType.value.entries.size(); i++)
            {
                if(genAlsa_device::properties_active.formatType.value.entries[i] == "SND_PCM_FORMAT_S32_LE")
                {
                    idx = i;
                    break;
                }
            }
            if(JVX_CHECK_SIZE_SELECTED(idx))
            {
                jvx_bitZSet(genAlsa_device::properties_active.formatType.value.selection(), idx);
                std::cout << "TAuto-switch access mode in ALSA Driver has been succeful." << std::endl;

            }
        }

    }
    return res;
}
