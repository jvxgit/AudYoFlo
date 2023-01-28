#include "jvx.h"
#include "CjvxAudioNodeVocoder.h"
//#include "jvxWavFiles/jvxWavReader.h"

CjvxAudioNodeVocoder::CjvxAudioNodeVocoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_node_params_1io.preferred.buffersize.min = FRAME_SIZE;
	_common_set_node_params_1io.preferred.buffersize.max = FRAME_SIZE;
	_common_set_node_params_1io.preferred.number_input_channels.max = 2;
	_common_set_node_params_1io.preferred.number_input_channels.min = 1;
	_common_set_node_params_1io.preferred.number_output_channels.min = 1;
	_common_set_node_params_1io.preferred.number_output_channels.max = 2;
	_common_set_node_params_1io.preferred.samplerate.min = SAMPLING_RATE;
	_common_set_node_params_1io.preferred.samplerate.max = SAMPLING_RATE;
	_common_set_node_params_1io.preferred.format.min = JVX_DATAFORMAT_DATA;
	_common_set_node_params_1io.preferred.format.max = JVX_DATAFORMAT_DATA;
}

CjvxAudioNodeVocoder::~CjvxAudioNodeVocoder()
{
}

jvxErrorType
CjvxAudioNodeVocoder::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		genVocoder_node::init__properties_vocoder();
		genVocoder_node::allocate__properties_vocoder();
		genVocoder_node::register__properties_vocoder(static_cast<CjvxProperties*>(this));

/*
		jvxWavReader* theReader = new jvxWavReader;
		theReader->open_file("C:/Users/hauke/Documents/MATLAB/mywav.wav");
		theReader->scan_file(err);*/
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeVocoder::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genVocoder_node::unregister__properties_vocoder(static_cast<CjvxProperties*>(this));
		genVocoder_node::deallocate__properties_vocoder();
	}
	return(res);
};

jvxErrorType
CjvxAudioNodeVocoder::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;

	// This default function does not tolerate a lot of unexpected settings
	assert(theData->con_params.format == theData->con_compat.format);
	assert(theData->con_params.buffersize == theData->con_compat.buffersize);

	res = process_buffer(
		theData->con_data.buffers[idx_sender_to_receiver],
		theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
		theData->con_params.buffersize,
		theData->con_params.rate,
		theData->con_params.number_channels,
		theData->con_compat.number_channels,
		theData->con_params.format);

	return(res);
};

jvxErrorType
CjvxAudioNodeVocoder::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
		jvxSize samplerate, jvxSize numChans_input,jvxSize numChans_output,jvxDataFormat format)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(
		((jvxSize)bSize == CjvxAudioNode::_common_set_node_params_1io.preferred.buffersize.min) &&
		((jvxSize)samplerate == CjvxAudioNode::_common_set_node_params_1io.preferred.samplerate.min) &&
		(format == CjvxAudioNode::_common_set_node_params_1io.preferred.format.min) &&
		((jvxSize)(jvxInt16)numChans_input <= CjvxAudioNode::_common_set_node_params_1io.preferred.number_input_channels.max) &&
		((jvxSize)numChans_input >= CjvxAudioNode::_common_set_node_params_1io.preferred.number_input_channels.min) &&
		((jvxSize)numChans_output >= CjvxAudioNode::_common_set_node_params_1io.preferred.number_output_channels.min) &&
		((jvxSize)numChans_output <= CjvxAudioNode::_common_set_node_params_1io.preferred.number_output_channels.max))
	{
		if(numChans_input == 1)
		{
			theVocoder.process_frame(
				reinterpret_cast<jvxData*>(buffers_input[0]), NULL,
				reinterpret_cast<jvxData*>(buffers_output[0]));
		}
		else
		{
			theVocoder.process_frame(
				reinterpret_cast<jvxData*>(buffers_input[0]),
				reinterpret_cast<jvxData*>(buffers_input[1]),
				reinterpret_cast<jvxData*>(buffers_output[0]));
		}
		if(numChans_output == 2)
		{
			memcpy(buffers_output[1], buffers_output[0], sizeof(jvxData)*bSize);
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_SETTING;
	}

	return(res);
}

jvxErrorType
CjvxAudioNodeVocoder::set_property(
	jvxCallManagerProperties& callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = CjvxAudioNode::set_property(callGate, rawPtr, ident, trans);
	if(res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if(
			(category == genVocoder_node::properties_vocoder.autoModePitch.category) &&
			(propId == genVocoder_node::properties_vocoder.autoModePitch.globalIdx))
		{
			vocoder::operationMode mode = vocoder::FUNCTIONALITY_AUTO;
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.autoModePitch.value.selection() & 0x2))
			{
				mode = vocoder::FUNCTIONALITY_FIXED;
			}

			theVocoder.setAutoModePitch(mode);
		}

		if(
			(category == genVocoder_node::properties_vocoder.autoModeVuv.category) &&
			(propId == genVocoder_node::properties_vocoder.autoModeVuv.globalIdx))
		{
			vocoder::operationMode mode = vocoder::FUNCTIONALITY_AUTO;
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.autoModeVuv.value.selection() & 0x2))
			{
				mode = vocoder::FUNCTIONALITY_FIXED;
			}

			theVocoder.setAutoModeVUV(mode);
		}

		if(
			(category == genVocoder_node::properties_vocoder.MixingCrit.category) &&
			(propId == genVocoder_node::properties_vocoder.MixingCrit.globalIdx))
		{
			theVocoder.setExcitationMixingCrit(genVocoder_node::properties_vocoder.MixingCrit.value);
		}

		if(
			(category == genVocoder_node::properties_vocoder.pitchFrequency.category) &&
			(propId == genVocoder_node::properties_vocoder.pitchFrequency.globalIdx))
		{
			theVocoder.setCurrentPitchFrequency(genVocoder_node::properties_vocoder.pitchFrequency.value);
		}

		if(
			(category == genVocoder_node::properties_vocoder.pitchFrequencyMin.category) &&
			(propId == genVocoder_node::properties_vocoder.pitchFrequencyMin.globalIdx))
		{
			theVocoder.setCurrentPitchFrequencyMin(genVocoder_node::properties_vocoder.pitchFrequencyMin.value);
		}

		if(
			(category == genVocoder_node::properties_vocoder.pitchFrequencyMax.category) &&
			(propId == genVocoder_node::properties_vocoder.pitchFrequencyMax.globalIdx))
		{
			theVocoder.setCurrentPitchFrequencyMax(genVocoder_node::properties_vocoder.pitchFrequencyMax.value);
		}

		if(
			(category == genVocoder_node::properties_vocoder.outputSelect.category) &&
			(propId == genVocoder_node::properties_vocoder.outputSelect.globalIdx))
		{
			vocoder::routingOut outmode = vocoder::ROUTING_OUT_5;
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.outputSelect.value.selection() & 0x1))
			{
				outmode = vocoder::ROUTING_OUT_1;
			}
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.outputSelect.value.selection() & 0x2))
			{
				outmode = vocoder::ROUTING_OUT_2;
			}
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.outputSelect.value.selection() & 0x4))
			{
				outmode = vocoder::ROUTING_OUT_3;
			}
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.outputSelect.value.selection() & 0x8))
			{
				outmode = vocoder::ROUTING_OUT_4;
			}

			theVocoder.setOutputRoute(outmode);
		}

		if(
			(category == genVocoder_node::properties_vocoder.inputSelect.category) &&
			(propId == genVocoder_node::properties_vocoder.inputSelect.globalIdx))
		{
			vocoder::routingIn inmode = vocoder::ROUTING_IN_2;
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.inputSelect.value.selection() & 0x1))
			{
				inmode = vocoder::ROUTING_IN_1;
			}

			theVocoder.setInputRoute(inmode);
		}

		if(
			(category == genVocoder_node::properties_vocoder.hardVuVDecision.category) &&
			(propId == genVocoder_node::properties_vocoder.hardVuVDecision.globalIdx))
		{
			jvxBool hardDec = true;
			if(JVX_EVALUATE_BITFIELD(genVocoder_node::properties_vocoder.hardVuVDecision.value.selection() & 0x2))
			{
				hardDec = false;
			}

			theVocoder.setVUVHardDecisision(hardDec);
		}

		if(
			(category == genVocoder_node::properties_vocoder.lpcOrder.category) &&
			(propId == genVocoder_node::properties_vocoder.lpcOrder.globalIdx))
		{
			theVocoder.setUsedLpcOrder(JVX_DATA2INT32(genVocoder_node::properties_vocoder.lpcOrder.value.val()));
		}


		if(
			(category == genVocoder_node::properties_vocoder.volume.category) &&
			(propId == genVocoder_node::properties_vocoder.volume.globalIdx))
		{
			theVocoder.setVolume(genVocoder_node::properties_vocoder.volume.value);
		}

		if(
			(category == genVocoder_node::properties_vocoder.pitchScale.category) &&
			(propId == genVocoder_node::properties_vocoder.pitchScale.globalIdx))
		{
			theVocoder.setPitchScale(genVocoder_node::properties_vocoder.pitchScale.value);
		}

			if(
			(category == genVocoder_node::properties_vocoder.formantWarp.category) &&
			(propId == genVocoder_node::properties_vocoder.formantWarp.globalIdx))
		{
			theVocoder.setFormantWarp(genVocoder_node::properties_vocoder.formantWarp.value);
		}
	}
	return(res);
}

jvxErrorType
CjvxAudioNodeVocoder::get_property(jvxCallManagerProperties& callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxData dblVal = 0;
	int intVal = 0;
	vocoder::routingOut outR;
	vocoder::routingIn inR;
	vocoder::operationMode mode;
	jvxBool bVal;

	JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
	if(
		(category == genVocoder_node::properties_vocoder.MixingCrit.category) &&
		(propId == genVocoder_node::properties_vocoder.MixingCrit.globalIdx))
	{
		theVocoder.getExcitationMixingCrit(dblVal);
		genVocoder_node::properties_vocoder.MixingCrit.value = dblVal;
	}

	if(
		(category == genVocoder_node::properties_vocoder.pitchFrequency.category) &&
		(propId == genVocoder_node::properties_vocoder.pitchFrequency.globalIdx))
	{
		theVocoder.getCurrentPitchFrequency(intVal);
		genVocoder_node::properties_vocoder.pitchFrequency.value = intVal;
	}

	if(
		(category == genVocoder_node::properties_vocoder.pitchFrequencyMin.category) &&
		(propId == genVocoder_node::properties_vocoder.pitchFrequencyMin.globalIdx))
	{
		theVocoder.getCurrentPitchFrequencyMin(intVal);
		genVocoder_node::properties_vocoder.pitchFrequencyMin.value = intVal;
	}

	if(
		(category == genVocoder_node::properties_vocoder.pitchFrequencyMax.category) &&
		(propId == genVocoder_node::properties_vocoder.pitchFrequencyMax.globalIdx))
	{
		theVocoder.getCurrentPitchFrequencyMax(intVal);
		genVocoder_node::properties_vocoder.pitchFrequencyMax.value = intVal;
	}

	if(
		(category == genVocoder_node::properties_vocoder.outputSelect.category) &&
		(propId == genVocoder_node::properties_vocoder.outputSelect.globalIdx))
	{
		theVocoder.getOutputRoute(outR);
		switch(outR)
		{
		case vocoder::ROUTING_OUT_1:
			genVocoder_node::properties_vocoder.outputSelect.value.selection() = 0x1;
			break;
		case vocoder::ROUTING_OUT_2:
			genVocoder_node::properties_vocoder.outputSelect.value.selection() = 0x2;
			break;
		case vocoder::ROUTING_OUT_3:
			genVocoder_node::properties_vocoder.outputSelect.value.selection() = 0x4;
			break;
		case vocoder::ROUTING_OUT_4:
			genVocoder_node::properties_vocoder.outputSelect.value.selection() = 0x8;
			break;
		case vocoder::ROUTING_OUT_5:
			genVocoder_node::properties_vocoder.outputSelect.value.selection() = 0x10;
			break;
		}
	}

	if(
		(category == genVocoder_node::properties_vocoder.inputSelect.category) &&
		(propId == genVocoder_node::properties_vocoder.inputSelect.globalIdx))
	{
		theVocoder.getInputRoute(inR);
		switch(inR)
		{
		case vocoder::ROUTING_IN_1:
			genVocoder_node::properties_vocoder.inputSelect.value.selection() = 0x1;
			break;
		case vocoder::ROUTING_IN_2:
			genVocoder_node::properties_vocoder.inputSelect.value.selection() = 0x2;
			break;
		}
	}

	theVocoder.getVUVHardDecisision(bVal);
	if(bVal)
	{
		genVocoder_node::properties_vocoder.hardVuVDecision.value.selection() = 0x1;
	}
	else
	{
		genVocoder_node::properties_vocoder.hardVuVDecision.value.selection() = 0x2;
	}


	if(
		(category == genVocoder_node::properties_vocoder.autoModeVuv.category) &&
		(propId == genVocoder_node::properties_vocoder.autoModeVuv.globalIdx))
	{
		theVocoder.getAutoModeVUV(mode);
		switch(mode)
		{
		case vocoder::FUNCTIONALITY_AUTO:
			genVocoder_node::properties_vocoder.autoModeVuv.value.selection() = 0x1;
			break;
		case vocoder::FUNCTIONALITY_FIXED:
			genVocoder_node::properties_vocoder.autoModeVuv.value.selection() = 0x2;
			break;
		}
	}

	if(
		(category == genVocoder_node::properties_vocoder.autoModePitch.category) &&
		(propId == genVocoder_node::properties_vocoder.autoModePitch.globalIdx))
	{
		theVocoder.getAutoModePitch(mode);
		switch(mode)
		{
		case vocoder::FUNCTIONALITY_AUTO:
			genVocoder_node::properties_vocoder.autoModePitch.value.selection() = 0x1;
			break;
		case vocoder::FUNCTIONALITY_FIXED:
			genVocoder_node::properties_vocoder.autoModePitch.value.selection() = 0x2;
			break;
		}
	}

	if(
		(category == genVocoder_node::properties_vocoder.lpcOrder.category) &&
		(propId == genVocoder_node::properties_vocoder.lpcOrder.globalIdx))
	{
		theVocoder.getUsedLpcOrder(intVal);
		genVocoder_node::properties_vocoder.lpcOrder.value.val() = intVal;
	}

	theVocoder.getVolume(dblVal);
	genVocoder_node::properties_vocoder.volume.value = dblVal;

	if(
		(category == genVocoder_node::properties_vocoder.pitchScale.category) &&
		(propId == genVocoder_node::properties_vocoder.pitchScale.globalIdx))
	{
		theVocoder.getPitchScale(dblVal);
		genVocoder_node::properties_vocoder.pitchScale.value = dblVal;
	}

	if(
		(category == genVocoder_node::properties_vocoder.formantWarp.category) &&
		(propId == genVocoder_node::properties_vocoder.formantWarp.globalIdx))
	{
		theVocoder.getFormantWarp(dblVal);
		genVocoder_node::properties_vocoder.formantWarp.value = dblVal;
	}

	return(CjvxAudioNode::get_property(callGate, rawPtr,  ident, trans	));
}

jvxErrorType
CjvxAudioNodeVocoder::prepare()
{
	jvxErrorType res = CjvxAudioNode::prepare();
	if(res == JVX_NO_ERROR)
	{
		if(_common_set_node_params_1io.processing.number_input_channels == 1)
		{
			_update_property_access_type(
				JVX_PROPERTY_ACCESS_READ_ONLY,
				genVocoder_node::properties_vocoder.inputSelect.category,
				genVocoder_node::properties_vocoder.inputSelect.globalIdx);

			this->_lock_properties_local();
			genVocoder_node::properties_vocoder.inputSelect.value.selection() = 0x1;
			this->_unlock_properties_local();
		}
	}
	return(res);
}

jvxErrorType
CjvxAudioNodeVocoder::postprocess()
{
	jvxErrorType res = CjvxAudioNode::postprocess();
	if(res == JVX_NO_ERROR)
	{
		if(_common_set_node_params_1io.processing.number_input_channels == 1)
		{
			_undo_update_property_access_type(
				genVocoder_node::properties_vocoder.inputSelect.category,
				genVocoder_node::properties_vocoder.inputSelect.globalIdx);
		}
	}
	return(res);
}

jvxErrorType CjvxAudioNodeVocoder::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	std::vector<std::string> warnings;
	if(_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genVocoder_node::put_configuration__properties_vocoder(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);
		}
	return JVX_NO_ERROR;
}

jvxErrorType CjvxAudioNodeVocoder::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	if(_common_set_min.theState >= JVX_STATE_ACTIVE)
		{
			genVocoder_node::get_configuration__properties_vocoder(callConf, processor, sectionWhereToAddAllSubsections);
		}
	return JVX_NO_ERROR;
}
