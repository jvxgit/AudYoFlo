#include "CjvxSpNMixChainEnterLeave.h"

void
CjvxSpNMixChainEnterLeave::offset_channels_to_property()
{
	genMixChain::sources_channel_routing.all_definitions.value.entries.clear();
	jvx_bitFClear(genMixChain::sources_channel_routing.all_definitions.value.selection(0));

	for (auto& elm : presets_channel_routing)
	{
		genMixChain::sources_channel_routing.all_definitions.value.entries.push_back(jvxComponentIdentification_txt(elm.first));
	}
	if (presets_channel_routing.size())
	{
		jvx_bitZSet(genMixChain::sources_channel_routing.all_definitions.value.selection(0), 0);
	}

	CjvxProperties::add_property_report_collect(genMixChain::sources_channel_routing.all_definitions.descriptor.std_str(), true);
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMixChainEnterLeave, set_on_config)
{
	jvxBool triggerTest = false;
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.number_channels_side))
	{
		// New number of channels, run the test function
		triggerTest = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.operation_mode))
	{
		operationMode = genMixChain::translate__config__operation_mode_from();
		triggerTest = true;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT_SIMPLE(genMixChain::config.channel_routing))
	{
		// Correct channel order
		correct_order_channel_route(ptrChannelRoutes, szChannelRoutes);

		// Update channel routings
		if (szChannelRoutes)
		{
			memcpy(oldRouting.data(), ptrChannelRoutes, szChannelRoutes * sizeof(jvxSize));
		}

		operationMode = genMixChain::translate__config__operation_mode_from();
		triggerTest = true;
	}

	if (triggerTest)
	{
		this->inform_chain_test();
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMixChainEnterLeave, specify_one_definition)
{
	std::vector<std::string> lst;
	jvx_parseCommandLineOneToken(genMixChain::sources_channel_routing.one_new_entry.value, lst, ',');
	if (lst.size() >= 3)
	{
		chanOffsetAndMaxChans newElm;
		bool err = false;
		auto res = jvxComponentIdentification_decode(newElm.cpId, lst[0]);
		if (res != JVX_NO_ERROR)
		{
			err = true;
		}

		if(!err)
		{
			newElm.idxOffset = jvx_string2Size(lst[1], err);
		}

		if (!err)
		{
			newElm.channel_num = jvx_string2Size(lst[2], err);
		}

		if (!err)
		{
			if (lst.size() > 3)
			{
				newElm.deviceChannelPrefix = lst[3];
			}
		}

		if (!err)
		{
			auto elmI = presets_channel_routing.find(newElm.cpId);
			if (elmI == presets_channel_routing.end())
			{
				presets_channel_routing[newElm.cpId] = newElm;
				genMixChain::sources_channel_routing.last_error.value = jvxErrorType_txt(JVX_NO_ERROR);
				offset_channels_to_property();				
			}
			else
			{
				genMixChain::sources_channel_routing.last_error.value = jvxErrorType_txt(JVX_ERROR_DUPLICATE_ENTRY);
			}
		}
		else
		{
			genMixChain::sources_channel_routing.last_error.value = jvxErrorType_txt(JVX_ERROR_PARSE_ERROR);
		}
	}
	else
	{
		genMixChain::sources_channel_routing.last_error.value = jvxErrorType_txt(JVX_ERROR_INVALID_FORMAT);
	}
	CjvxProperties::add_property_report_collect(genMixChain::sources_channel_routing.last_error.descriptor.std_str(), false);
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMixChainEnterLeave, remove_one_definition)
{
	jvxSize idx = jvx_bitfieldSelection2Id(genMixChain::sources_channel_routing.all_definitions);
	if (JVX_CHECK_SIZE_SELECTED(idx))
	{
		if (idx < presets_channel_routing.size())
		{
			auto itElm = presets_channel_routing.begin();
			std::advance(itElm, idx);

			presets_channel_routing.erase(itElm);
			genMixChain::sources_channel_routing.last_error.value = jvxErrorType_txt(JVX_NO_ERROR);
			CjvxProperties::add_property_report_collect(genMixChain::sources_channel_routing.last_error.descriptor.std_str(), false);

			offset_channels_to_property();
		}
	}
	return JVX_NO_ERROR;
}
