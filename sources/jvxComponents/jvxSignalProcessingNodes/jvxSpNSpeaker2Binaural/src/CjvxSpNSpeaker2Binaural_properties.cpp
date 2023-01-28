#include "CjvxSpNSpeaker2Binaural.h"

// =====================================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, set_bypass_mode)
{
	/*
	 * Arguments
	 * jvxCallManagerProperties* callGate
	 * jvxHandle* fld
	 * jvxSize* id
	 * jvxPropertyCategoryType* cat
	 * jvxSize* offsetStart
	 * jvxSize* numElements
	 * jvxDataFormat* form
	 */

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSpeaker2Binaural_node::config.bypass))
	{
		// Incorporate the property value
		update_bypass_mode();
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, select_hrtf_file)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, set_speaker_posthook)
{
	jvxSize newSelect = jvx_bitfieldSelection2Id(CjvxSpNSpeaker2Binaural::config.select_speaker.value.selection(),
		CjvxSpNSpeaker2Binaural::config.select_speaker.value.entries.size());

	if (newSelect != idSelectChannel)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);

		idSelectChannel = newSelect;
		CjvxObject::_report_command_request(prio);
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, set_parameter_speaker)
{
	jvxSize numChans = JVX_SIZE_UNSELECTED;
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSpeaker2Binaural_node::parameter.fir_hrtf_left))
	{
		filtLeft.number_channels(&numChans);
		if (JVX_CHECK_SIZE_SELECTED(numChans))
		{
			if (idSelectChannel < numChans)
			{
				jvxData* ptr_fir = nullptr;
				jvxSize len_fir = 0;
				jvxSize nUsed = 0;
				std::string txt = genSpeaker2Binaural_node::parameter.fir_hrtf_left.value;
				filtLeft.access_fir_hrtf(idSelectChannel, &ptr_fir, &len_fir);
				jvx_string2ValueList(txt, ptr_fir, JVX_DATAFORMAT_DATA, len_fir, &nUsed);
			}
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSpeaker2Binaural_node::parameter.fir_hrtf_right))
	{
		filtRight.number_channels(&numChans);
		if (JVX_CHECK_SIZE_SELECTED(numChans))
		{
			if (idSelectChannel < numChans)
			{
				jvxData* ptr_fir = nullptr;
				jvxSize len_fir = 0;
				jvxSize nUsed = 0;
				std::string txt = genSpeaker2Binaural_node::parameter.fir_hrtf_right.value;
				filtRight.access_fir_hrtf(idSelectChannel, &ptr_fir, &len_fir);
				jvx_string2ValueList(txt, ptr_fir, JVX_DATAFORMAT_DATA, len_fir, &nUsed);				
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNSpeaker2Binaural, get_parameter_speaker)
{
	jvxSize numChans = JVX_SIZE_UNSELECTED;
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSpeaker2Binaural_node::parameter.fir_hrtf_left))
	{
		filtLeft.number_channels(&numChans);
		if (JVX_CHECK_SIZE_SELECTED(numChans))
		{
			if (idSelectChannel < numChans)
			{
				std::string txt;
				filtLeft.current_fir_hrtf(idSelectChannel, txt);
				genSpeaker2Binaural_node::parameter.fir_hrtf_left.value = txt;
			}
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genSpeaker2Binaural_node::parameter.fir_hrtf_right))
	{
		filtRight.number_channels(&numChans);
		if (JVX_CHECK_SIZE_SELECTED(numChans))
		{
			if (idSelectChannel < numChans)
			{
				std::string txt;
				filtRight.current_fir_hrtf(idSelectChannel, txt);
				genSpeaker2Binaural_node::parameter.fir_hrtf_right.value = txt;
			}
		}
	}
	return JVX_NO_ERROR;
}

// ==================================================================

void
CjvxSpNSpeaker2Binaural::update_bypass_mode(jvxBool updateChain)
{
	if (genSpeaker2Binaural_node::config.bypass.value)
	{
		force2OutputChannels = false;
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
		_common_set_1io_zerocopy.forward_complain = true;
	}
	else
	{
		force2OutputChannels = true;
		_common_set_ldslave.zeroCopyBuffering_cfg = false;
		_common_set_1io_zerocopy.forward_complain = false;
	}

	if (updateChain)
	{
		JVX_TRIGGER_TEST_CHAIN_1IO_CONNECTOR();
	}
}

void
CjvxSpNSpeaker2Binaural::update_properties(jvxBool mayActRendering)
{
	jvxSize i;
	jvxSize oldSelect = JVX_SIZE_UNSELECTED;
	jvxSize numChans = 0;
	if (mayActRendering)
	{
		genSpeaker2Binaural_node::mode.involved.value = true;
		genSpeaker2Binaural_node::mode.involved.isValid = true;
		genSpeaker2Binaural_node::config.select_speaker.isValid = true;
		genSpeaker2Binaural_node::parameter.fir_hrtf_left.isValid = true;
		genSpeaker2Binaural_node::parameter.fir_hrtf_right.isValid = true;

		filtLeft.number_channels(&numChans);

		for (i = 0; i < numChans; i++)
		{
			genSpeaker2Binaural_node::config.select_speaker.value.entries.push_back(("SPEAKER #" + jvx_size2String(i)).c_str());
		}
		jvx_bitFClear(genSpeaker2Binaural_node::config.select_speaker.value.selection());

		if (oldSelect >= genSpeaker2Binaural_node::config.select_speaker.value.entries.size())
		{
			if (genSpeaker2Binaural_node::config.select_speaker.value.entries.size())
			{
				oldSelect = 0;
			}
			else
			{
				oldSelect = JVX_SIZE_UNSELECTED;

			}
		}
		if (JVX_CHECK_SIZE_SELECTED(oldSelect))
		{
			idSelectChannel = oldSelect;
			jvx_bitZSet(genSpeaker2Binaural_node::config.select_speaker.value.selection(),
				idSelectChannel);
		}

		if (numChans)
		{
			filtLeft.number_channels(&numChans);
			if (idSelectChannel < numChans)
			{
				std::string txt;
				filtLeft.current_fir_hrtf(idSelectChannel, txt);
				genSpeaker2Binaural_node::parameter.fir_hrtf_left.value = txt;
			}
			filtRight.number_channels(&numChans);
			if (idSelectChannel < numChans)
			{
				std::string txt;
				filtRight.current_fir_hrtf(idSelectChannel, txt);
				genSpeaker2Binaural_node::parameter.fir_hrtf_right.value = txt;
			}
		}
		else
		{
			genSpeaker2Binaural_node::parameter.fir_hrtf_left.value = "";
			genSpeaker2Binaural_node::parameter.fir_hrtf_right.value = "";
			genSpeaker2Binaural_node::parameter.fir_hrtf_left.isValid = false;
			genSpeaker2Binaural_node::parameter.fir_hrtf_right.isValid = false;
		}

		_undo_update_property_access_type(
			genSpeaker2Binaural_node::config.bypass.category,
			genSpeaker2Binaural_node::config.bypass.globalIdx);

		_update_property_access_type(
			JVX_PROPERTY_ACCESS_FULL_READ_AND_WRITE,
			genSpeaker2Binaural_node::config.bypass.category,
			genSpeaker2Binaural_node::config.bypass.globalIdx);
		jvx_bitZSet(genSpeaker2Binaural_node::mode.function.value.selection(), 0);
	}
	else
	{
		genSpeaker2Binaural_node::config.select_speaker.value.entries.clear();
		genSpeaker2Binaural_node::config.select_speaker.isValid = false;
		genSpeaker2Binaural_node::mode.involved.isValid = false;
		genSpeaker2Binaural_node::parameter.fir_hrtf_left.value = "";
		genSpeaker2Binaural_node::parameter.fir_hrtf_right.value = "";
		genSpeaker2Binaural_node::parameter.fir_hrtf_left.isValid = false;
		genSpeaker2Binaural_node::parameter.fir_hrtf_right.isValid = false;

		_undo_update_property_access_type(
			genSpeaker2Binaural_node::config.bypass.category,
			genSpeaker2Binaural_node::config.bypass.globalIdx);
		_update_property_access_type(
			JVX_PROPERTY_ACCESS_READ_ONLY,
			genSpeaker2Binaural_node::config.bypass.category,
			genSpeaker2Binaural_node::config.bypass.globalIdx);

		jvx_bitFClear(genSpeaker2Binaural_node::mode.function.value.selection());
	}

	if (genSpeaker2Binaural_node::mode.slave_mode.value)
	{
		genSpeaker2Binaural_node::config.bin_definition_file.isValid = false;
		genSpeaker2Binaural_node::config.clear_definition_file.isValid = false;
	}
	else
	{
		genSpeaker2Binaural_node::config.bin_definition_file.isValid = true;
		genSpeaker2Binaural_node::config.clear_definition_file.isValid = true;
	}
}