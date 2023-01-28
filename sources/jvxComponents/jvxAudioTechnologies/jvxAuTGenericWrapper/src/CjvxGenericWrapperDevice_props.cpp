#include "CjvxGenericWrapperDevice.h"

void
CjvxGenericWrapperDevice::rereadChannelStatusHw_noLock()
{
	jvxSize i,j;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSelectionList fldSelList;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	if(runtime.theProps)
	{
		if(jvx_findPropertyDescriptor("jvx_selection_input_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(res == JVX_NO_ERROR)
			{
				for(i = 0; i < fldSelList.strList.ll(); i++)
				{
					for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
					{
						if(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
						{
							if(runtime.channelMappings.inputChannelMapper[j].refHint == i)
							{
								if(jvx_bitTest(fldSelList.bitFieldSelected(), i))
								{
									runtime.channelMappings.inputChannelMapper[j].isSelected = true;
								}
								else
								{
									runtime.channelMappings.inputChannelMapper[j].isSelected = false;
								}
							}
						}
					}
				}

				// TODO: What if number of channels changes while in operation? If required in the future, here we would need to add functionality
			}
		}
		else
		{
			assert(0);
		}

		if(jvx_findPropertyDescriptor("jvx_selection_output_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(res == JVX_NO_ERROR)
			{
				for(i = 0; i < fldSelList.strList.ll(); i++)
				{
					for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
					{
						if(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
						{
							if(runtime.channelMappings.outputChannelMapper[j].refHint == i)
							{
								if(jvx_bitTest(fldSelList.bitFieldSelected(), i))
								{
									runtime.channelMappings.outputChannelMapper[j].isSelected = true;
								}
								else
								{
									runtime.channelMappings.outputChannelMapper[j].isSelected = false;
								}
							}
						}
					}
				}

				// TODO: What if number of channels changes while in operation? If required in the future, here we would need to add functionality
			}
		}
		else
		{
			assert(0);
		}
	}
}

void
CjvxGenericWrapperDevice::pullProperty_buffersize_nolock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	runtime.props_hw.buffersize.sellist_exists = false;

	if(runtime.theProps)
	{
		if(jvx_findPropertyDescriptor("jvx_selection_framesize_sellist", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (jvx_check_property_access_ok(res, callGate.access_protocol))
			{
				runtime.props_hw.buffersize.entries.clear();
				runtime.props_hw.buffersize.selection_id = -1;
				runtime.props_hw.buffersize.sellist_exists = true;
				for (i = 0; i < fldSelList.strList.ll(); i++)
				{
					std::string txt = fldSelList.strList.std_str_at(i);
					jvxInt32 val = atoi(txt.c_str());
					runtime.props_hw.buffersize.entries.push_back(val);
					if (jvx_bitTest(fldSelList.bitFieldSelected(), i))
					{
						runtime.props_hw.buffersize.selection_id = (jvxInt32)i;
					}
				}
			}
		}
		if(jvx_findPropertyDescriptor("jvx_framesize_int32", &idx, &format, &num, &decHtTp))
		{
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset(true);
			res = runtime.theProps->get_property(callGate, jPRG(
				&runtime.props_hw.buffersize.entry, 1, JVX_DATAFORMAT_32BIT_LE), 
				ident, trans);
			assert(res == JVX_NO_ERROR);
		}
	}
}

void
CjvxGenericWrapperDevice::pushProperty_buffersize_nolock()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
	if(runtime.theProps)
	{
		if(runtime.props_hw.buffersize.sellist_exists)
		{
			if(jvx_findPropertyDescriptor("jvx_selection_framesize_sellist", &idx, &format, &num, &decHtTp))
			{
				assert(runtime.props_hw.buffersize.selection_id >= 0);
				fldSelList.bitFieldSelected() = ((jvxBitField)1 << runtime.props_hw.buffersize.selection_id);
				
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = runtime.theProps->set_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
				assert(res == JVX_NO_ERROR);
			}
		}
		else
		{
			if(jvx_findPropertyDescriptor("jvx_framesize_int32", &idx, &format, &num, &decHtTp))
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = runtime.theProps->set_property(callGate, jPRG(&runtime.props_hw.buffersize.entry, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
				assert(res == JVX_NO_ERROR);
			}
		}
	}
}

void
CjvxGenericWrapperDevice::pullProperty_samplerate_nolock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	runtime.props_hw.samplerate.sellist_exists = false;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	if(runtime.theProps)
	{
		if(jvx_findPropertyDescriptor("jvx_selection_rate_sellist", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if (jvx_check_property_access_ok(res, callGate.access_protocol))
			{
				runtime.props_hw.samplerate.entries.clear();
				runtime.props_hw.samplerate.selection_id = -1;
				runtime.props_hw.samplerate.sellist_exists = true;
				for (i = 0; i < fldSelList.strList.ll(); i++)
				{
					std::string txt = fldSelList.strList.std_str_at(i);
					jvxInt32 val = atoi(txt.c_str());
					runtime.props_hw.samplerate.entries.push_back(val);
					if (jvx_bitTest(fldSelList.bitFieldSelected(), i))
					{
						runtime.props_hw.samplerate.selection_id = (jvxInt32)i;
					}
				}
			}
		}

		if (runtime.props_hw.samplerate.entries.size() == 0)
		{
			runtime.props_hw.samplerate.sellist_exists = false;
			runtime.props_hw.samplerate.selection_id = -1;
		}

		if(jvx_findPropertyDescriptor("jvx_rate_int32", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&runtime.props_hw.samplerate.entry, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
			bool rr =
				JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
					JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
					runtime.theProps);
			assert(rr);
		}
	}
}

void
CjvxGenericWrapperDevice::pushProperty_samplerate_nolock()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	//jvxCallManagerProperties callGate;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
	if(runtime.theProps)
	{
		if(runtime.props_hw.samplerate.sellist_exists)
		{
			if(jvx_findPropertyDescriptor("jvx_selection_rate_sellist", &idx, &format, &num, &decHtTp))
			{
				assert(runtime.props_hw.samplerate.selection_id >= 0);
				fldSelList.bitFieldSelected() = ((jvxBitField)1 << runtime.props_hw.samplerate.selection_id);

				ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				trans.reset(true);

				res = runtime.theProps->set_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST),
					ident, trans);
				bool rr =
					JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
						JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
						runtime.theProps);
				assert(rr);
			}
		}
		else
		{
			if(jvx_findPropertyDescriptor("jvx_rate_int32", &idx, &format, &num, &decHtTp))
			{
				ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				trans.reset(true);

				res = runtime.theProps->set_property(callGate, jPRG(&runtime.props_hw.samplerate.entry, 1, JVX_DATAFORMAT_32BIT_LE), ident, trans);
				bool rr =
					JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
						JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
						runtime.theProps);
				assert(rr);
			}
		}
	}
}

void
CjvxGenericWrapperDevice::pullProperty_format_nolock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;

	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;

	runtime.props_hw.format.sellist_exists = false;
	if(runtime.theProps)
	{
		if(jvx_findPropertyDescriptor("jvx_selection_formats_sellist", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			if(jvx_check_property_access_ok(res, callGate.access_protocol))
			{
				runtime.props_hw.format.entries.clear();
				runtime.props_hw.format.selection_id = -1;
				runtime.props_hw.format.sellist_exists = true;
				for (i = 0; i < fldSelList.strList.ll(); i++)
				{
					std::string txt = fldSelList.strList.std_str_at(i);
					jvxDataFormat form = jvxDataFormat_decode(txt.c_str());
					runtime.props_hw.format.entries.push_back(form);
					if (jvx_bitTest(fldSelList.bitFieldSelected(), i))
					{
						runtime.props_hw.format.selection_id = (jvxInt32)i;
					}
				}
			}
		}
		if(jvx_findPropertyDescriptor("jvx_processing_format_int16", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset(); 
			res = runtime.theProps->get_property(callGate, jPRG(&runtime.props_hw.format.entry, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
			bool rr =
				JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
					JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
					runtime.theProps);
			assert(rr);
		}
	}
}

void
CjvxGenericWrapperDevice::pushProperty_format_nolock()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSelectionList fldSelList;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	jvxBool isValid = false;
	jvxCallManagerProperties callGate;
	callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
	jPAGID ident;
	jPD trans;

	if(runtime.theProps)
	{
		if(runtime.props_hw.format.sellist_exists)
		{
			if(jvx_findPropertyDescriptor("jvx_selection_formats_sellist", &idx, &format, &num, &decHtTp))
			{
				assert(runtime.props_hw.format.selection_id >= 0);
				fldSelList.bitFieldSelected() = ((jvxBitField)1 << runtime.props_hw.format.selection_id);

				ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				trans.reset(true);

				res = runtime.theProps->set_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
					ident, trans);
				assert(res == JVX_NO_ERROR);
			}
		}
		else
		{
			if(jvx_findPropertyDescriptor("jvx_processing_format_int16", &idx, &format, &num, &decHtTp))
			{
				ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				trans.reset(true);

				res = runtime.theProps->set_property(callGate, jPRG(&runtime.props_hw.format.entry, 1, JVX_DATAFORMAT_16BIT_LE), 
					ident, trans);
				assert(res == JVX_NO_ERROR);
			}
		}
	}
}

// =========================================================================================

void
CjvxGenericWrapperDevice::pullProperty_stopsilence_nolock()
{
	//jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBool valB = c_false;
	jvxData valD = 0;
	jvxSize idx = 0;
	jvxDataFormat format = JVX_DATAFORMAT_NONE;
	jvxInt32 num = 0;
	jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
	//jvxBool isValid = false;
	//jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
	jvxCallManagerProperties callGate;
	jPAGID ident;
	jPD trans;
	if (runtime.theProps)
	{
		if (jvx_findPropertyDescriptor("jvx_activate_fillup_silence_on_stop_int16", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&valB, 1, JVX_DATAFORMAT_16BIT_LE), ident, trans);
			if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
					JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
					runtime.theProps))
			{
				CjvxAudioDevice_genpcg::properties_active.activateSilenceStop.value = valB;
			}

		}

		if (jvx_findPropertyDescriptor("jvx_period_fillup_silence_on_stop_data", &idx, &format, &num, &decHtTp))
		{
			callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
			ident.reset(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
			trans.reset();
			res = runtime.theProps->get_property(callGate, jPRG(&valD, 1, JVX_DATAFORMAT_DATA), ident, trans);
			if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol,
				JVX_PROPERTY_DESCRIBE_IDX_CAT(idx, JVX_PROPERTY_CATEGORY_PREDEFINED),
				runtime.theProps))
			{
				CjvxAudioDevice_genpcg::properties_active.periodSilenceStop.value = valD;
			}
		}
	}
}

void
CjvxGenericWrapperDevice::updateChannelExposure_nolock()
{
	jvxSize i;
	std::string txt;

	CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.clear();
	CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection() = 0;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		//txt = JVX_GENERATE_CHANNEL_TOKEN(runtime.channelMappings.inputChannelMapper[i]);
		jvx_genw_encodeChannelName(txt, runtime.channelMappings.inputChannelMapper[i].theChannelType,
			runtime.channelMappings.inputChannelMapper[i].description,
			runtime.channelMappings.inputChannelMapper[i].friendlyName,
			(jvxInt32)runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice);
		CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.push_back(txt);
		if(runtime.channelMappings.inputChannelMapper[i].isSelected)
		{
			jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i);
		}
	}

	CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.clear();
	CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection() = 0;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		//txt = JVX_GENERATE_CHANNEL_TOKEN(runtime.channelMappings.outputChannelMapper[i]);
		jvx_genw_encodeChannelName(txt, runtime.channelMappings.outputChannelMapper[i].theChannelType,
			runtime.channelMappings.outputChannelMapper[i].description,
			runtime.channelMappings.outputChannelMapper[i].friendlyName,
			(jvxInt32)runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice);
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.push_back(txt);
		if(runtime.channelMappings.outputChannelMapper[i].isSelected)
		{
			jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i);
		}
	}

/*
	// Now update the second version of channels
	genGenericWrapper_device::properties_active.inputchannelselection_order.value.entries = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries;
	genGenericWrapper_device::properties_active.inputchannelselection_order.value.selection() = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection;
	genGenericWrapper_device::properties_active.inputchannelselection_order.value.exclusive = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.exclusive;

	genGenericWrapper_device::properties_active.outputchannelselection_order.value.entries = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries;
	genGenericWrapper_device::properties_active.outputchannelselection_order.value.selection() = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection;
	genGenericWrapper_device::properties_active.outputchannelselection_order.value.exclusive = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.exclusive;
	*/
}

jvxErrorType
CjvxGenericWrapperDevice::rearrangeChannelMapper_noLock()
{
	jvxSize i,j;
	std::vector<oneEntryChannelMapper> new_inputChannelMapper;
	std::string tokenIs;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		runtime.channelMappings.inputChannelMapper[i].tmp.check = false;
	}

	//std::vector<oneEntryChannelMapper> outputChannelMapper;
	for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
	{
		std::string tokenDesired = CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries[i];
        
        //_report_text_message(("++ DESIRED: " + tokenDesired).c_str(),JVX_REPORT_PRIORITY_WARNING);

		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			//std::string tokenIs = JVX_GENERATE_CHANNEL_TOKEN(runtime.channelMappings.inputChannelMapper[j]);
			jvx_genw_encodeChannelName(tokenIs, runtime.channelMappings.inputChannelMapper[j].theChannelType,
				runtime.channelMappings.inputChannelMapper[j].description,
				runtime.channelMappings.inputChannelMapper[j].friendlyName,
				(jvxInt32)runtime.channelMappings.inputChannelMapper[j].toHwFile.pointsToIdAllThisDevice);

            //_report_text_message(("++ COMPARE: " + tokenIs).c_str(),JVX_REPORT_PRIORITY_WARNING);

            if(tokenIs == tokenDesired)
			{
				runtime.channelMappings.inputChannelMapper[j].tmp.check = true;
				if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
				{
                    //_report_text_message("++-- SELECTME: ",JVX_REPORT_PRIORITY_WARNING);
					runtime.channelMappings.inputChannelMapper[j].isSelected = true;
				}
				else
				{ 
                    //_report_text_message("++-- DONTSELECTME: ",JVX_REPORT_PRIORITY_WARNING);
					runtime.channelMappings.inputChannelMapper[j].isSelected = false;
				}
				new_inputChannelMapper.push_back(runtime.channelMappings.inputChannelMapper[j]);
				break;
			}
		}
	}
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.inputChannelMapper[i].tmp.check == false)
		{
			runtime.channelMappings.inputChannelMapper[i].isSelected = false;
			new_inputChannelMapper.push_back(runtime.channelMappings.inputChannelMapper[i]);
		}
	}
	runtime.channelMappings.inputChannelMapper = new_inputChannelMapper;

	// ==========================================================================
	std::vector<oneEntryChannelMapper> new_outputChannelMapper;

	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		runtime.channelMappings.outputChannelMapper[i].tmp.check = false;
	}

	//std::vector<oneEntryChannelMapper> outputChannelMapper;
	for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); i++)
	{
		std::string tokenDesired = CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries[i];
		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			//std::string tokenIs = JVX_GENERATE_CHANNEL_TOKEN(runtime.channelMappings.outputChannelMapper[j]);

			jvx_genw_encodeChannelName(tokenIs, runtime.channelMappings.outputChannelMapper[j].theChannelType,
				runtime.channelMappings.outputChannelMapper[j].description,
				runtime.channelMappings.outputChannelMapper[j].friendlyName,
				(jvxInt32)runtime.channelMappings.outputChannelMapper[j].toHwFile.pointsToIdAllThisDevice);

			if(tokenIs == tokenDesired)
			{
				runtime.channelMappings.outputChannelMapper[j].tmp.check = true;
				if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
				{
					runtime.channelMappings.outputChannelMapper[j].isSelected = true;
				}
				else
				{
					runtime.channelMappings.outputChannelMapper[j].isSelected = false;
				}
				new_outputChannelMapper.push_back(runtime.channelMappings.outputChannelMapper[j]);
				break;
			}
		}
	}
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.outputChannelMapper[i].tmp.check == false)
		{
			runtime.channelMappings.outputChannelMapper[i].isSelected = false;
			new_outputChannelMapper.push_back(runtime.channelMappings.outputChannelMapper[i]);
		}
	}
	runtime.channelMappings.outputChannelMapper = new_outputChannelMapper;

	if(_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		if(onInit.connectedDevice)
		{
			// Transfer channel selection also to the connected device
			pass_channelsetup_nolock();
		}
	}

	return(JVX_NO_ERROR);
}

void
CjvxGenericWrapperDevice::updateChannelInternal_nolock()
{
	jvxSize i;
	std::string txt;

	for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
	{
		if(i < runtime.channelMappings.inputChannelMapper.size())
		{
			if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
			{
				runtime.channelMappings.inputChannelMapper[i].isSelected = true;
			}
			else
			{
				runtime.channelMappings.inputChannelMapper[i].isSelected = false;
			}
		}
	}

	for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); i++)
	{
		if(i < runtime.channelMappings.outputChannelMapper.size())
		{
			if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
			{
				runtime.channelMappings.outputChannelMapper[i].isSelected = true;
			}
			else
			{
				runtime.channelMappings.outputChannelMapper[i].isSelected = false;
			}
		}
	}
}

void
CjvxGenericWrapperDevice::updateDependentVariables_lock(jvxSize propId, jvxPropertyCategoryType category, jvxBool updateAll, jvxPropertyCallPurpose callpurp)
{
	CjvxAudioDevice::updateDependentVariables_lock(propId, category, updateAll, callpurp);
};

/*
void
CjvxGenericWrapperDevice::pushProperties_lock()
{
	jvxSelectionList lst;
	jvxStringList str;
	lst.strList = &str;

	if(onInit.connectedDevice)
	{
		if(runtime.props_hw.buffersize.sellist_exists)
		{
		}
	}
}
*/


// =====================================================================================


void
CjvxGenericWrapperDevice::pass_channelsetup_nolock()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	// Prepare the wrapped device
	if(onInit.connectedDevice)
	{
		jvxSelectionList fldSelList;

		jvxSize idx = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
		jvxBool isValid = false;
		jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;
		// First: specify current status of channels to wrapped device
		jvxCallManagerProperties callGate;
		callGate.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;

		if(jvx_findPropertyDescriptor("jvx_selection_input_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			fldSelList.bitFieldSelected() = 0;
			for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
			{
				if(
					(runtime.channelMappings.inputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE) &&
					(runtime.channelMappings.inputChannelMapper[i].isSelected == true))
				{
					jvx_bitSet(fldSelList.bitFieldSelected(), runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice);
				}
			}

			// If we end up in "report_properties", we should not procede any modification!
			this->runtime.lockParams.allParamsLocked = true;
			if(runtime.theProps)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = runtime.theProps->set_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), ident, trans);
			}
			this->runtime.lockParams.allParamsLocked = false;
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			assert(0);
		}

		idx = 0;
		format = JVX_DATAFORMAT_NONE;
		num = 0;
		decHtTp = JVX_PROPERTY_DECODER_NONE;
		isValid = false;
		accessType = JVX_PROPERTY_ACCESS_READ_ONLY;

		if(jvx_findPropertyDescriptor("jvx_selection_output_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			fldSelList.bitFieldSelected() = 0;
			for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
			{
				if(
					(runtime.channelMappings.outputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE) &&
					(runtime.channelMappings.outputChannelMapper[i].isSelected == true))
				{
					jvx_bitSet(fldSelList.bitFieldSelected(), runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice);
				}
			}
			// If we end up in "report_properties", we should not procede any modification!
			this->runtime.lockParams.allParamsLocked = true;
			if(runtime.theProps)
			{
				jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(idx, JVX_PROPERTY_CATEGORY_PREDEFINED);
				jvx::propertyDetail::CjvxTranferDetail trans(true);

				res = runtime.theProps->set_property(callGate, jPRG(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST), 
					ident, trans);
			}
			this->runtime.lockParams.allParamsLocked = false;
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			assert(0);
		}

		//onInit.connectedDevice->prepare(XX, YY);
	}
}

jvxErrorType
CjvxGenericWrapperDevice::updateChannelMapperWithFiles_nolock()
{
	jvxSize i,j;
	jvxBool keepon;
	std::string tmp;
	jvxInt32 numDummyChannels = 0;
	jvxInt32 newIdDummy = 0;


	// First, check what is missing on input files
	for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	{
		jvxInt32 numChannelsWithThisId = 0;

		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if(
				(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) &&
				(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theFilesRef->theInputFiles[i].common.uniqueIdHandles)
				)
			{
				numChannelsWithThisId++;
			}
		}
		if(numChannelsWithThisId < onInit.theFilesRef->theInputFiles[i].common.number_channels)
		{
			// Add channel
			for(j = numChannelsWithThisId; j < onInit.theFilesRef->theInputFiles[i].common.number_channels; j++)
			{
				this->addChannel_input_nolock(j, onInit.theFilesRef->theInputFiles[i].common.uniqueIdHandles, onInit.theFilesRef->theInputFiles[i].common.name, onInit.theFilesRef->theInputFiles[i].common.friendlyName, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
			}
		}
		/*
		else
		{
			if (numChannelsWithThisId)
			{
				for (j = numChannelsWithThisId - 1; j >= onInit.theFilesRef->theInputFiles[i].common.number_channels; j--)
				{
					this->remChannel_input_nolock(j, onInit.theFilesRef->theInputFiles[i].common.uniqueIdHandles, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
				}
			}
		}*/
	}

	// First, check what is missing on input channel references
	for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
	{
		jvxInt32 numChannelsWithThisId = 0;

		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if(
				(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) &&
				(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theExtRef->theInputReferences[i].uniqueIdHandles)
				)
			{
				numChannelsWithThisId++;
			}
		}
		if(numChannelsWithThisId < onInit.theExtRef->theInputReferences[i].number_channels)
		{
			// Add channel
			for(j = numChannelsWithThisId; j < onInit.theExtRef->theInputReferences[i].number_channels; j++)
			{
				this->addChannel_input_nolock(j, onInit.theExtRef->theInputReferences[i].uniqueIdHandles, onInit.theExtRef->theInputReferences[i].description, onInit.theExtRef->theInputReferences[i].description, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
			}
		}
		/*
		else
		{
			assert(0);
			if (numChannelsWithThisId)
			{
				for (j = numChannelsWithThisId - 1; j >= onInit.theExtRef->theInputReferences[i].number_channels; j--)
				{
					this->remChannel_input_nolock(j, onInit.theExtRef->theInputReferences[i].uniqueIdHandles, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
				}
			}
		}*/
	}

	// Check what is missing on dummy input channels
	numDummyChannels = 0;
	newIdDummy = 0;

	for (j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
	{
		if (runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY)
		{
			newIdDummy = JVX_MAX(newIdDummy, runtime.channelMappings.inputChannelMapper[j].refHint);
			numDummyChannels++;
		}
	}
	if (numDummyChannels < onInit.theDummyRef->inputs.size())
	{
		// Add channel
		for (j = numDummyChannels; j < onInit.theDummyRef->inputs.size(); j++)
		{
			tmp = "Dummy";
			this->addChannel_input_nolock(j, onInit.theDummyRef->inputs[j].uniqueId, tmp, tmp, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY);
		}
	}

	keepon = true;
	while(keepon)
	{
		keepon = false;

		// Next, check what is too much
		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE)
			{
				jvxBool foundFile = false;
				for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
				{
					if(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theFilesRef->theInputFiles[i].common.uniqueIdHandles)
					{
						foundFile = true;
					}
				}
				if(!foundFile)
				{
					this->remChannel_input_id_nolock(runtime.channelMappings.inputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
					keepon = true;
					break;
				}
			}
		}
	}

	keepon = true;
	while(keepon)
	{
		keepon = false;
		// Next, check what is too much
		for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL)
			{
				jvxBool foundFile = false;
				for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
				{
					if(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theExtRef->theInputReferences[i].uniqueIdHandles)
					{
						foundFile = true;
					}
				}
				if(!foundFile)
				{
					this->remChannel_input_id_nolock(runtime.channelMappings.inputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
					keepon = true;
					break;
				}
			}
		}
	}

	keepon = true;
	while (keepon)
	{
		keepon = false;
		// Next, check what is too much
		for (j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
		{
			if (runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY)
			{
				jvxBool foundFile = false;
				for (i = 0; i < onInit.theDummyRef->inputs.size(); i++)
				{
					if (runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theDummyRef->inputs[i].uniqueId)
					{
						foundFile = true;
					}
				}
				if (!foundFile)
				{
					this->remChannel_input_id_nolock(runtime.channelMappings.inputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY);
					keepon = true;
					break;
				}
			}
		}
	}





	// First, check what is missing on output files
	for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	{
		jvxInt32 numChannelsWithThisId = 0;

		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if(
				(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) &&
				(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles)
				)
			{
				numChannelsWithThisId++;
			}
		}
		if(numChannelsWithThisId < onInit.theFilesRef->theOutputFiles[i].common.number_channels)
		{
			// Add channel
			for(j = numChannelsWithThisId; j < onInit.theFilesRef->theOutputFiles[i].common.number_channels; j++)
			{
				this->addChannel_output_nolock(j, onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles, onInit.theFilesRef->theOutputFiles[i].common.name, onInit.theFilesRef->theOutputFiles[i].common.friendlyName, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
			}
		} 
		
		else if(numChannelsWithThisId > onInit.theFilesRef->theOutputFiles[i].common.number_channels)
		{
			for(j = numChannelsWithThisId-1; j >= onInit.theFilesRef->theOutputFiles[i].common.number_channels; j--)
			{
				this->remChannel_output_nolock(j, onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
			}
		}
		
		// In case the friendly name has changed, we need to update this
		this->renameChannel_output_name_nolock(onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles, onInit.theFilesRef->theOutputFiles[i].common.name, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
	}

	for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
	{
		jvxInt32 numChannelsWithThisId = 0;

		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if(
				(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) &&
				(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theExtRef->theOutputReferences[i].uniqueIdHandles)
				)
			{
				numChannelsWithThisId++;
			}
		}
		if(numChannelsWithThisId < onInit.theExtRef->theOutputReferences[i].number_channels)
		{
			// Add channel
			for(j = numChannelsWithThisId; j < onInit.theExtRef->theOutputReferences[i].number_channels; j++)
			{
				this->addChannel_output_nolock(j, onInit.theExtRef->theOutputReferences[i].uniqueIdHandles, onInit.theExtRef->theOutputReferences[i].description, onInit.theExtRef->theOutputReferences[i].description, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
			}
		}
		/*
		else
		{
			assert(0);
			for(j = numChannelsWithThisId-1; j >= onInit.theExtRef->theOutputReferences[i].number_channels; j--)
			{
				this->remChannel_output_nolock(j, onInit.theExtRef->theOutputReferences[i].uniqueIdHandles, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
			}
		}*/
	}

	// Check what is missing on dummy input channels
	numDummyChannels = 0;
	newIdDummy = 0;

	for (j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
	{
		if (runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY)
		{
			newIdDummy = JVX_MAX(newIdDummy, runtime.channelMappings.outputChannelMapper[j].refHint);
			numDummyChannels++;
		}
	}
	if (numDummyChannels < onInit.theDummyRef->outputs.size())
	{
		// Add channel
		for (j = numDummyChannels; j < onInit.theDummyRef->outputs.size(); j++)
		{
			tmp = "Dummy";
			this->addChannel_output_nolock(j, onInit.theDummyRef->outputs[j].uniqueId, tmp, tmp, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY);
		}
	}

	// Remove no longer used channels

	keepon = true;
	while(keepon)
	{
		keepon = false;

		// Next, check what is too much
		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE)
			{
				jvxBool foundFile = false;
				for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
				{
					if(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles)
					{
						foundFile = true;
					}
				}
				if(!foundFile)
				{
					this->remChannel_output_id_nolock(runtime.channelMappings.outputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE);
					keepon = true;
					break;
				}
			}
		}
	}

	keepon = true;
	while(keepon)
	{
		keepon = false;
		// Next, check what is too much
		for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL)
			{
				jvxBool foundFile = false;
				for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
				{
					if(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theExtRef->theOutputReferences[i].uniqueIdHandles)
					{
						foundFile = true;
					}
				}
				if(!foundFile)
				{
					this->remChannel_output_id_nolock(runtime.channelMappings.outputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL);
					keepon = true;
					break;
				}
			}
		}
	}

	keepon = true;
	while (keepon)
	{
		keepon = false;
		// Next, check what is too much
		for (j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
		{
			if (runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY)
			{
				jvxBool foundFile = false;
				for (i = 0; i < onInit.theDummyRef->outputs.size(); i++)
				{
					if (runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theDummyRef->outputs[i].uniqueId)
					{
						foundFile = true;
					}
				}
				if (!foundFile)
				{
					this->remChannel_output_id_nolock(runtime.channelMappings.outputChannelMapper[j].refHint, JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY);
					keepon = true;
					break;
				}
			}
		}
	}
	return(JVX_NO_ERROR);
}


void
CjvxGenericWrapperDevice::reportUpdateAllPropertiesInput()
{
	_report_property_has_changed(
		CjvxAudioDevice_genpcg::properties_active.inputchannelselection.category,
		CjvxAudioDevice_genpcg::properties_active.inputchannelselection.globalIdx,
		0, 1,
		false, (JVX_COMPONENT_UNKNOWN));
}

void
CjvxGenericWrapperDevice::reportUpdateAllPropertiesOutput()
{
	_report_property_has_changed(
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.category,
		CjvxAudioDevice_genpcg::properties_active.outputchannelselection.globalIdx, 0, 1,
		false, (JVX_COMPONENT_UNKNOWN));
}

jvxErrorType
CjvxGenericWrapperDevice::number_properties(jvxCallManagerProperties& callGate, jvxSize* num)
{
	jvxSize numL = 0;
	jvxSize numO = 0;
	jvxErrorType res = JVX_MY_BASE_CLASS_D::number_properties(callGate, &numL);
	jvxErrorType resL = JVX_NO_ERROR;

	if(res == JVX_NO_ERROR)
	{
		if(onInit.connectedDevice)
		{
			IjvxProperties* theProps = NULL;
			resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
			if((resL == JVX_NO_ERROR) && theProps)
			{
				resL = theProps->number_properties(callGate, &numO);
				onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
			}
		}
		if(num)
		{
			*num = numL + numO;
		}
	}
	return(res);
}

jvxErrorType 
CjvxGenericWrapperDevice::description_property(
	jvxCallManagerProperties& callGate,
	jvx::propertyDescriptor::IjvxPropertyDescriptor& descr,
	const jvx::propertyAddress::IjvxPropertyAddress& idProp)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::description_property(callGate, descr, idProp);
	if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if(onInit.connectedDevice)
		{
			res = forwardPropertyRequest(callGate,
				&descr, idProp,
				[&](IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& locident)
				{
					jvxErrorType res = theProps->description_property(callGate, descr, locident);
					return res;
				},
				[&](jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dnms, jvx::propertyDescriptor::CjvxPropertyDescriptorMin* dmin)
				{
					if (dnms)
					{
						std::string tag = jvx_makePathExpr("/JVX_GENW", dnms->descriptor.std_str());
						dnms->descriptor.assign(tag);
						dnms->description.assign("<" + dnms->description.std_str() + ">");

					}
					if (dmin)
					{
						dmin->globalIdx += JVX_PROPERTIES_OFFSET_SUBCOMPONENT;
					}
				});
		}
	}
	return(res);
}

/*
jvxErrorType
CjvxGenericWrapperDevice::description_property(
	jvxCallManagerProperties& callGate, jvxSize idx, jvxPropertyCategoryType* category,
	jvxUInt64* settype, jvxUInt64* stateAccess,
	jvxDataFormat* format, jvxSize* sz, jvxPropertyAccessType* accessType, 
	jvxPropertyDecoderHintType* decTp, jvxSize* globalIdx,
	jvxPropertyContext* ctxt, jvxApiString* name, jvxApiString* description, 
	jvxApiString* descriptor, bool* isValid,
	jvxFlagTag* accFlags)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxPropertyCategoryType categoryO;
	jvxUInt64 settypeO;
	jvxUInt64 stateAccessO;
	jvxDataFormat formatO;
	jvxSize szO;
	jvxPropertyAccessType accessTypeO;
	jvxPropertyDecoderHintType decTpO;
	jvxSize globalIdxO;
	jvxApiString nameO ;
	jvxApiString descriptionO;
	jvxApiString descriptOR;
	std::string nmO;
	std::string descrO;
	std::string descrOR;
	jvxBool isVal = false;
	res = JVX_MY_BASE_CLASS_D::description_property(callGate, idx, category, settype, stateAccess, format, sz, accessType, decTp, globalIdx, ctxt, name, 
		description, descriptor, isValid, accFlags);

	if(res == JVX_ERROR_ID_OUT_OF_BOUNDS)
	{
		// Id may target the properties of the sub component
		jvxSize numL = 0;
		res = JVX_MY_BASE_CLASS_D::number_properties(callGate, &numL);
		if(res == JVX_NO_ERROR)
		{
			idx -= numL;
			if(onInit.connectedDevice)
			{
				IjvxProperties* theProps = NULL;
				resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
				if((resL == JVX_NO_ERROR) && theProps)
				{
					res = theProps->description_property(callGate, idx, &categoryO, &settypeO, &stateAccessO, &formatO, &szO, &accessTypeO, &decTpO, &globalIdxO, 
						ctxt, &nameO, &descriptionO, &descriptOR, &isVal, accFlags);
					if(res == JVX_NO_ERROR)
					{
						nmO = nameO.std_str();
						descrO = "<" + descriptionO.std_str() + ">";
						descrOR = "/JVX_GENW" + descriptOR.std_str();

						globalIdxO += JVX_PROPERTIES_OFFSET_SUBCOMPONENT;

						if(category)
						{
							*category = categoryO;
						}

						if(settype)
						{
							*settype = settypeO;
						}

						if(stateAccess)
						{
							*stateAccess = stateAccessO;
						}

						if(format)
						{
							*format = formatO;
						}

						if(sz)
						{
							*sz = szO;
						}

						if(accessType)
						{
							*accessType = accessTypeO;
						}

						if(decTp)
						{
							*decTp = decTpO;
						}

						if(globalIdx)
						{
							*globalIdx = globalIdxO;
						}
						if(isValid)
						{
							*isValid = isVal;
						}

						if (name)
						{
							name->assign(nmO);
						}
						if(description)
						{
							description->assign(descrO);
						}
						if (descriptor)
						{
							descriptor->assign(descrOR);
						}
					}
					 onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
				}
			}
		}
	}
	return(res);
}
*/

jvxErrorType
CjvxGenericWrapperDevice::request_takeover_property(jvxCallManagerProperties& callGate,
	const jvxComponentIdentification& thisismytype, IjvxObject* thisisme, jvxHandle* fld, jvxSize numElements,
	jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent, jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxInt32 tmp32;
	jvxInt32 numChosen = 0;
	jvxInt32 numSelect = 0;
	jvxInt32 numUnselect = 0;
	
	if(JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.autoOff.value.selection() & 0x1))
	{
		if(
			(
			(CjvxAudioDevice_genpcg::properties_active.buffersize.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.buffersize.category == cat)
			) ||
			(
			(CjvxAudioDevice_genpcg::properties_active.samplerate.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.samplerate.category == cat)
			) ||
			(
			(CjvxAudioDevice_genpcg::properties_active.format.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.format.category == cat)
			))
		{
			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(propId, cat);
			jvx::propertyDetail::CjvxTranferDetail trans(onlyContent, offsetStart);

			res = set_property(callGate, jPRG(fld, numElements, format), ident, trans);
		}

		if(
			(CjvxAudioDevice_genpcg::properties_active.numberinputchannels.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.numberinputchannels.category == cat))
		{
			assert(format == JVX_DATAFORMAT_32BIT_LE);
			assert(numElements == 1);
			assert(offsetStart == 0);
			assert(fld);

			res = JVX_NO_ERROR;

			this->_lock_properties_local();
			tmp32 = *((jvxInt32*)fld);
			if(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size() >= tmp32)
			{

				for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
				{
					if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i))
					{
						numChosen ++;
					}
				}
				numSelect = JVX_MAX(0, tmp32 - numChosen);
				numUnselect = JVX_MAX(0, numChosen - tmp32);

				if(numSelect > 0)
				{
					for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
					{
						if(i != last_user_interaction.last_selection_user_input_channels)
						{
							if( !
								(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i)))
							{
								jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), i);
								numSelect--;
								if(numSelect == 0)
								{
									break;
								}
							}
						}
					}
				}

				if(numUnselect > 0)
				{
					for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size(); i++)
					{
						jvxSize idx = (CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size() - i - 1);
						if(idx != last_user_interaction.last_selection_user_input_channels)
						{
							if(
								(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx)))
							{
								jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), idx);
								numUnselect--;
								if(numUnselect == 0)
								{
									break;
								}
							}
						}
					}
				}
				if(numSelect > 0)
				{
					assert(numSelect == 1);
					jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), last_user_interaction.last_selection_user_input_channels);
				}
				if(numUnselect > 0)
				{
					assert(numUnselect == 1);
					jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection(), last_user_interaction.last_selection_user_input_channels);
				}
				last_user_interaction.last_selection_user_input_channels = -1;
			}
			else
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			this->_unlock_properties_local();
			if(res == JVX_NO_ERROR)
			{
				JVX_MY_BASE_CLASS_D::updateDependentVariables_lock(
					CjvxAudioDevice_genpcg::properties_active.inputchannelselection.globalIdx,
					CjvxAudioDevice_genpcg::properties_active.inputchannelselection.category,
					true, JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER);

				this->lock_settings();
				rearrangeChannelMapper_noLock();
				updateChannelInternal_nolock();
				//this->updateSWSamplerateAndBuffersize_nolock();
				assert(0);
				this->unlock_settings();

			}
		}

		if(
			(CjvxAudioDevice_genpcg::properties_active.numberoutputchannels.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.numberoutputchannels.category == cat))
		{
			assert(format == JVX_DATAFORMAT_32BIT_LE);
			assert(numElements == 1);
			assert(offsetStart == 0);
			assert(fld);

			res = JVX_NO_ERROR;

			this->_lock_properties_local();
			tmp32 = *(jvxInt32*)fld;
			if(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size() >= tmp32)
			{

				for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); i++)
				{
					if(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i))
					{
						numChosen ++;
					}
				}
				numSelect = JVX_MAX(0, tmp32 - numChosen);
				numUnselect = JVX_MAX(0, numChosen - tmp32);

				if(numSelect > 0)
				{
					for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size(); i++)
					{
						if(i != last_user_interaction.last_selection_user_output_channels)
						{
							if( !
								(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(),i)))
							{
								jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), i);
								numSelect--;
								if(numSelect == 0)
								{
									break;
								}
							}
						}
					}
				}

				if(numUnselect > 0)
				{
					for(i = 0; i < CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size() ; i++)
					{
						jvxSize idx = (CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size() - i - 1);
						if(idx != last_user_interaction.last_selection_user_output_channels)
							if(
								(jvx_bitTest(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), idx)))
							{
								jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), idx);
								numUnselect--;
								if(numUnselect == 0)
								{
									break;
								}
							}
					}
				}
				if(numSelect > 0)
				{
					assert(numSelect == 1);
					jvx_bitSet(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), last_user_interaction.last_selection_user_output_channels);
				}
				if(numUnselect > 0)
				{
				  assert(numUnselect == 1);
				  jvx_bitClear(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection(), last_user_interaction.last_selection_user_output_channels);
				}
				last_user_interaction.last_selection_user_output_channels = -1;

			}
			else
			{
				res = JVX_ERROR_INVALID_SETTING;
			}
			this->_unlock_properties_local();

			if(res == JVX_NO_ERROR)
			{
				JVX_MY_BASE_CLASS_D::updateDependentVariables_lock(
					CjvxAudioDevice_genpcg::properties_active.outputchannelselection.globalIdx,
					CjvxAudioDevice_genpcg::properties_active.outputchannelselection.category,
					true, JVX_PROPERTY_CALL_PURPOSE_TAKE_OVER);

				this->lock_settings();
				rearrangeChannelMapper_noLock();
				updateChannelInternal_nolock();
				//this->updateSWSamplerateAndBuffersize_nolock();
				assert(0);
				this->unlock_settings();

			}
		}
	}

	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::set_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxBool updateUi = false;
	jvxBool updatePrcParams = false;
	jvxBitField bitFieldBefore;
	jvxBitField bitFieldDiff;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::set_property(callGate, rawPtr, ident, trans);

	if (res == JVX_NO_ERROR)
	{
		JVX_TRANSLATE_PROP_GENERIC_FROM_RAW(rawPtr);
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		// Try to find out which channel the user clicked to avoid undo of user interaction
		if (
			(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.inputchannelselection.category == category))
		{
			if (format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				bitFieldDiff = (CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.selection() ^ ((jvxSelectionList*)fld)->bitFieldSelected());
				jvxSize last_selection_user_input_channels = JVX_SIZE_INT32(jvx_bitfieldSelection2Id(bitFieldDiff,
					CjvxAudioDevice_genpcg::properties_active.inputchannelselection.value.entries.size()));
				std::vector< oneEntryChannelMapper>::iterator elm = runtime.channelMappings.inputChannelMapper.begin();
				if (JVX_CHECK_SIZE_SELECTED(last_selection_user_input_channels))
				{
					std::advance(elm, last_selection_user_input_channels);
					if (elm != runtime.channelMappings.inputChannelMapper.end())
					{
						elm->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
				}
			}
		}

		if (
			(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.globalIdx == propId) &&
			(CjvxAudioDevice_genpcg::properties_active.outputchannelselection.category == category))
		{
			if (format == JVX_DATAFORMAT_SELECTION_LIST)
			{
				bitFieldDiff = (CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.selection() ^ ((jvxSelectionList*)fld)->bitFieldSelected());
				jvxSize last_selection_user_output_channels = JVX_SIZE_INT32(jvx_bitfieldSelection2Id(bitFieldDiff,
					CjvxAudioDevice_genpcg::properties_active.outputchannelselection.value.entries.size()));
				std::vector< oneEntryChannelMapper>::iterator elm = runtime.channelMappings.outputChannelMapper.begin();
				if (JVX_CHECK_SIZE_SELECTED(last_selection_user_output_channels))
				{
					std::advance(elm, last_selection_user_output_channels);
					if (elm != runtime.channelMappings.outputChannelMapper.end())
					{
						elm->lastModCnt = runtime.channelMappings.modifyWeightCnt++;
					}
				}
			}
		}

		/*
		jvxErrorType res = JVX_MY_BASE_CLASS_D::set_property(callGate, fld, numElements, format, ident, trans);
		if (res == JVX_NO_ERROR)
		{
		*/
		if (
			(genGenericWrapper_device::properties_active.command.globalIdx == propId) &&
			(genGenericWrapper_device::properties_active.command.category == category))
		{
			std::string command = genGenericWrapper_device::properties_active.command.value;
			std::string f_expr;
			std::vector<std::string> args;
			jvxSize i;
			resL = jvx_parseCommandIntoToken(command, f_expr, args);
			if (resL == JVX_NO_ERROR)
			{
				if (f_expr == "reset_channel_history_out")
				{
					for (i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
					{
						runtime.channelMappings.outputChannelMapper[i].lastModCnt = 0;
					}
				}
				if (f_expr == "reset_channel_history_in")
				{
					for (i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
					{
						runtime.channelMappings.inputChannelMapper[i].lastModCnt = 0;
					}
				}
			}
		}
		/*
		if(propId == CjvxAudioDevice_genpcg::properties_active.buffersize.id)
		{
			updateUi = true;
		}
		if(propId == CjvxAudioDevice_genpcg::properties_active.samplerate.id)
		{
			updateUi = true;
		}
		if(propId == CjvxAudioDevice_genpcg::properties_active.format.id)
		{
			updateUi = true;
		}
		if(propId == CjvxAudioDevice_genpcg::properties_active..id)
		{
			updateUi = true;
		}*/
		if (
			(propId == genGenericWrapper_device::properties_active.qualityResamplerIn.globalIdx) && (genGenericWrapper_device::properties_active.qualityResamplerIn.category == category))
		{
			if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() & 0x1))
			{
				this->runtime.props_user.resampler.orderFilterResamplingInput = 32;
			}
			else if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() & 0x2))
			{
				this->runtime.props_user.resampler.orderFilterResamplingInput = 64;
			}
			else if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() & 0x4))
			{
				this->runtime.props_user.resampler.orderFilterResamplingInput = 128;
			}
		}

		if (
			(propId == genGenericWrapper_device::properties_active.qualityResamplerOut.globalIdx) && (genGenericWrapper_device::properties_active.qualityResamplerOut.category == category))
		{
			if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() & 0x1))
			{
				this->runtime.props_user.resampler.orderFilterResamplingOutput = 32;
			}
			else if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() & 0x2))
			{
				this->runtime.props_user.resampler.orderFilterResamplingOutput = 64;
			}
			else if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() & 0x4))
			{
				this->runtime.props_user.resampler.orderFilterResamplingOutput = 128;
			}
		}

		if (
			(
				(CjvxAudioDevice_genpcg::properties_active.activateSilenceStop.globalIdx == propId) &&
				(CjvxAudioDevice_genpcg::properties_active.activateSilenceStop.category == category)) ||
			(
				(CjvxAudioDevice_genpcg::properties_active.periodSilenceStop.globalIdx == propId) &&
				(CjvxAudioDevice_genpcg::properties_active.periodSilenceStop.category == category)))

		{
			// Forward these parameters
			if (onInit.connectedDevice)
			{
				IjvxProperties* theProps = NULL;
				resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
				if ((resL == JVX_NO_ERROR) && theProps)
				{
					res = theProps->set_property(callGate, rawPtr, ident, trans);
					onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
				}
			}
		}

		if (category == JVX_PROPERTY_CATEGORY_PREDEFINED)
		{
			updateUi = true;
		}

		if (updateUi)
		{
			this->lock_settings();
			rearrangeChannelMapper_noLock();
			updateChannelInternal_nolock();
			this->unlock_settings();

			if (_common_set_ld_master.refProc)
			{
				jvxSize uId = JVX_SIZE_UNSELECTED;
				_common_set_ld_master.refProc->unique_id_connections(&uId);
				this->_request_test_chain_master(uId);
			}
		}
	} // if (res == JVX_NO_ERROR)
		else
		{
		if (onInit.connectedDevice)
		{
			/*
				propId -= JVX_PROPERTIES_OFFSET_SUBCOMPONENT;

				IjvxProperties* theProps = NULL;
				resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
				if ((resL == JVX_NO_ERROR) && theProps)
				{
					res = theProps->set_property(callGate, fld, numElements, format, ident, trans);
					onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));

					// Make sure that the fundamental settings are never changed during processing
					this->_lock_state();
					if (_common_set_min.theState <= JVX_STATE_ACTIVE)
					{
						updatePrcParams = true;
					}
					this->_unlock_state();

					if (updatePrcParams)
					{
						this->lock_settings();
						rearrangeChannelMapper_noLock();
						updateChannelInternal_nolock();

						if (_common_set_ld_master.refProc)
						{
							jvxSize uId = JVX_SIZE_UNSELECTED;
							_common_set_ld_master.refProc->unique_id_connections(&uId);
							this->_request_test_chain_master(uId);
						}

						this->unlock_settings();
					}
				}
			*/

			res = forwardPropertyRequest(callGate,
				nullptr, ident,
				[&](IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& locident)
				{
					jvxErrorType res = theProps->set_property(callGate, rawPtr, locident, trans);
					return res;
				},
				nullptr);
			if (res == JVX_NO_ERROR)
			{
				if (_common_set_min.theState == JVX_STATE_ACTIVE)
				{
					this->lock_settings();
					rearrangeChannelMapper_noLock();
					updateChannelInternal_nolock();

					if (_common_set_ld_master.refProc)
					{
						jvxSize uId = JVX_SIZE_UNSELECTED;
						_common_set_ld_master.refProc->unique_id_connections(&uId);
						this->_request_test_chain_master(uId);
					}

					this->unlock_settings();
				}
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::get_property(jvxCallManagerProperties& callGate,
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::get_property(callGate, rawPtr, ident, trans);

	if(res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{		
		if (onInit.connectedDevice)
		{
			res = forwardPropertyRequest(callGate,
				nullptr, ident,
				[&](IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& locident)
				{
					jvxErrorType res = theProps->get_property(callGate, rawPtr, locident, trans);
					return res;
				},
				nullptr);
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::get_property_extended_info(
	jvxCallManagerProperties& callGate,
	jvxHandle* fld, jvxSize requestId,
	const jvx::propertyAddress::IjvxPropertyAddress& ident)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_MY_BASE_CLASS_D::get_property_extended_info(callGate, fld, requestId, ident);

	if (res == JVX_ERROR_ELEMENT_NOT_FOUND)
	{
		if (onInit.connectedDevice)
		{
			res = forwardPropertyRequest(callGate,
				nullptr, ident,
				[&](IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& locident)
				{
					jvxErrorType res = theProps->get_property_extended_info(callGate, fld, requestId, locident);
					return res;
				},
				nullptr);
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::stop_property_group(jvxCallManagerProperties& callGate)
{
	jvxErrorType res = CjvxAudioDevice::stop_property_group(callGate);
	if (res == JVX_NO_ERROR)
	{
		if (onInit.connectedDevice)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			IjvxProperties* theProps = NULL;
			resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
			if ((resL == JVX_NO_ERROR) && theProps)
			{
				jvxCallManagerProperties callGateLoc;
				callGateLoc.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
				res = theProps->stop_property_group(callGateLoc);
				onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
			}
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::start_property_group(jvxCallManagerProperties& callGate)
{
	jvxErrorType res = CjvxAudioDevice::start_property_group(callGate);
	if (res == JVX_NO_ERROR)
	{
		if (onInit.connectedDevice)
		{
			jvxErrorType resL = JVX_NO_ERROR;
			IjvxProperties* theProps = NULL;
			resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
			if ((resL == JVX_NO_ERROR) && theProps)
			{
				jvxCallManagerProperties callGateLoc;
				callGateLoc.call_purpose = JVX_PROPERTY_CALL_PURPOSE_INTERNAL_PASS;
				res = theProps->start_property_group(callGateLoc);
				onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
			}
		}
	}
	return res;
}

jvxErrorType
CjvxGenericWrapperDevice::updateResamplerInputProperties_nolock()
{
	if(runtime.props_user.resampler.orderFilterResamplingInput == 32)
	{
		genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() = 0x1;
	}
	else if(runtime.props_user.resampler.orderFilterResamplingInput == 64)
	{
		genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() = 0x2;
	}
	else if(runtime.props_user.resampler.orderFilterResamplingInput == 128)
	{
		genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() = 0x4;
	}
	else
	{
		genGenericWrapper_device::properties_active.qualityResamplerIn.value.selection() = 0x2;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::updateResamplerOutputProperties_nolock()
{
	if(runtime.props_user.resampler.orderFilterResamplingOutput == 32)
	{
		genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() = 0x1;
	}
	else if(runtime.props_user.resampler.orderFilterResamplingOutput == 64)
	{
		genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() = 0x2;
	}
	else if(runtime.props_user.resampler.orderFilterResamplingOutput == 128)
	{
		genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() = 0x4;
	}
	else
	{
		genGenericWrapper_device::properties_active.qualityResamplerOut.value.selection() = 0x2;
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::forwardPropertyRequest(
	jvxCallManagerProperties& callGate,
	jvx::propertyDescriptor::IjvxPropertyDescriptor* descr,
	const jvx::propertyAddress::IjvxPropertyAddress& idProp,
	std::function<jvxErrorType(IjvxProperties* theProps, const jvx::propertyAddress::IjvxPropertyAddress& ident)> func,
	std::function<void(jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dnms, jvx::propertyDescriptor::CjvxPropertyDescriptorMin* nmin)> funcSuccess)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	IjvxProperties* theProps = NULL;
	jvxErrorType resL = onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if ((resL == JVX_NO_ERROR) && theProps)
	{
		CjvxProperties::propAddressing addresses;
		addresses.cast(&idProp);

		CjvxProperties::propDescriptors descrs;
		descrs.cast(descr);

		if (addresses.addrDescr)
		{
			jPAD cpAddr;
			resL = addresses.prepDescr(cpAddr, "JVX_GENW");
			if (resL == JVX_NO_ERROR)
			{
				res = func(theProps, cpAddr); // theProps->description_property(callGate, descr, cpAddr);
				if (res == JVX_NO_ERROR)
				{
					if (funcSuccess != nullptr)
					{
						funcSuccess(descrs.dFull, descrs.dMin);
					}
				}
			}
		}
		else if (addresses.addrLin)
		{
			jPAL cpLin;

			// Id may target the properties of the sub component
			jvxSize numL = 0;
			resL = JVX_MY_BASE_CLASS_D::number_properties(callGate, &numL);
			assert(resL == JVX_NO_ERROR);
			resL = addresses.prepLin(cpLin, numL);
			if (resL == JVX_NO_ERROR)
			{
				res = func(theProps, cpLin);// theProps->description_property(callGate, descr, cpLin);
				if (res == JVX_NO_ERROR)
				{
					if (funcSuccess != nullptr)
					{
						funcSuccess(descrs.dFull, descrs.dMin);
					}
				}
			}
		}
		else if (addresses.addrIdx)
		{
			jPAGID cpIdx;
			resL = addresses.prepIdx(cpIdx, JVX_PROPERTIES_OFFSET_SUBCOMPONENT);
			if (resL == JVX_NO_ERROR)
			{
				res = func(theProps, cpIdx); // theProps->description_property(callGate, descr, cpIdx);
				if (res == JVX_NO_ERROR)
				{
					if (funcSuccess != nullptr)
					{
						funcSuccess(descrs.dFull, descrs.dMin);
					}
				}
			}
		}
		else
		{
			assert(0);
		}
		onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperDevice, set_channel_gain)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.inputChannelGain))
	{
		if (channelSelectIndex < runtime.channelMappings.inputChannelMapper.size())
		{
			runtime.channelMappings.inputChannelMapper[channelSelectIndex].gain =
				genGenericWrapper_device::properties_active_higher.inputChannelGain.value;

			// Pass to processing engine
			if (this->processingControl.levels.input_gain && (channelSelectIndex < this->processingControl.levels.num_entries_input))
			{
				this->processingControl.levels.input_gain[channelSelectIndex] = runtime.channelMappings.inputChannelMapper[channelSelectIndex].gain;
			}
		}
		tune.offsetStart = channelSelectIndex;
		channelSelectIndex = JVX_SIZE_UNSELECTED;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.outputChannelGain))
	{
		if (channelSelectIndex < runtime.channelMappings.outputChannelMapper.size())
		{
			runtime.channelMappings.outputChannelMapper[channelSelectIndex].gain =
				genGenericWrapper_device::properties_active_higher.outputChannelGain.value;

			// Pass to processing engine
			if (this->processingControl.levels.output_gain && (channelSelectIndex < this->processingControl.levels.num_entries_output))
			{
				this->processingControl.levels.output_gain[channelSelectIndex] = runtime.channelMappings.outputChannelMapper[channelSelectIndex].gain;
			}
		}
		tune.offsetStart = channelSelectIndex;
		channelSelectIndex = JVX_SIZE_UNSELECTED;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.directIn))
	{
		if (channelSelectIndex < runtime.channelMappings.inputChannelMapper.size())
		{
			runtime.channelMappings.inputChannelMapper[channelSelectIndex].directIn =
				genGenericWrapper_device::properties_active_higher.directIn.value;

			// Pass to processing engine
			if (this->processingControl.levels.direct && (channelSelectIndex < this->processingControl.levels.num_entries_output))
			{
				this->processingControl.levels.direct[channelSelectIndex] = runtime.channelMappings.inputChannelMapper[channelSelectIndex].directIn;
			}
		}
		tune.offsetStart = channelSelectIndex;
		channelSelectIndex = JVX_SIZE_UNSELECTED;
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperDevice, get_channel_gain)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.inputChannelGain))
	{
		if (tune.offsetStart< runtime.channelMappings.inputChannelMapper.size())
		{
			genGenericWrapper_device::properties_active_higher.inputChannelGain.value =
				runtime.channelMappings.inputChannelMapper[tune.offsetStart].gain;
			tune.offsetStart = 0;
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.outputChannelGain))
	{
		if (tune.offsetStart < runtime.channelMappings.outputChannelMapper.size())
		{
			genGenericWrapper_device::properties_active_higher.outputChannelGain.value =
				runtime.channelMappings.outputChannelMapper[tune.offsetStart].gain;
			tune.offsetStart = 0;
		}
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.directIn))
	{
		if (tune.offsetStart < runtime.channelMappings.inputChannelMapper.size())
		{
			genGenericWrapper_device::properties_active_higher.directIn.value =
				runtime.channelMappings.inputChannelMapper[tune.offsetStart].directIn;
			tune.offsetStart = 0;
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperDevice, set_channel_index)
{
	if (
		(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.inputChannelGain))||
		(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.directIn)) ||
		(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genGenericWrapper_device::properties_active_higher.outputChannelGain)))
	{
		channelSelectIndex = tune.offsetStart;
		tune.offsetStart = 0;
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperDevice, get_device_cap_avail)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxProperties* theProps = NULL;
	if (onInit.connectedDevice)
	{
		onInit.connectedDevice->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
		if (theProps)
		{
			res = theProps->get_property(callGate, *rawPtr, 
				jPAGID(ident.id, ident.cat), jPD(false, tune.offsetStart));
			onInit.connectedDevice->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(theProps));
			
			if (res == JVX_NO_ERROR)
			{
				// This concludes this "get": By returning ABORT the calling get__property is immediately completed since all tests have been
				// done in the get_property function here
				res = JVX_ERROR_ABORT;
			}
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxGenericWrapperDevice, get_device_sslot)
{
	// CjvxDeviceCaps_genpcg::device_caps.dev_sslot.value = _common_set.theComponentType.slotsubid;
	return JVX_NO_ERROR;
}

