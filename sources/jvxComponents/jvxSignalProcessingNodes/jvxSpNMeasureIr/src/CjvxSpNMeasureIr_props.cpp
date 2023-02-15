#include "CjvxSpNMeasureIr.h"

// ==============================================================================
// ==============================================================================

void
CjvxSpNMeasureIr::set_measurement_idx()
{
	auto elm = measurements.begin();
	if (selMeasures < measurements.size())
	{
		// Update the generator fields
		std::advance(elm, selMeasures);
		generator_to_props((*elm));
		evaluation_to_props((*elm));
		genMeasureIr_node::measurements.measurement_description.value = (*elm)->description;
	}
}

void 
CjvxSpNMeasureIr::reconstruct_properties(jvxSize selMeasuresNew, jvxBool triggerUpdate)
{
	jvxSize i;

	genMeasureIr_node::measurements.all_measurements.value.entries.clear();
	auto elm = measurements.begin();
	for (; elm != measurements.end(); elm++)
	{
		genMeasureIr_node::measurements.all_measurements.value.entries.push_back((*elm)->description);
	}

	selMeasures = selMeasuresNew;
	if (selMeasures >= genMeasureIr_node::measurements.all_measurements.value.entries.size())
	{
		selMeasures = JVX_SIZE_UNSELECTED;
	}
	if (JVX_CHECK_SIZE_SELECTED(selMeasures))
	{		
		jvx_bitZSet(genMeasureIr_node::measurements.all_measurements.value.selection(), selMeasures);
		genMeasureIr_node::measurements.measurement_description.value = genMeasureIr_node::measurements.all_measurements.value.entries[selMeasures];
	}
	else
	{
		if (genMeasureIr_node::measurements.all_measurements.value.entries.size())
		{
			selMeasures = 0;
			jvx_bitZSet(genMeasureIr_node::measurements.all_measurements.value.selection(), 0);
			genMeasureIr_node::measurements.measurement_description.value = genMeasureIr_node::measurements.all_measurements.value.entries[0];
		}
		else
		{
			genMeasureIr_node::measurements.measurement_description.value = "--";
		}
	}

	// ==============================================================================

	genMeasureIr_node::measurements.measurement_in_channels.value.entries.clear();
	if (JVX_CHECK_SIZE_SELECTED(selMeasures))
	{
		if (JVX_CHECK_SIZE_SELECTED(node_inout._common_set_node_params_a_1io.number_channels))
		{
			for (i = 0; i < node_inout._common_set_node_params_a_1io.number_channels; i++)
			{
				std::string txt = "Capture Channel #" + jvx_size2String(i);
				genMeasureIr_node::measurements.measurement_in_channels.value.entries.push_back(txt);
			}
		}
	}

	// ==============================================================================

	genMeasureIr_node::measurements.measurement_out_channels.value.entries.clear();
	if (JVX_CHECK_SIZE_SELECTED(selMeasures))
	{
		if (JVX_CHECK_SIZE_SELECTED(node_inout._common_set_node_params_a_1io.number_channels))
		{
			for (i = 0; i < node_inout._common_set_node_params_a_1io.number_channels; i++)
			{
				std::string txt = "Rendering Channel #" + jvx_size2String(i);
				genMeasureIr_node::measurements.measurement_out_channels.value.entries.push_back(txt);
			}
		}
	}

	channels_to_properties();	

	// ==============================================================================

	update_generator_properties();

	// ==============================================================================

	if (triggerUpdate)
	{
		jvxCBitField prio = 0;
		jvx_bitZSet(prio, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		CjvxObject::_report_command_request(prio);
	}
}

void
CjvxSpNMeasureIr::channels_from_properties()
{
	jvxSize i;

	if (JVX_CHECK_SIZE_SELECTED(selMeasures))
	{
		auto elm = measurements.begin();
		if (selMeasures < measurements.size())
		{
			std::advance(elm, selMeasures);

			// ============================================================================
			// Align output channels
			// ============================================================================

			for (i = 0; i < genMeasureIr_node::measurements.measurement_out_channels.value.entries.size(); i++)
			{
				if (jvx_bitTest(genMeasureIr_node::measurements.measurement_out_channels.value.selection(), i))
				{
					auto elmO = (*elm)->outputs.find(i);
					if (elmO == (*elm)->outputs.end())
					{
						oneOutput addOut;
						addOut.active = true;
						addOut.id = i;
						(*elm)->outputs[addOut.id] = addOut;
					}
					else
					{
						// THere is an entry already
					}
				}
				else
				{
					auto elmO = (*elm)->outputs.find(i);
					if (elmO != (*elm)->outputs.end())
					{
						(*elm)->outputs.erase(elmO);
					}
				}

			}

			// ============================================================================
			// Align input channels
			// ============================================================================
			for (i = 0; i < genMeasureIr_node::measurements.measurement_in_channels.value.entries.size(); i++)
			{
				if (jvx_bitTest(genMeasureIr_node::measurements.measurement_in_channels.value.selection(), i))
				{
					auto elmI = (*elm)->inputs.find(i);
					if (elmI == (*elm)->inputs.end())
					{
						oneInput addIn;
						addIn.active = true;
						addIn.id = i;
						(*elm)->inputs[addIn.id] = addIn;
					}
					else
					{
						// THere is an entry already
					}
				}
				else
				{
					auto elmI = (*elm)->inputs.find(i);
					if (elmI != (*elm)->inputs.end())
					{
						(*elm)->inputs.erase(elmI);
					}
				}

			}
		}
	}
}

	
void
CjvxSpNMeasureIr::channels_to_properties()
{
	jvx_bitFClear(genMeasureIr_node::measurements.measurement_in_channels.value.selection());
	jvx_bitFClear(genMeasureIr_node::measurements.measurement_out_channels.value.selection());
	
	if(selMeasures < measurements.size())
	{
		auto elmI = measurements.begin();
		std::advance(elmI, selMeasures);

		auto elmCI = (*elmI)->inputs.begin();
		for (; elmCI != (*elmI)->inputs.end(); elmCI++)
		{
			// There might be more entries in the list than in properties which resulted from previous setups
			if (elmCI->first < node_inout._common_set_node_params_a_1io.number_channels)
			{
				jvx_bitSet(genMeasureIr_node::measurements.measurement_in_channels.value.selection(), elmCI->first);
			}
		}
		genMeasureIr_node::measurements.measurement_in_channels.isValid = true;
		genMeasureIr_node::measurements.measurement_all_in_channels_on.isValid = true;
		genMeasureIr_node::measurements.measurement_all_in_channels_off.isValid = true;

		auto elmCO = (*elmI)->outputs.begin();
		for (; elmCO != (*elmI)->outputs.end(); elmCO++)
		{
			// There might be more entries in the list than in properties which resulted from previous setups
			if (elmCO->first < node_output._common_set_node_params_a_1io.number_channels)
			{
				jvx_bitSet(genMeasureIr_node::measurements.measurement_out_channels.value.selection(), elmCO->first);
			}
		}
		genMeasureIr_node::measurements.measurement_out_channels.isValid = true;
		genMeasureIr_node::measurements.measurement_all_out_channels_on.isValid = true;
		genMeasureIr_node::measurements.measurement_all_out_channels_off.isValid = true;
	}
	else
	{
		genMeasureIr_node::measurements.measurement_in_channels.isValid = false;
		genMeasureIr_node::measurements.measurement_all_in_channels_on.isValid = false;
		genMeasureIr_node::measurements.measurement_all_in_channels_off.isValid = false;

		genMeasureIr_node::measurements.measurement_out_channels.isValid = false;
		genMeasureIr_node::measurements.measurement_all_out_channels_on.isValid = false;
		genMeasureIr_node::measurements.measurement_all_out_channels_off.isValid = false;
	}
}

void
CjvxSpNMeasureIr::update_generator_properties()
{
	if (JVX_CHECK_SIZE_SELECTED(selMeasures))
	{
		genMeasureIr_node::generator.type_excitation.isValid = true;
		genMeasureIr_node::generator.noise_type_excitation.isValid = true;
		genMeasureIr_node::generator.amplitude_excitation.isValid = true;
		genMeasureIr_node::generator.frequency_low_excitation.isValid = true;
		genMeasureIr_node::generator.frequency_up_excitation.isValid = true;
		genMeasureIr_node::generator.length_excitation.isValid = true;
		genMeasureIr_node::generator.number_periods_excitation.isValid = true;
		genMeasureIr_node::generator.seed_excitation.isValid = true;
		genMeasureIr_node::generator.silence_start_excitation.isValid = true;
		genMeasureIr_node::generator.silence_stop_excitation.isValid = true;

	}
	else
	{
		genMeasureIr_node::generator.type_excitation.isValid = false;
		genMeasureIr_node::generator.noise_type_excitation.isValid = false;
		genMeasureIr_node::generator.amplitude_excitation.isValid = false;
		genMeasureIr_node::generator.frequency_low_excitation.isValid = false;
		genMeasureIr_node::generator.frequency_up_excitation.isValid = false;
		genMeasureIr_node::generator.length_excitation.isValid = false;
		genMeasureIr_node::generator.number_periods_excitation.isValid = false;
		genMeasureIr_node::generator.seed_excitation.isValid = false;
		genMeasureIr_node::generator.silence_start_excitation.isValid = false;
		genMeasureIr_node::generator.silence_stop_excitation.isValid = false;
	}
}

// ==============================================================================

void
CjvxSpNMeasureIr::generator_from_props(CjvxSpNMeasureIr_oneMeasurement* elm)
{
	elm->generator.seed_noise = genMeasureIr_node::generator.seed_excitation.value;
	elm->generator.amplitude = genMeasureIr_node::generator.amplitude_excitation.value;
	elm->generator.freq_low_hz = genMeasureIr_node::generator.frequency_low_excitation.value;
	elm->generator.freq_up_hz = genMeasureIr_node::generator.frequency_up_excitation.value;
	elm->generator.length_seconds = genMeasureIr_node::generator.length_excitation.value;
	elm->generator.silence_start_seconds = genMeasureIr_node::generator.silence_start_excitation.value;
	elm->generator.silence_stop_seconds = genMeasureIr_node::generator.silence_stop_excitation.value;
	elm->generator.loop_count = genMeasureIr_node::generator.number_periods_excitation.value;
	elm->generator.tpWave = genMeasureIr_node::translate__generator__type_excitation_from();
	elm->generator.noise_type = genMeasureIr_node::translate__generator__noise_type_excitation_from();
}

void
CjvxSpNMeasureIr::generator_to_props(CjvxSpNMeasureIr_oneMeasurement* elm)
{
	genMeasureIr_node::generator.seed_excitation.value = elm->generator.seed_noise;
	genMeasureIr_node::generator.amplitude_excitation.value = elm->generator.amplitude;
	genMeasureIr_node::generator.frequency_low_excitation.value = elm->generator.freq_low_hz;
	genMeasureIr_node::generator.frequency_up_excitation.value = elm->generator.freq_up_hz;
	genMeasureIr_node::generator.length_excitation.value = elm->generator.length_seconds;
	genMeasureIr_node::generator.silence_start_excitation.value = elm->generator.silence_start_seconds;
	genMeasureIr_node::generator.silence_stop_excitation.value = elm->generator.silence_stop_seconds;
	genMeasureIr_node::generator.number_periods_excitation.value = (jvxInt32)elm->generator.loop_count;
	genMeasureIr_node::translate__generator__type_excitation_to(elm->generator.tpWave);
	genMeasureIr_node::translate__generator__noise_type_excitation_to(elm->generator.noise_type);
}

// ==============================================================================

void
CjvxSpNMeasureIr::evaluation_from_props(CjvxSpNMeasureIr_oneMeasurement* elm)
{
	elm->evaluation.derive_ir = genMeasureIr_node::evaluation.evaluate_ir.value;
	elm->evaluation.skip_eval = genMeasureIr_node::evaluation.skip_periods.value;
	elm->evaluation.store_data = genMeasureIr_node::evaluation.store_data.value;
}

void
CjvxSpNMeasureIr::evaluation_to_props(CjvxSpNMeasureIr_oneMeasurement* elm)
{
	genMeasureIr_node::evaluation.evaluate_ir.value = elm->evaluation.derive_ir;
	genMeasureIr_node::evaluation.skip_periods.value = elm->evaluation.skip_eval;
	genMeasureIr_node::evaluation.store_data.value = elm->evaluation.store_data;
}

// ==============================================================================

void
CjvxSpNMeasureIr::allInPutOnOff(CjvxSpNMeasureIr_oneMeasurement* elm, jvxBool allInOn, jvxBool allInOff, jvxBool allOutOn, jvxBool allOutOff)
{
	jvxSize i;
	if (allInOn || allInOff)
	{
		elm->inputs.clear();
		if (allInOn)
		{
			for (i = 0; i < node_inout._common_set_node_params_a_1io.number_channels; i++)
			{
				oneInput newIn;
				newIn.active = true;
				newIn.id = i;
				elm->inputs[i] = newIn;
			}
		}
	}
	if (allOutOn || allOutOff)
	{
		elm->outputs.clear();
		if (allOutOn)
		{
			for (i = 0; i < node_output._common_set_node_params_a_1io.number_channels; i++)
			{
				oneOutput newOut;
				newOut.active = true;
				newOut.id = i;
				elm->outputs[i] = newOut;
			}
		}
	}
}

std::string
CjvxSpNMeasureIr::correctNameAndSet(std::string nm, CjvxSpNMeasureIr_oneMeasurement* elm)
{
	jvxBool existsalready = true;
	while (existsalready)
	{
		existsalready = false;
		auto elmF = measurements.begin();
		for (; elmF != measurements.end(); elmF++)
		{
			if (elm != *elmF)
			{
				if ((*elmF)->description == nm)
				{
					nm += "_";
					existsalready = true;
					break;
				}
			}
		}
	}
	return nm;
}

// ================================================================================

void
CjvxSpNMeasureIr::update_plot_measurements()
{
	std::string nmSel;
	jvxSize cnt = 0;
	jvxSize idSelB4 = JVX_SIZE_UNSELECTED;

	jvxSize selId = jvx_bitfieldSelection2Id(
		genMeasureIr_node::results.stored_measurements.value.selection(),
		genMeasureIr_node::results.stored_measurements.value.entries.size());

	if (selId < genMeasureIr_node::results.stored_measurements.value.entries.size())
	{
		nmSel = genMeasureIr_node::results.stored_measurements.value.entries[selId];
	}

	genMeasureIr_node::results.stored_measurements.value.entries.clear();
	auto elm = this->collectedResults.begin();
	for (; elm != collectedResults.end(); elm++)
	{
		std::string nmMeasure = elm->nameMeasure + " [" + elm->tokenMeasure + "]";
		genMeasureIr_node::results.stored_measurements.value.entries.push_back(nmMeasure);
		if (elm->nameMeasure == nmSel)
		{
			idSelB4 = cnt;
		}
		cnt++;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(idSelB4))
	{
		if (genMeasureIr_node::results.stored_measurements.value.entries.size())
		{
			idSelB4 = 0;
		}
	}


	if (JVX_CHECK_SIZE_SELECTED(idSelB4))
	{
		jvx_bitZSet(genMeasureIr_node::results.stored_measurements.value.selection(), idSelB4);
	}
	update_plot_channels(idSelB4);
}

void
CjvxSpNMeasureIr::update_plot_channels(jvxSize idSelMeas)
{
	std::string nmSel;
	jvxSize cnt = 0;
	jvxSize idSelB4 = JVX_SIZE_UNSELECTED;
	if (idSelMeas < this->collectedResults.size())
	{
		auto elm = this->collectedResults.begin();
		std::advance(elm, idSelMeas);

		nmSel = "";
		idSelB4 = jvx_bitfieldSelection2Id(genMeasureIr_node::results.stored_measurement_channels.value.selection(),
			genMeasureIr_node::results.stored_measurement_channels.value.entries.size());

		if (idSelB4 < genMeasureIr_node::results.stored_measurement_channels.value.entries.size())
		{
			nmSel = genMeasureIr_node::results.stored_measurement_channels.value.entries[idSelB4];
		}
		genMeasureIr_node::results.stored_measurement_channels.value.entries.clear();
		cnt = 0;
		auto elmC = elm->resultsChannel.begin();
		for (; elmC != elm->resultsChannel.end(); elmC++)
		{
			genMeasureIr_node::results.stored_measurement_channels.value.entries.push_back(elmC->nmChan.std_str());
			if (elmC->nmChan.std_str() == nmSel)
			{
				idSelB4 = cnt;
			}
			cnt++;
		}

		if (JVX_CHECK_SIZE_UNSELECTED(idSelB4))
		{
			if (genMeasureIr_node::results.stored_measurement_channels.value.entries.size())
			{
				idSelB4 = 0;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(idSelB4))
		{
			jvx_bitZSet(genMeasureIr_node::results.stored_measurement_channels.value.selection(), idSelB4);
		}
	}
	else
	{
		genMeasureIr_node::results.stored_measurement_channels.value.entries.clear();
		jvx_bitFClear(genMeasureIr_node::results.stored_measurement_channels.value.selection());
	}
}

void
CjvxSpNMeasureIr::read_result(const std::string fName, jvxBool& updateUi)
{
	oneMeasurementResult oneResult;
	IjvxRtAudioFileReader* theReader = NULL;
	IjvxConfigProcessor* confHdl = NULL;
	IjvxObject* confObj = NULL;
	IjvxObject* readObj = NULL;

	_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &confObj, 0, NULL);
	if (confObj)
	{
		confObj->request_specialization(reinterpret_cast<jvxHandle**>(&confHdl), NULL, NULL);
	}
	_common_set.theToolsHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, &readObj, 0, NULL);
	if (readObj)
	{
		readObj->request_specialization(reinterpret_cast<jvxHandle**>(&theReader), NULL, NULL);
	}

	if (confHdl && theReader)
	{
		theReader->initialize(_common_set_min.theHostRef);

		jvxErrorType res = CjvxSpNMeasureIr_oneMeasurement::read_measurement(
			&oneResult,
			fName,
			theReader,
			confHdl, JVX_WAV_READ_WRITE_INTERNAL_BSIZE);

		if (res != JVX_NO_ERROR)
		{
			std::cout << __FUNCTION__ << "-- Failed to read measurment data from file <" << fName << ">." << std::endl;
		}

		theReader->terminate();

		if (oneResult.resultsChannel.size())
		{
			jvxBool alreadyInUse = false;
			auto elm = collectedResults.begin();
			for (; elm != collectedResults.end(); elm++)
			{
				if (
					(elm->nameMeasure == oneResult.nameMeasure) &&
					(elm->tokenMeasure == oneResult.tokenMeasure))
				{
					alreadyInUse = true;
					break;
				}
			}
			if (!alreadyInUse)
			{
				collectedResults.push_back(oneResult);
			}

			update_plot_measurements();
			updateUi = true;
		}
	}

	if (confObj)
	{
		_common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, confObj);
	}
	if (readObj)
	{
		_common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_READER, readObj, 0, NULL);
	}
}