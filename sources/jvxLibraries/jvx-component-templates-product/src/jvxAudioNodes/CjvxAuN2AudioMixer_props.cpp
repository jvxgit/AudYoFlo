#include "jvxAudioNodes/CjvxAuN2AudioMixer.h"

/*
 * EXPECTATION: All these functions are called within the main thread, the prop fields
 * are not used directly in the processing thread!
 */
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

// ================================================================================================

void
CjvxAuN2AudioMixer::new_setup_to_properties(jvxBool inputSide, jvxBool outputSide)
{
	jvxSize numChannels = 0;

	jvxCallManagerProperties callGate;
	CjvxProperties::_start_property_report_collect(callGate);

	this->_lock_properties_local();
	if (inputSide)
	{
		numChannels = 0;
		gen2AudioMixer_node::deassociate__mixer_input__mixer_control(static_cast<CjvxProperties*>(this));
		gen2AudioMixer_node::mixer_input.mixer_control.channels.value.clear();

		if (mixer_input.fldMax)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_input.fldMax);
		}
		if (mixer_input.fldAvrg)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_input.fldAvrg);
		}
		if (mixer_input.fldMute)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_input.fldMute);
		}
		if (mixer_input.fldGain)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_input.fldGain);
		}
		if (mixer_input.fldClip)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_input.fldClip);
		}
		mixer_input.lenField = 0;
		if (extender) extender->deleteLinearFields(true);

		for (auto& uidElm : registeredChannelListInput)
		{
			numChannels += uidElm.second.channels.size();
			for (auto& oneChan : uidElm.second.channels)
			{
				gen2AudioMixer_node::mixer_input.mixer_control.channels.value.push_back(oneChan.name);
			}
		}

		if (numChannels)
		{
			mixer_input.lenField = numChannels;

			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_input.fldGain, jvxData, mixer_input.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_input.fldAvrg, jvxData, mixer_input.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_input.fldMax, jvxData, mixer_input.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_input.fldMute, jvxBitField, mixer_input.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_input.fldClip, jvxCBool, mixer_input.lenField);

			if (extender) extender->reallocateLinearFields(true, mixer_input.lenField);

			jvxSize cnt = 0;
			for (auto& uidElm : registeredChannelListInput)
			{
				for (auto& oneChan : uidElm.second.channels)
				{
					oneChan.idx_in_linear_list = cnt;

					assert(cnt < mixer_input.lenField);

					mixer_input.fldGain[cnt] = oneChan.gain;
					jvx_bitZSet(mixer_input.fldMute[cnt], 0);
					if (oneChan.mute)
					{
						jvx_bitZSet(mixer_input.fldMute[cnt], 1);
					}
					mixer_input.fldAvrg[cnt] = oneChan.avrg;
					mixer_input.fldMax[cnt] = oneChan.max;
					mixer_input.fldClip[cnt] = oneChan.clip;

					if (extender) extender->fillLinearFields(true, cnt, oneChan.attSpecificPtr);
					cnt++;
				}
			}

			if (extender) extender->propUpdateLinearFields(this, true);

			gen2AudioMixer_node::associate__mixer_input__mixer_control(static_cast<CjvxProperties*>(this),
				mixer_input.fldGain, mixer_input.lenField,
				mixer_input.fldMute, mixer_input.lenField,
				mixer_input.fldClip, mixer_input.lenField,
				mixer_input.fldAvrg, mixer_input.lenField,
				mixer_input.fldMax, mixer_input.lenField);
			
		}
	}

	if (outputSide)
	{
		numChannels = 0;
		gen2AudioMixer_node::deassociate__mixer_output__mixer_control(static_cast<CjvxProperties*>(this));
		gen2AudioMixer_node::mixer_output.mixer_control.channels.value.clear();

		if (mixer_output.fldMax)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_output.fldMax);
		}
		if (mixer_output.fldAvrg)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_output.fldAvrg);
		}
		if (mixer_output.fldMute)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_output.fldMute);
		}
		if (mixer_output.fldGain)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_output.fldGain);
		}
		if (mixer_output.fldClip)
		{
			JVX_DSP_SAFE_DELETE_FIELD(mixer_output.fldClip);
		}
		mixer_output.lenField = 0;

		if (extender) extender->deleteLinearFields(false);

		for (auto& uidElm : registeredChannelListOutput)
		{
			numChannels += uidElm.second.channels.size();
			for (auto& oneChan : uidElm.second.channels)
			{
				gen2AudioMixer_node::mixer_output.mixer_control.channels.value.push_back(oneChan.name);
			}
		}

		if (numChannels)
		{
			mixer_output.lenField = numChannels;

			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_output.fldGain, jvxData, mixer_output.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_output.fldAvrg, jvxData, mixer_output.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_output.fldMax, jvxData, mixer_output.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_output.fldMute, jvxBitField, mixer_output.lenField);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(mixer_output.fldClip, jvxCBool, mixer_output.lenField);

			if (extender) extender->reallocateLinearFields(false, mixer_output.lenField);

			jvxSize cnt = 0;
			for (auto& uidElm : registeredChannelListOutput)
			{
				for (auto& oneChan : uidElm.second.channels)
				{
					oneChan.idx_in_linear_list = cnt;
					assert(cnt < mixer_output.lenField);

					mixer_output.fldGain[cnt] = oneChan.gain;
					jvx_bitZSet(mixer_output.fldMute[cnt], 0);
					if (oneChan.mute)
					{
						jvx_bitZSet(mixer_output.fldMute[cnt], 1);
					}
					mixer_output.fldAvrg[cnt] = oneChan.avrg;
					mixer_output.fldMax[cnt] = oneChan.max;
					mixer_output.fldClip[cnt] = oneChan.clip;

					if (extender) extender->fillLinearFields(false, cnt, oneChan.attSpecificPtr);
					cnt++;
				}
			}

			if (extender) extender->propUpdateLinearFields(this, false);

			gen2AudioMixer_node::associate__mixer_output__mixer_control(static_cast<CjvxProperties*>(this),
				mixer_output.fldGain, mixer_output.lenField,
				mixer_output.fldMute, mixer_output.lenField,
				mixer_output.fldClip, mixer_output.lenField,
				mixer_output.fldAvrg, mixer_output.lenField,
				mixer_output.fldMax, mixer_output.lenField);

		}
	}
	this->_unlock_properties_local();

	JVX_LOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);
	mixer_input.associationValid = true;
	mixer_output.associationValid = true;
	JVX_UNLOCK_MUTEX(_common_set_nv_proc.safeAcces_proc_tasks);

	// Report the changed properties
	if (inputSide)
	{
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.channels.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_gain.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_avrg.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_max.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_mute.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_clip.descriptor.std_str(), true);
	}
	if (outputSide)
	{
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.channels.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_gain.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_avrg.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_max.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_mute.descriptor.std_str(), true);
		CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_clip.descriptor.std_str(), true);
	}

	CjvxProperties::_stop_property_report_collect(callGate);	
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuN2AudioMixer, set_extend_ifx_reference)
{
	/*
	if (gen2AudioMixer_node::properties_interfacing.interf_mixer.value)
	{
		gen2AudioMixer_node::properties_interfacing.interf_mixer.value->prop_extender_specialization(
			reinterpret_cast<jvxHandle**>(&theDirectMixer_ctrl_clnt),
			jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_DIRECT_MIXER_CONTROL);
		if (theDirectMixer_ctrl_clnt)
		{
			theDirectMixer_ctrl_clnt->register_direct_control_interface(static_cast<IjvxDirectMixerControl*>(this));
		}
	}
	else
	{
		if (theDirectMixer_ctrl_clnt)
		{
			theDirectMixer_ctrl_clnt->unregister_direct_control_interface(static_cast<IjvxDirectMixerControl*>(this));
		}
		theDirectMixer_ctrl_clnt = nullptr;
	}
	*/
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuN2AudioMixer, update_level_get)
{
	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(*rawPtr);
	// This defines 
	// jvxHandle* fld
	// jvxSize numElements
	// jvxDataFormat format 

	jvxSize idx_first_set = tune.offsetStart;
	jvxSize idx_last_set = idx_first_set + numElements - 1;
	jvxData* sourceData = nullptr;
	jvxCBool* sourceBool = nullptr;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::mixer_input.mixer_control.level_clip))
	{
		// sourceBool = gen2AudioMixer_node::mixer_control_input.level_clip.ptr;
		SINGLE_VALUE_GET(registeredChannelListInput, clip, mixer_input.fldClip, mixer_input.lenField);
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
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuN2AudioMixer, update_level_set)
{
	JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(*rawPtr);
	// This defines 
	// jvxHandle* fld
	// jvxSize numElements
	// jvxDataFormat format 

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
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuN2AudioMixer, clear_storage)
{
	inputChannelsInStorage.clear();
	outputChannelsInStorage.clear();
	gen2AudioMixer_node::properties_storage.clear_storage.value = c_false;
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuN2AudioMixer, address_profiles)
{
	jvxBool lstRefresh = false;
	std::string propsReport;
	std::string activateName;
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::profiles.profile_add))
	{
		std::string addme = gen2AudioMixer_node::profiles.profile_add.value;

		// Add current setup to profile list
		auto elmP = profileList.find(addme);

		// If another entry already exists, remove it first
		if (elmP != profileList.end())
		{
			profileList.erase(elmP);
		}

		oneProfileParameters newProfEntry;

		for (auto& set : registeredChannelListInput)
		{
			std::string masName = set.second.masName;
			updateChannelToStorage(newProfEntry.inputChannelsInStorage, masName, set.second.channels, true);
		}
		for (auto& set : registeredChannelListOutput)
		{
			std::string masName = set.second.masName;
			updateChannelToStorage(newProfEntry.outputChannelsInStorage, masName, set.second.channels, false);
		}
		profileList[addme] = newProfEntry;
		lstRefresh = true;
		activateName = addme;
		
		gen2AudioMixer_node::profiles.profile_add.value.clear();
		
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::profiles.clear_profile))
	{
		if (gen2AudioMixer_node::profiles.clear_profile.value == c_true)
		{
			// remove the currently selected profile
			jvxSize selId = jvx_bitfieldSelection2Id(
				gen2AudioMixer_node::profiles.profile_list.value.selection(),
				gen2AudioMixer_node::profiles.profile_list.value.entries.size());
			if (JVX_CHECK_SIZE_SELECTED(selId))
			{
				std::string txt = gen2AudioMixer_node::profiles.profile_list.value.entries[selId];
				auto elmP = profileList.find(txt);
				if (elmP != profileList.end())
				{
					profileList.erase(txt);
				}
				lstRefresh = true;
			}
			gen2AudioMixer_node::profiles.clear_profile.value = c_false;
		}
	}
	else if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, gen2AudioMixer_node::profiles.profile_list))
	{
		jvxSize selId = jvx_bitfieldSelection2Id(
			gen2AudioMixer_node::profiles.profile_list.value.selection(),
			gen2AudioMixer_node::profiles.profile_list.value.entries.size());
		if (JVX_CHECK_SIZE_SELECTED(selId))
		{
			std::string entry = gen2AudioMixer_node::profiles.profile_list.value.entries[selId];

			const auto& elmP = profileList.find(entry);
			
			// Load profile entries
			jvxSize cntChan = 0;
			for (auto& set : registeredChannelListInput)
			{
				std::string masName = set.second.masName;
				for (auto& elmC : set.second.channels)
				{
					updateChannelFromStorage((*elmP).second.inputChannelsInStorage, elmC, masName, true);
					mixer_input.fldGain[cntChan] = elmC.gain;
					jvx_bitZSet(mixer_input.fldMute[cntChan], 0);
					if (elmC.mute)
					{
						jvx_bitZSet(mixer_input.fldMute[cntChan], 1);
					}

					if (extender)
					{
						extender->fillLinearFields(true, cntChan, elmC.attSpecificPtr);
					}
					
					cntChan++;
				}
			}

			cntChan = 0;
			for (auto& set : registeredChannelListOutput)
			{
				std::string masName = set.second.masName;
				for (auto& elmC : set.second.channels)
				{
					updateChannelFromStorage((*elmP).second.outputChannelsInStorage, elmC, masName, false);

					mixer_output.fldGain[cntChan] = elmC.gain;
					jvx_bitZSet(mixer_output.fldMute[cntChan], 0);
					if (elmC.mute)
					{
						jvx_bitZSet(mixer_output.fldMute[cntChan], 1);
					}
					if (extender)
					{
						extender->fillLinearFields(false, cntChan, elmC.attSpecificPtr);
					}
					cntChan++;
				}				
			}

			CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_gain.descriptor.std_str());
			CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_input.mixer_control.level_mute.descriptor.std_str());
			CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_gain.descriptor.std_str());
			CjvxProperties::add_property_report_collect(gen2AudioMixer_node::mixer_output.mixer_control.level_mute.descriptor.std_str());
			if (extender)
			{
				extender->reportPropertyUpdateChannels(true);
				extender->reportPropertyUpdateChannels(false);
			}
		}
	}

	if (lstRefresh)
	{
		update_profile_list_properties(activateName);
		CjvxProperties::add_property_report_collect(
			gen2AudioMixer_node::profiles.profile_list.descriptor.std_str(),
			true);
	}	

	return JVX_NO_ERROR;
}

void 
CjvxAuN2AudioMixer::update_profile_list_properties(const std::string& activateThis)
{
	// All profile operations are operated in the main thread, there is no need to 
	// lock anything
	std::string actProf;
	jvxSize idNew = JVX_SIZE_UNSELECTED;
	jvxSize cnt = 0;
	if (!activateThis.empty())
	{
		actProf = activateThis;
	}
	else
	{
		jvxSize idBefore = jvx_bitfieldSelection2Id(
			gen2AudioMixer_node::profiles.profile_list.value.selection(),
			gen2AudioMixer_node::profiles.profile_list.value.entries.size());
		if (idBefore < gen2AudioMixer_node::profiles.profile_list.value.entries.size())
		{
			actProf = gen2AudioMixer_node::profiles.profile_list.value.entries[idBefore];
		}
	}

	gen2AudioMixer_node::profiles.profile_list.value.entries.clear();
	jvx_bitFClear(gen2AudioMixer_node::profiles.profile_list.value.selection());

	for (auto elm : profileList)
	{
		if (elm.first == actProf)
		{
			idNew = cnt;
		}
		cnt++;
		gen2AudioMixer_node::profiles.profile_list.value.entries.push_back(elm.first);
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idNew))
	{
		if (profileList.size())
		{
			idNew = 0;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(idNew))
	{
		jvx_bitZSet(gen2AudioMixer_node::profiles.profile_list.value.selection(), idNew);
	}
}

