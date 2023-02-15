#include "CjvxSpNMeasureIr.h"

// ===================================================================
// ===================================================================

jvxErrorType
CjvxSpNMeasureIr::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize selNew = JVX_SIZE_UNSELECTED;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		jvxConfigData* secRead = NULL;
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genMeasureIr_node::put_configuration_all(callMan,
				processor, sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &secRead, "measurements");
			if (secRead)
			{
				put_configuration_all_measurements(secRead, processor);
			}

			reconstruct_properties(selNew);
			set_measurement_idx();

			secRead = NULL;
			jvxSize cnt = 0;
			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
				&secRead, "collected_results");

			if (secRead)
			{
				while (1)
				{
					jvxConfigData* secEntry = NULL;
					processor->getReferenceEntryCurrentSection_name(
						secRead, &secEntry, ("path_result_" + jvx_size2String(cnt)).c_str());
					if (secEntry)
					{
						jvxApiString astr;
						jvxBool updateUi = false;
						processor->getAssignmentString(secEntry, &astr);
						std::string fName = astr.std_str();
						CjvxSpNMeasureIr::read_result(fName, updateUi);

					}
					else
					{
						break;
					}
					cnt++;
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxSpNMeasureIr::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize cnt = 0;
	jvxConfigData* secWrite = NULL;
	jvxConfigData* subSecWrite = NULL;
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan,
		processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genMeasureIr_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);

		processor->createEmptySection(&secWrite, "measurements");

		auto elm = measurements.begin();
		for (; elm != measurements.end(); elm++)
		{
			get_configuration_one_measurement((*elm), cnt, secWrite, processor);
			cnt++;
		}
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secWrite);

		secWrite = NULL;
		cnt = 0;
		processor->createEmptySection(&secWrite, "collected_results");

		auto elmR = collectedResults.begin();
		for (; elmR != collectedResults.end(); elmR++)
		{
			if (elmR->storeConfig)
			{
				processor->createAssignmentString(&subSecWrite,
					("path_result_" + jvx_size2String(cnt)).c_str(),
					elmR->pathStoreProto.c_str());
				processor->addSubsectionToSection(secWrite, subSecWrite);
				cnt++;
			}
		}
		processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secWrite);
	}
	return res;
}

void
CjvxSpNMeasureIr::get_configuration_one_measurement(CjvxSpNMeasureIr_oneMeasurement* oneM, jvxSize cnt,
	jvxConfigData* secWrite, IjvxConfigProcessor* processor)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* subSecWrite = NULL;
	jvxConfigData* subSubSecWrite = NULL;
	std::string sec = "JVX_MEASUREMENT_" + jvx_size2String(cnt);
	processor->createEmptySection(&subSecWrite, sec.c_str());

	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_AMPLITUDE", subSecWrite, oneM->generator.amplitude, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_FREQ_LOW_HZ", subSecWrite, oneM->generator.freq_low_hz, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_FREQ_UP_HZ", subSecWrite, oneM->generator.freq_up_hz, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_LENGTH_SEC", subSecWrite, oneM->generator.length_seconds, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_LOOP_CNT", subSecWrite, oneM->generator.loop_count, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_NOISE_TYPE", subSecWrite, oneM->generator.noise_type, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_SEED_NOISE", subSecWrite, oneM->generator.seed_noise, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_SIL_START_SEC", subSecWrite, oneM->generator.silence_start_seconds, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_SIL_STOP_SEC", subSecWrite, oneM->generator.silence_stop_seconds, processor);
	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "MEASUREIR_WAVE_TYPE", subSecWrite, oneM->generator.tpWave, processor);
	
	// ====================================================================================

	subSubSecWrite = NULL;
	processor->createAssignmentString(&subSubSecWrite, "MEASUREIR_DESCRIPTION", oneM->description.c_str());
	processor->addSubsectionToSection(subSecWrite, subSubSecWrite);

	// ====================================================================================
	
	subSubSecWrite = NULL; 
	processor->createAssignmentValueList(&subSubSecWrite, "INPUTS");
	auto elmI = oneM->inputs.begin();
	for (; elmI != oneM->inputs.end(); elmI++)
	{
		processor->addAssignmentValueToList(subSubSecWrite, 0, elmI->first);
	}
	processor->addSubsectionToSection(subSecWrite, subSubSecWrite);
	
	// ====================================================================================
	
	subSubSecWrite = NULL;
	processor->createAssignmentValueList(&subSubSecWrite, "OUTPUTS");
	auto elmO = oneM->outputs.begin();
	for (; elmO != oneM->outputs.end(); elmO++)
	{
		processor->addAssignmentValueToList(subSubSecWrite, 0, elmO->first);
	}
	processor->addSubsectionToSection(subSecWrite, subSubSecWrite);

	// ====================================================================================

	processor->addSubsectionToSection(secWrite, subSecWrite);
}

void
CjvxSpNMeasureIr::put_configuration_all_measurements(jvxConfigData* secRead, IjvxConfigProcessor* processor)
{
	jvxSize i;
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;
	while (1)
	{
		jvxSize valS;
		jvxConfigData* subSecRead = NULL;
		jvxConfigData* subSubSecRead = NULL;
		jvxApiString astr;
		std::string sec = "JVX_MEASUREMENT_" + jvx_size2String(cnt);
		processor->getReferenceEntryCurrentSection_name(secRead, &subSecRead, sec.c_str());
		if (subSecRead)
		{
			CjvxSpNMeasureIr_oneMeasurement* oneM = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(oneM, CjvxSpNMeasureIr_oneMeasurement);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_AMPLITUDE", subSecRead, &oneM->generator.amplitude, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_FREQ_LOW_HZ", subSecRead, &oneM->generator.freq_low_hz, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_FREQ_UP_HZ", subSecRead, &oneM->generator.freq_up_hz, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_LENGTH_SEC", subSecRead, &oneM->generator.length_seconds, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_LOOP_CNT", subSecRead, &oneM->generator.loop_count, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_NOISE_TYPE", subSecRead, &valS, processor);
			oneM->generator.noise_type = (jvxGeneratorWaveNoiseType)valS;
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_SEED_NOISE", subSecRead, &oneM->generator.seed_noise, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_SIL_START_SEC", subSecRead, &oneM->generator.silence_start_seconds, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_SIL_STOP_SEC", subSecRead, &oneM->generator.silence_stop_seconds, processor);
			JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "MEASUREIR_WAVE_TYPE", subSecRead, &valS, processor);
			oneM->generator.tpWave = (jvxGeneratorWaveType)valS;

			// ================================================================================================
			subSubSecRead = NULL;
			processor->getReferenceEntryCurrentSection_name(subSecRead, &subSubSecRead, "MEASUREIR_DESCRIPTION");
			if (subSubSecRead)
			{
				processor->getAssignmentString(subSubSecRead, &astr);
				oneM->description = correctNameAndSet(astr.std_str(), NULL);
			}
			else
			{
				oneM->description = JVX_COMPOSE_MEASUREMENT(cnt);
			}
				
			// ================================================================================================

			subSubSecRead = NULL;
			processor->getReferenceEntryCurrentSection_name(subSecRead, &subSubSecRead, "INPUTS");
			if (subSubSecRead)
			{
				jvxApiValueList lst;
				processor->getValueList_id(subSubSecRead, &lst, 0);
				for (i = 0; i < lst.ll(); i++)
				{
					oneInput oneI;
					oneI.active = true;
					jvxValue val = lst.elm_at(i);
					val.toContent(&oneI.id);
					oneM->inputs[oneI.id] = oneI;
				}
			}

			// ================================================================================================

			subSubSecRead = NULL;
			processor->getReferenceEntryCurrentSection_name(subSecRead, &subSubSecRead, "OUTPUTS");
			if (subSubSecRead)
			{
				jvxApiValueList lst;
				processor->getValueList_id(subSubSecRead, &lst, 0);
				for (i = 0; i < lst.ll(); i++)
				{
					oneOutput oneO;
					oneO.active = true;
					jvxValue val = lst.elm_at(i);
					val.toContent(&oneO.id);
					oneM->outputs[oneO.id] = oneO;
				}
			}
			measurements.push_back(oneM);
			cnt++;
		}
		else
		{
			break;
		}
	}
}