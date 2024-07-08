#include "CjvxSpNLevelControl.h"
#include "jvx-helpers-cpp.h"

/*
 * EXPECTATION: All these functions are called within the main thread, the prop fields
 * are not used directly in the processing thread!
 *
#define SINGLE_VALUE_SET(channelList, varnamestr, varnamefld, lenField) \
	if (varnamefld) \
	{ \
		jvxSize idx_chan = 0; \
		for (auto& set : channelList) \
		{ \
			for (auto& elms : set.second.channels) \
			{ \
				if (idx_chan >= idx_first_set) \
				{ \
					assert(idx_chan < lenField); \
					elms.varnamestr = varnamefld[idx_chan]; \
					if (idx_chan == idx_last_set) \
					{ \
						return JVX_NO_ERROR; \
					} \
				} \
				idx_chan++; \
			} \
		} \
	}

#define SINGLE_VALUE_GET(channelList, varnamestr, varnamefld, lenField) \
	if (varnamefld) \
	{ \
		jvxSize idx_chan = 0; \
		for (auto& set : channelList) \
		{ \
			for (auto& elms : set.second.channels) \
			{ \
				if (idx_chan >= idx_first_set) \
				{ \
					assert(idx_chan < lenField); \
					varnamefld[idx_chan] = elms.varnamestr; \
					if (idx_chan == idx_last_set) \
					{ \
						return JVX_NO_ERROR; \
					} \
				} \
				idx_chan++; \
			} \
		} \
	}
*/

CjvxSpNLevelControl::CjvxSpNLevelControl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	forward_complain = true;
	_common_set_node_base_1io.involveStateSwitchHost = true;
}

CjvxSpNLevelControl::~CjvxSpNLevelControl()
{	
}

jvxErrorType
CjvxSpNLevelControl::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{					
		genLevelMeter_node::init_all();
		genLevelMeter_node::allocate_all();
		genLevelMeter_node::register_all(static_cast<CjvxProperties*>(this));
		genLevelMeter_node::register_callbacks(static_cast<CjvxProperties*>(this), update_level_set, update_level_get, reinterpret_cast<jvxHandle*>(this));
	}
	return res;
}

jvxErrorType
CjvxSpNLevelControl::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genLevelMeter_node::unregister_callbacks(static_cast<CjvxProperties*>(this));
		genLevelMeter_node::unregister_all(static_cast<CjvxProperties*>(this));
		genLevelMeter_node::deallocate_all();

		CjvxBareNode1io::deactivate();
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxSpNLevelControl::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Attach the level meter buffer
		mixer.lenField = _common_set_icon.theData_in->con_params.number_channels;
		
		/*
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrLevel, jvxData, numChannels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrMute, jvxCBool, numChannels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrGain, jvxData, numChannels);

		for (i = 0; i < numChannels; i++)
		{
			ptrMute[i] = c_false;
			ptrGain[i] = 1.0;
		}
		genLevelMeter_node::associate__monitor(
			static_cast<CjvxProperties*>(this),
			ptrLevel, numChannels);

		genLevelMeter_node::associate__control(
			static_cast<CjvxProperties*>(this),
			ptrMute, numChannels, 
			ptrGain, numChannels);
		*/

		// ==================================================================================
		if (mixer.lenField)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldGain, jvxData, mixer.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldAvrg, jvxData, mixer.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldMax, jvxData, mixer.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldMute, jvxBitField, mixer.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldClip, jvxCBool, mixer.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer.fldCntMax, jvxSize, mixer.lenField);
			for (i = 0; i < mixer.lenField; i++)
			{
				mixer.fldGain[i] = 1;
				mixer.fldMute[i] = false;
			}

			genLevelMeter_node::associate__level_control(static_cast<CjvxProperties*>(this),
				mixer.fldGain, mixer.lenField,
				mixer.fldMute, mixer.lenField,
				mixer.fldClip, mixer.lenField,
				mixer.fldAvrg, mixer.lenField,
				mixer.fldMax, mixer.lenField);
		}
		// ==================================================================================
	}
	return res;
}

// void copy_measurement(jvxSize cnt, jvxData* ptr);

jvxErrorType
CjvxSpNLevelControl::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// ==================================================================================
		genLevelMeter_node::deassociate__level_control(static_cast<CjvxProperties*>(this));
		if (mixer.fldMax)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldMax);
		}
		if (mixer.fldAvrg)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldAvrg);
		}
		if (mixer.fldMute)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldMute);
		}
		if (mixer.fldGain)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldGain);
		}
		if (mixer.fldClip)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldClip);
		}
		if (mixer.fldCntMax)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer.fldCntMax);
		}
		mixer.lenField = 0;
		// ==================================================================================

		// Detach the level meter
		//genLevelMeter_node::deassociate__monitor(static_cast<CjvxProperties*>(this));
		//genLevelMeter_node::deassociate__control(static_cast<CjvxProperties*>(this));
		//JVX_DSP_SAFE_DELETE_FIELD(ptrLevel);
		//JVX_DSP_SAFE_DELETE_FIELD(ptrMute);
		// numChannels = 0;
	}
	return res;
}

jvxErrorType
CjvxSpNLevelControl::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!genLevelMeter_node::config.bypass.value)
	{
		jvxSize i, j;
		jvxData alpha = genLevelMeter_node::config.smoothing_alpha.value;
		jvxData m1alpha = 1.0 - alpha;
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_icon.theData_in, idx_stage);
		jvxSize numChannels = _common_set_icon.theData_in->con_params.number_channels;
		jvxSize rate = _common_set_icon.theData_in->con_params.rate;
		jvxSize bsize = _common_set_icon.theData_in->con_params.buffersize;
		jvxDataFormat format = _common_set_icon.theData_in->con_params.format;
		for (i = 0; i < numChannels; i++)
		{
			jvxSize mapi = i;		
			jvxData maxOld = mixer.fldMax[i];
			jvx_fieldLevelGainClip(
				reinterpret_cast<jvxHandle**>(bufsIn), 1,
				bsize, format,
				mixer.fldAvrg, mixer.fldMax, 
				mixer.fldGain, mixer.fldClip, &mapi, //<-mapi is the index in fields!
				0.95, 0.995);

			if (mixer.fldMax[i] > maxOld)
			{
				mixer.fldCntMax[i] = 0;
			}
			if (mixer.fldCntMax[i] >= rate)
			{
				mixer.fldMax[i] *= 0.5;
			}
			else
			{
				mixer.fldCntMax[i] += bsize;
			}
			/*
			jvxData gain = ptrGain[i];
			if (ptrMute[i])
			{
				memset(bufsIn[i], 0, sizeof(jvxData) * _common_set_icon.theData_in->con_params.buffersize);
			}
			for (j = 0; j < _common_set_icon.theData_in->con_params.buffersize; j++)
			{
				jvxData tmp = 0;
				bufsIn[i][j] *= gain;
				tmp = bufsIn[i][j];
				tmp *= tmp;
				ptrLevel[i] = alpha * ptrLevel[i] + m1alpha * tmp;
			}
			*/
		}
	}

	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

// =====================================================================================

jvxErrorType 
CjvxSpNLevelControl::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1io::put_configuration(
		callMan,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			genLevelMeter_node::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);			
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSpNLevelControl::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genLevelMeter_node::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNLevelControl, update_level_get)
{
	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(*rawPtr);
	// This defines 
	// jvxHandle* fld
	// jvxSize numElements
	// jvxDataFormat format 

	/*
	/jvxSize idx_first_set = tune.offsetStart;
	jvxSize idx_last_set = idx_first_set + numElements - 1;
	jvxData* sourceData = nullptr;
	jvxCBool* sourceBool = nullptr;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genLevelMeter_node::level_control.level_clip))
	{
		// sourceBool = gen2AudioMixer_node::mixer_control_input.level_clip.ptr;
		SINGLE_VALUE_GET(registeredChannelListInput, clip, mixer.fldClip, mixer.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_gain))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_gain.ptr;
		SINGLE_VALUE_GET(registeredChannelListInput, gain, mixer_input.fldGain, mixer_input.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_mute))
	{
		// sourceBool = gen2AudioMixer_node::mixer_control_input.level_mute.ptr;
		if (mixer_input.fldMute)
		{
			jvxSize idx_chan = 0;
			for (auto& set : registeredChannelListInput)
			{
				for (auto& elms : set.second.channels)
				{
					if (idx_chan >= idx_first_set)
					{
						assert(idx_chan < mixer_input.lenField);
						jvx_bitZSet(mixer_input.fldMute[idx_chan], 0); // no mute
						if (elms.mute)
						{
							jvx_bitZSet(mixer_input.fldMute[idx_chan], 1); // mute
						}
						if (idx_chan == idx_last_set)
						{
							return JVX_NO_ERROR;
						}
					}
					idx_chan++;
				}
			}
		}
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_avrg))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_avrg.ptr;
		SINGLE_VALUE_GET(registeredChannelListInput, avrg, mixer_input.fldAvrg, mixer_input.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_max))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_avrg.ptr;
		SINGLE_VALUE_GET(registeredChannelListInput, max, mixer_input.fldMax, mixer_input.lenField);
	}

	// ============================================================================
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_clip))
	{
		// sourceBool = gen2AudioMixer_node::mixer_control_input.level_clip.ptr;
		SINGLE_VALUE_GET(registeredChannelListOutput, clip, mixer_output.fldClip, mixer_output.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_gain))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_gain.ptr;
		SINGLE_VALUE_GET(registeredChannelListOutput, gain, mixer_output.fldGain, mixer_output.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_mute))
	{
		// sourceBool = gen2AudioMixer_node::mixer_control_input.level_mute.ptr;
		if (mixer_input.fldMute)
		{
			jvxSize idx_chan = 0;
			for (auto& set : registeredChannelListOutput)
			{
				for (auto& elms : set.second.channels)
				{
					if (idx_chan >= idx_first_set)
					{
						assert(idx_chan < mixer_output.lenField);
						jvx_bitZSet(mixer_output.fldMute[idx_chan], 0); // no mute
						if (elms.mute)
						{
							jvx_bitZSet(mixer_output.fldMute[idx_chan], 1); // mute
						}
						if (idx_chan == idx_last_set)
						{
							return JVX_NO_ERROR;
						}
					}
					idx_chan++;
				}
			}
		}
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_avrg))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_avrg.ptr;
		SINGLE_VALUE_GET(registeredChannelListOutput, avrg, mixer_output.fldAvrg, mixer_output.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_max))
	{
		// sourceData = gen2AudioMixer_node::mixer_control_input.level_avrg.ptr;
		SINGLE_VALUE_GET(registeredChannelListOutput, max, mixer_output.fldMax, mixer_output.lenField);
	}
	*/
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNLevelControl, update_level_set)
{
	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(*rawPtr);
	// This defines 
	// jvxHandle* fld
	// jvxSize numElements
	// jvxDataFormat format 
	/*
	jvxSize idx_first_set = tune.offsetStart;
	jvxSize idx_last_set = idx_first_set + numElements - 1;
	jvxData* targetData = nullptr;
	jvxCBool* targetBool = nullptr;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_clip))
	{
		// targetBool = gen2AudioMixer_node::mixer_control_input.level_clip.ptr;
		SINGLE_VALUE_SET(registeredChannelListInput, clip, mixer_input.fldClip, mixer_input.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_gain))
	{
		// A value in range is "1" element but there might be more than only one entry in the list of values!!
		SINGLE_VALUE_SET(registeredChannelListInput, gain, mixer_input.fldGain, mixer_input.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_mute))
	{
		// targetBool = gen2AudioMixer_node::mixer_control_input.level_mute.ptr;
		if (mixer_input.fldMute)
		{
			jvxSize idx_chan = 0;
			for (auto& set : registeredChannelListInput)
			{
				for (auto& elms : set.second.channels)
				{
					if (idx_chan >= idx_first_set)
					{
						assert(idx_chan < mixer_input.lenField);
						elms.mute = jvx_bitTest(mixer_input.fldMute[idx_chan], 1);
						if (idx_chan == idx_last_set)
						{
							return JVX_NO_ERROR;
						}
					}
					idx_chan++;
				}
			}
		}
	}

	// ======================================================================================
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_clip))
	{
		// targetBool = gen2AudioMixer_node::mixer_control_input.level_clip.ptr;
		SINGLE_VALUE_SET(registeredChannelListOutput, clip, mixer_output.fldClip, mixer_output.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_gain))
	{
		// A value in range is "1" element but there might be more than only one entry in the list of values!!
		SINGLE_VALUE_SET(registeredChannelListOutput, gain, mixer_output.fldGain, mixer_output.lenField);
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_output.mixer_control.level_mute))
	{
		// targetBool = gen2AudioMixer_node::mixer_control_input.level_mute.ptr;
		if (mixer_input.fldMute)
		{
			jvxSize idx_chan = 0;
			for (auto& set : registeredChannelListOutput)
			{
				for (auto& elms : set.second.channels)
				{
					if (idx_chan >= idx_first_set)
					{
						assert(idx_chan < mixer_output.lenField);
						elms.mute = jvx_bitTest(mixer_output.fldMute[idx_chan], 1);
						if (idx_chan == idx_last_set)
						{
							return JVX_NO_ERROR;
						}
					}
					idx_chan++;
				}
			}
		}
	}
	*/
	return JVX_NO_ERROR;
}