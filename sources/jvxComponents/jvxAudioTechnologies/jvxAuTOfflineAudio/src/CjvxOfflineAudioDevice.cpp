#include "CjvxOfflineAudioDevice.h"
#include <cassert>
#include "jvx_config.h"

#include "mcg_exports_matlab.h"

// ==========================================================================

CjvxOfflineAudioDevice::CjvxOfflineAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));

	//resetAsio();
	//allHandlerPtrs = allHandlers;
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_DATA);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_8BIT);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_16BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_32BIT_LE);
	_common_set_audio_device.formats.push_back(JVX_DATAFORMAT_64BIT_LE);

	_common_set.thisisme = static_cast<IjvxObject*>(this);
	//addme.theType.tp = JVX_COMPONENT_AUDIO_NODE;
	//addme.theType.slotid = JVX_SIZE_UNSELECTED;
	//addme.theType.slotsubid = JVX_SIZE_UNSELECTED;
}

CjvxOfflineAudioDevice::~CjvxOfflineAudioDevice()
{
}

jvxErrorType
CjvxOfflineAudioDevice::select(IjvxObject* theOwner)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::select( theOwner);
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		_theExtCallHandler = NULL;
		requestExternalCallHandle(_common_set_min.theHostRef, &_theExtCallHandler);
		initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), _theExtCallObjectName);
#endif
	}
	return(res);
}
 
jvxErrorType
CjvxOfflineAudioDevice::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::unselect();
	if(res == JVX_NO_ERROR)
	{
#ifdef JVX_EXTERNAL_CALL_ENABLED
		terminateExternalCallhandler(_theExtCallHandler);
		returnExternalCallHandle(_common_set_min.theHostRef, _theExtCallHandler);
		_theExtCallHandler = NULL;
		_theExtCallObjectName = "";
#endif
	}
	return(res);
}


jvxErrorType
CjvxOfflineAudioDevice::activate()
{
	jvxErrorType res = JVX_NO_ERROR;

	res = CjvxAudioDevice::activate(); 
	if(res == JVX_NO_ERROR)
	{
		genOfflineAudio_device::init__properties_active();
		genOfflineAudio_device::allocate__properties_active();
		genOfflineAudio_device::register__properties_active(static_cast<CjvxProperties*>(this));

		// Align nmber of channels and exposure
		alignChannelsExposed();
		
	}
	return(res);
}

jvxErrorType
CjvxOfflineAudioDevice::deactivate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::deactivate();
	if(res == JVX_NO_ERROR)
	{
		/*
		genOfflineAudio_device::unregister__properties_active_higher(static_cast<CjvxProperties*>(this));
		genOfflineAudio_device::deallocate__properties_active_higher();
		*/
	}
	return(res);
}

jvxErrorType
CjvxOfflineAudioDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<std::string> warnings;

	// Read all parameters for base class
	CjvxAudioDevice::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno );
	
	if(this->_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// Read all parameters from this class
		
		genOfflineAudio_device::put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		if(!warnings.empty())
		{
			jvxSize i;
			std::string txt;
			for(i = 0; i < warnings.size(); i++)
			{
				txt = "Failed to read property " + warnings[i];
				_report_text_message(txt.c_str(), JVX_REPORT_PRIORITY_WARNING);
			}


			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return(res);
}

jvxErrorType
CjvxOfflineAudioDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	// Write all parameters from base class
	CjvxAudioDevice::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	
	// Write all parameters from this class
	genOfflineAudio_device::get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);

	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxOfflineAudioDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{

	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxAudioDevice::set_property(callGate, rawPtr, ident, trans);
	if(res == JVX_NO_ERROR)
	{		
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (
		    ((category == JVX_PROPERTY_CATEGORY_UNSPECIFIC) && (propId == genOfflineAudio_device::properties_active.numinchannelsmat.globalIdx))||
		    ((category == JVX_PROPERTY_CATEGORY_UNSPECIFIC) && (propId == genOfflineAudio_device::properties_active.numoutchannelsmat.globalIdx)))
		{
			// Copy the current channel selection to the "real" channel handler
			CjvxAudioDevice::properties_active.inputchannelselection.value =
				genOfflineAudio_device::properties_active.inputchannelselection_shadow.value;

			CjvxAudioDevice::properties_active.outputchannelselection.value =
				genOfflineAudio_device::properties_active.outputchannelselection_shadow.value;
		}

		// Update
		alignChannelsExposed();

		CjvxAudioDevice::updateDependentVariables_lock(CjvxAudioDevice::properties_active.inputchannelselection.globalIdx, 
			CjvxAudioDevice::properties_active.inputchannelselection.category, false);
		CjvxAudioDevice::updateDependentVariables_lock(CjvxAudioDevice::properties_active.outputchannelselection.globalIdx, 
			CjvxAudioDevice::properties_active.outputchannelselection.category, false);

		if (
		    ((category == JVX_PROPERTY_CATEGORY_UNSPECIFIC) && (propId == genOfflineAudio_device::properties_active.numinchannelsmat.globalIdx)) ||
		    ((category == JVX_PROPERTY_CATEGORY_PREDEFINED) && (propId == CjvxAudioDevice_genpcg::properties_active.inputchannelselection.globalIdx)) ||
		    ((category == JVX_PROPERTY_CATEGORY_UNSPECIFIC) && (propId == genOfflineAudio_device::properties_active.numoutchannelsmat.globalIdx)) ||
		    ((category == JVX_PROPERTY_CATEGORY_PREDEFINED) && (propId == CjvxAudioDevice_genpcg::properties_active.outputchannelselection.globalIdx)))
		{
			// Copy the updated channel selection to the "shadow" channel handler
			genOfflineAudio_device::properties_active.inputchannelselection_shadow.value =
				CjvxAudioDevice::properties_active.inputchannelselection.value;
 
			genOfflineAudio_device::properties_active.outputchannelselection_shadow.value =
				CjvxAudioDevice::properties_active.outputchannelselection.value;
		}
	}
	return(res);
}

void
CjvxOfflineAudioDevice::alignChannelsExposed()
{
	jvxSize i;
	jvxBitField select = 0;

	// Exposed channels depends only on the number of channels. Channels may also be activated/deactivated
	jvxSize numInChannels = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size();
	jvxSize numOutChannels = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size();

	CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();
	select = 0;
	for(i = 0; i < genOfflineAudio_device::properties_active.numinchannelsmat.value; i++)
	{
		CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.push_back(("input#" + jvx_size2String(i)).c_str());
		if(i < numInChannels)
		{
			if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
			{
				jvx_bitSet(select, i); 
			}
		}
		else
		{
			jvx_bitSet(select, i);
		}
	}
	CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection() = select;


	CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.clear();
	select = 0;
	for(i = 0; i < genOfflineAudio_device::properties_active.numoutchannelsmat.value; i++)
	{
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(("output#" + jvx_size2String(i)).c_str());
		if(i < numOutChannels)
		{
			if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
			{
				jvx_bitSet(select, i);
			}
		}
		else
		{
			jvx_bitSet(select, i);
		}
	}
	CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection() = select;
}

jvxErrorType
CjvxOfflineAudioDevice::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	_common_set_ocon.theData_out.con_params.buffersize = _inproc.buffersize;
	_common_set_ocon.theData_out.con_params.format = _inproc.format;
	_common_set_ocon.theData_out.con_data.buffers = NULL;
	_common_set_ocon.theData_out.con_data.number_buffers = 1;
	_common_set_ocon.theData_out.con_params.number_channels = _inproc.numInputs;
	_common_set_ocon.theData_out.con_params.rate = _inproc.samplerate;
	res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res != JVX_NO_ERROR)
	{
		jvx_neutralDataLinkDescriptor(&_common_set_ocon.theData_out, false);
		goto leave_error;
	}
	return(res);
leave_error:
	return(res);
}

// ================================================================================0
// ================================================================================0

jvxErrorType
CjvxOfflineAudioDevice::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	_common_set_icon.theData_in->con_params.buffersize = _inproc.buffersize;
	_common_set_icon.theData_in->con_params.format = _inproc.format;
	_common_set_icon.theData_in->con_data.buffers = NULL;
	_common_set_icon.theData_in->con_data.number_buffers = JVX_MAX(1, _common_set_icon.theData_in->con_data.number_buffers);
	_common_set_icon.theData_in->con_params.number_channels = _inproc.numOutputs;
	_common_set_icon.theData_in->con_params.rate = _inproc.samplerate;

	res = allocate_pipeline_and_buffers_prepare_to();

	return res;
}

jvxErrorType
CjvxOfflineAudioDevice::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = deallocate_pipeline_and_buffers_postprocess_to();

	return res;
}

jvxErrorType
CjvxOfflineAudioDevice::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	return res;
}

jvxErrorType
CjvxOfflineAudioDevice::process_start_icon(
	jvxSize pipeline_offset, 
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = shift_buffer_pipeline_idx_on_start( pipeline_offset,  idx_stage,
		tobeAccessedByStage, clbk, priv_ptr);

	return res;
}

jvxErrorType
CjvxOfflineAudioDevice::process_stop_icon(
	jvxSize idx_stage, 
	jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;
	// This is the return from the link list

	res = shift_buffer_pipeline_idx_on_stop( 
		idx_stage,  shift_fwd,
		tobeAccessedByStage,
		clbk,
		priv_ptr);

	return res;
}


#ifdef JVX_EXTERNAL_CALL_ENABLED

jvxErrorType 
CjvxOfflineAudioDevice::processOneBatch( /* One matrix of input samples->*/ const jvxExternalDataType* paramIn0, 
			 /* One matrix of output samples->*/ jvxExternalDataType** paramOut0)
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxDataFormat formatIn = JVX_DATAFORMAT_NONE;
	
	jvxInt32 dimXIn = 0;
	jvxInt32 dimYIn = 0;

	if(_common_set_ld_master.state != JVX_STATE_PROCESSING)
	{ 
		return JVX_ERROR_NOT_READY;
	}

	_theExtCallHandler->getPropertiesVariable(paramIn0, &formatIn, NULL, &dimXIn, &dimYIn);
	//_theExtCallHandler->getPropertiesVariable(paramIn0, &formatIn, NULL, &dimXIn, &dimYIn);

	if(
		(formatIn == _inproc.format) &&
		(dimYIn == _inproc.numInputs) &&
		(dimXIn == _inproc.buffersize))
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		assert(res == JVX_NO_ERROR);

		_theExtCallHandler->convertExternalToC(_common_set_ocon.theData_out.con_data.buffers[
			*_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr], dimYIn, dimXIn, formatIn, paramIn0, "input #0");

		res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
		if (res != JVX_NO_ERROR)
		{
			jvx_fields_clear(_common_set_icon.theData_in->con_data.buffers
				[*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr],
				jvxDataFormat_size[_common_set_icon.theData_in->con_params.format],
				_common_set_icon.theData_in->con_params.buffersize,
				_common_set_icon.theData_in->con_params.number_channels);
		}

		_theExtCallHandler->convertCToExternal(paramOut0,
			(const jvxHandle**)_common_set_icon.theData_in->con_data.buffers
				[*_common_set_icon.theData_in->con_pipeline.idx_stage_ptr],
			_inproc.numOutputs, _inproc.buffersize, _inproc.format);
	

		resL = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
		assert(resL == JVX_NO_ERROR);

	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
CjvxOfflineAudioDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxAudioDevice::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxOfflineAudioDevice::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	if (tp == JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS)
	{
		jvxErrorType res = transfer_backward_ocon_match_setting(tp, data, nullptr, nullptr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res == JVX_NO_ERROR)
		{
			updateChainOutputParameter();
		}
		return res;
	}
	return CjvxAudioDevice::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}

#endif
