#include "CjvxSpNMeasureIr.h"

// ===================================================================
// ===================================================================

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_config_posthook)
{
	jvxBool updateUi = false;

	// Allow modification of input output channels
	/*
	if(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::config.number_input_channels_max))
	{
		updateUi = true;

		neg_input._update_parameters_fixed(
			genMeasureIr_node::config.number_input_channels_max.value);

		// Update will be troggered from chain update
		reconstruct_properties(selMeasures, false);
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::config.number_output_channels_max))
	{
		updateUi = true;

		neg_output._update_parameters_fixed(
			genMeasureIr_node::config.number_output_channels_max.value);
		
		// Update will be troggered from chain update
		reconstruct_properties(selMeasures, false);
	}
	*/

	inform_chain_test();

	if (updateUi)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
		CjvxObject::_report_command_request(prio);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_generator_posthook)
{
	// Copy the settings to the proper measurement
	if (selMeasures < measurements.size())
	{
		auto elm = measurements.begin();
		if (genMeasureIr_node::config.apply_to_all.value == c_true)
		{
			for (; elm != measurements.end(); elm++)
			{
				generator_from_props(*elm);
			}
		}
		else
		{
			std::advance(elm, selMeasures);
			generator_from_props(*elm);
		}
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_measurement_posthook)
{
	jvxBool updateChannels = false;
	jvxBool updateUi = false;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.all_measurements))
	{
		selMeasures = jvx_bitfieldSelection2Id(genMeasureIr_node::measurements.all_measurements.value.selection(),
			genMeasureIr_node::measurements.all_measurements.value.entries.size());
		set_measurement_idx();
		updateUi = true;
		updateChannels = true;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_description))
	{
		if (selMeasures < measurements.size())
		{
			auto elm = measurements.begin();
			std::advance(elm, selMeasures);
			(*elm)->description = correctNameAndSet(genMeasureIr_node::measurements.measurement_description.value, *elm);
			reconstruct_properties(selMeasures);
		}
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_in_channels))
	{
		channels_from_properties();
		updateChannels = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_out_channels))
	{
		channels_from_properties();
		updateChannels = true;
	}

	if (updateChannels)
	{
		channels_to_properties();
	}

	if (updateUi)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
		CjvxObject::_report_command_request(prio);
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, get_measurement_prehook)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, add_entry)
{
	if(JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_add_entry))
	{
		CjvxSpNMeasureIr_oneMeasurement* meas = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(meas, CjvxSpNMeasureIr_oneMeasurement);
		std::string txtName = JVX_COMPOSE_MEASUREMENT(measurements.size());
		
		meas->description = correctNameAndSet(txtName, NULL);

		jvxSize selNew = measurements.size();

		if (JVX_CHECK_SIZE_SELECTED(selMeasures))
		{
			if (selMeasures < measurements.size())
			{
				auto elm = measurements.begin();
				std::advance(elm, selMeasures);
				meas->generator = (*elm)->generator;
			}
		}

		measurements.push_back(meas);

		/*
		jvx_bitfieldSelection2Id(
			genMeasureIr_node::measurements.all_measurements.value.selection(),
			genMeasureIr_node::measurements.all_measurements.value.entries.size());
		*/

		reconstruct_properties(selNew);
		genMeasureIr_node::measurements.measurement_add_entry.value = c_false;

		set_measurement_idx();
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, remove_entry)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_rem_entry))
	{
		if (JVX_CHECK_SIZE_SELECTED(selMeasures))
		{
			auto elm = measurements.begin();
			std::advance(elm, selMeasures);
			JVX_DSP_SAFE_DELETE_OBJECT(*elm);
			measurements.erase(elm);
			reconstruct_properties(selMeasures - 1);
		}
		genMeasureIr_node::measurements.measurement_rem_entry.value = c_false;
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, trigger_start)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::control.trigger_start))
	{
		inProcessing.do_start = true;
		genMeasureIr_node::control.trigger_start.value = false;
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_all_channels_posthook)
{
	jvxBool allInOn = false;
	jvxBool allInOff = false;
	jvxBool allOutOn = false;
	jvxBool allOutOff = false;

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_all_in_channels_on))
	{
		allInOn = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_all_in_channels_off))
	{
		allInOff = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_all_out_channels_on))
	{
		allOutOn = true;
	}
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::measurements.measurement_all_out_channels_off))
	{
		allOutOff = true;
	}

	// ===============================================================================

	auto elm = measurements.begin();
	if (genMeasureIr_node::config.apply_to_all.value == c_true)
	{
		for (; elm != measurements.end(); elm++)
		{
			allInPutOnOff(*elm, allInOn, allInOff, allOutOn, allOutOff);
		}
	}
	else
	{
		if (selMeasures < measurements.size())
		{
			std::advance(elm, selMeasures);
			allInPutOnOff(*elm, allInOn, allInOff, allOutOn, allOutOff);
		}
	}

	channels_to_properties();

	jvxCBitField prio = 0;
	jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
	CjvxObject::_report_command_request(prio);

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, get_monitor_prehook)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_evaluation_posthook)
{
	if (selMeasures < measurements.size())
	{
		auto elm = measurements.begin();
		if (genMeasureIr_node::config.apply_to_all.value == c_true)
		{
			for (; elm != measurements.end(); elm++)
			{
				evaluation_from_props(*elm);
			}
		}
		else
		{
			std::advance(elm, selMeasures);
			evaluation_from_props(*elm);
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, get_measurement_data_prehook)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	const jPRG* ptr = castPropRawPointer<const jPRG>(rawPtr, JVX_DATAFORMAT_NONE);
	assert(ptr);
	oneMeasurementChannel* ptrIn = (oneMeasurementChannel*)ptr->raw();
	if (ptrIn)
	{
		jvxSize selResMeas = jvx_bitfieldSelection2Id(genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());
		jvxSize selResMeasChan = jvx_bitfieldSelection2Id(genMeasureIr_node::results.stored_measurement_channels.value.selection(),
			genMeasureIr_node::results.stored_measurement_channels.value.entries.size());

		std::string nmMeas = ptrIn->nmMeas.std_str();
		std::string nmChan = ptrIn->nmChan.std_str();

		if (!nmMeas.empty())
		{
			selResMeas = JVX_SIZE_UNSELECTED;
			for (i = 0; i < genMeasureIr_node::results.stored_measurements.value.entries.size(); i++)
			{
				if (nmMeas == genMeasureIr_node::results.stored_measurements.value.entries[i])
				{
					selResMeas = i;
					break;
				}
			}
		}

		if (!nmChan.empty())
		{
			selResMeasChan = JVX_SIZE_UNSELECTED;
			for (i = 0; i < genMeasureIr_node::results.stored_measurement_channels.value.entries.size(); i++)
			{
				if (nmChan == genMeasureIr_node::results.stored_measurement_channels.value.entries[i])
				{
					selResMeasChan = i;
					break;
				}
			}
		}

		if (
			JVX_CHECK_SIZE_UNSELECTED(selResMeasChan) ||
			JVX_CHECK_SIZE_UNSELECTED(selResMeas))
		{
			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		auto elmM = collectedResults.begin();
		if (selResMeas < collectedResults.size())
		{
			std::advance(elmM, selResMeas);

			auto elmC = elmM->resultsChannel.begin();
			if (selResMeasChan < elmM->resultsChannel.size())
			{
				std::advance(elmC, selResMeasChan);

				if (ptrIn->lBuf == 0)
				{
					ptrIn->lBuf = elmC->lBuf;
				}
				else
				{
					ptrIn->validFlds = 0;
					ptrIn->rate = elmC->rate;
					ptrIn->fileLocate.assign(elmM->pathStoreProto);
					jvxSize ml = JVX_MIN(ptrIn->lBuf, elmC->lBuf);
					if (ptrIn->bufIr)
					{
						if (elmC->validFlds & JVX_FLAG_IR_VALID)
						{
							memcpy(ptrIn->bufIr, elmC->bufIr, sizeof(jvxData)*ml);
							ptrIn->validFlds |= JVX_FLAG_IR_VALID;
						}
					}
					if (ptrIn->bufMeas)
					{
						if (elmC->validFlds & JVX_FLAG_MEAS_VALID)
						{
							memcpy(ptrIn->bufMeas, elmC->bufMeas, sizeof(jvxData)*ml);
							ptrIn->validFlds |= JVX_FLAG_MEAS_VALID;
						}
					}
					if (ptrIn->bufTest)
					{
						if (elmC->validFlds & JVX_FLAG_TEST_VALID)
						{
							memcpy(ptrIn->bufTest, elmC->bufTest, sizeof(jvxData)*ml);
							ptrIn->validFlds |= JVX_FLAG_TEST_VALID;
						}
					}
				}
				ptrIn->nmChan.assign(elmC->nmChan.std_str());
				ptrIn->nmMeas.assign(
					genMeasureIr_node::results.stored_measurements.value.entries[selResMeas]);
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_result_import_measurement)
{
	jvxBool updateUi = false;
	std::string fName = genMeasureIr_node::results.import_measurement.value;

	read_result(fName, updateUi);

	if (updateUi)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
		CjvxObject::_report_command_request(prio);
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_result_posthook)
{
	jvxBool updateUi = false;
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::results.stored_measurements))
	{
		jvxSize idSel = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());

		this->update_plot_channels(idSel);
		updateUi = true;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::results.trigger_erase_channel))
	{
		jvxSize idSelM = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());
		jvxSize idSelC = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurement_channels.value.selection(),
			genMeasureIr_node::results.stored_measurement_channels.value.entries.size());

		if (idSelM < collectedResults.size())
		{
			auto elm = collectedResults.begin();
			std::advance(elm, idSelM);

			if (idSelC < elm->resultsChannel.size())
			{
				auto elmC = elm->resultsChannel.begin();
				std::advance(elm, idSelC);
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufIr);
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufMeas);
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufTest);
				elm->resultsChannel.erase(elmC);
			}
			if (elm->resultsChannel.size() == 0)
			{
				collectedResults.erase(elm);
			}
		}

		update_plot_measurements();
		genMeasureIr_node::results.trigger_erase_channel.value = c_false;
		updateUi = true;
	}

	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::results.trigger_erase_measurement))
	{
		jvxSize idSelM = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());

		if (idSelM < collectedResults.size())
		{
			auto elm = collectedResults.begin();
			std::advance(elm, idSelM);

			auto elmC = elm->resultsChannel.begin();
			for (; elmC != elm->resultsChannel.end(); elmC++)
			{
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufIr);
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufMeas);
				JVX_DSP_SAFE_DELETE_FIELD(elmC->bufTest);
			}
			elm->resultsChannel.clear();

			collectedResults.erase(elm);
		}

		update_plot_measurements();
		genMeasureIr_node::results.trigger_erase_measurement.value = c_false;
		updateUi = true;
	}

	if (updateUi)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT);
		CjvxObject::_report_command_request(prio);
	}

	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, set_config_measurement_posthook)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::results.store_measurement_config))
	{
		jvxSize idSelM = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());

		if (idSelM < collectedResults.size())
		{
			auto elm = collectedResults.begin();
			std::advance(elm, idSelM);
			elm->storeConfig = genMeasureIr_node::results.store_measurement_config.value;
		}
	}
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSpNMeasureIr, get_config_measurement_prehook)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genMeasureIr_node::results.store_measurement_config))
	{
		jvxSize idSelM = jvx_bitfieldSelection2Id(
			genMeasureIr_node::results.stored_measurements.value.selection(),
			genMeasureIr_node::results.stored_measurements.value.entries.size());

		if (idSelM < collectedResults.size())
		{
			auto elm = collectedResults.begin();
			std::advance(elm, idSelM);
			genMeasureIr_node::results.store_measurement_config.value = elm->storeConfig;
		}
	}
	return JVX_NO_ERROR;
}
