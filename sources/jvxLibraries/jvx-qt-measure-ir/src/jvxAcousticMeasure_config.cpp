#include "jvxAcousticMeasure.h"

const char* tokens_td_modes[jvxAcousticMeasure::JVX_PLOT_MODE_TD_LIMIT]
{
	"JVX_PLOT_MODE_TD_IR",
	"JVX_PLOT_MODE_TD_IR_LOG",
	"JVX_PLOT_MODE_TD_MEAS",
	"JVX_PLOT_MODE_TD_TEST"
};

const char* tokens_sec_modes[jvxAcousticMeasure::JVX_PLOT_MODE_SEC_LIMIT]
{
	"JVX_PLOT_MODE_SEC_MAG",
	"JVX_PLOT_MODE_SEC_PHASE",
	"JVX_PLOT_MODE_SEC_GROUP_DELAY",
	"JVX_PLOT_MODE_SEC_HIST_DELAY"
};

// =============================================================================

jvxErrorType 
jvxAcousticMeasure::get_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res;
	std::string nmSec;

	jvxConfigData* datCfg = NULL;
	if (
		(!dataPlot1.measurement_name.empty()) && (!dataPlot1.channel_name.empty()))
	{
		processor->createEmptySection(&datCfg, "IMPORT_DATA_PLOT1");
		if (datCfg)
		{
			jvxConfigData* datSubSec = NULL;
			processor->createAssignmentString(&datSubSec, "IMPORT_DATA_MEASUREMENT_NAME",
				dataPlot1.measurement_name.c_str());
			processor->addSubsectionToSection(datCfg, datSubSec);

			datSubSec = NULL;
			processor->createAssignmentString(&datSubSec, "IMPORT_DATA_CHANNEL_NAME",
				dataPlot1.channel_name.c_str());
			processor->addSubsectionToSection(datCfg, datSubSec);

			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datCfg);
		}

	}

	datCfg = NULL;
	if (
		(!dataPlot2.measurement_name.empty()) && (!dataPlot2.channel_name.empty()))
	{
		processor->createEmptySection(&datCfg, "IMPORT_DATA_PLOT2");
		if (datCfg)
		{
			jvxConfigData* datSubSec = NULL;
			processor->createAssignmentString(&datSubSec, "IMPORT_DATA_MEASUREMENT_NAME",
				dataPlot1.measurement_name.c_str());
			processor->addSubsectionToSection(datCfg, datSubSec);

			datSubSec = NULL;
			processor->createAssignmentString(&datSubSec, "IMPORT_DATA_CHANNEL_NAME",
				dataPlot1.channel_name.c_str());
			processor->addSubsectionToSection(datCfg, datSubSec);

			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datCfg);
		}

	}

	// ====================================================================

	if (
		(timedomain_markers.markers_1.markers_x.size()) ||
		(timedomain_markers.markers_2.markers_x.size()))
	{
		auto elm = lstTdMarkers.find(modeTd);
		if (elm != lstTdMarkers.end())
		{
			elm->second = timedomain_markers;
		}
		else
		{
			lstTdMarkers[modeTd] = timedomain_markers;
		}
	}

	if (
		(secondary_markers.markers_1.markers_x.size()) ||
		(secondary_markers.markers_2.markers_x.size()))
	{
		auto elm = lstSecMarkers.find(modeSec);
		if (elm != lstSecMarkers.end())
		{
			elm->second = secondary_markers;
		}
		else
		{
			lstSecMarkers[modeSec] = secondary_markers;
		}
	}

	// ====================================================================
	if (lstTdMarkers.size())
	{
		auto elm = lstTdMarkers.begin();
		for (; elm != lstTdMarkers.end(); elm++)
		{
			jvxConfigData* secDat = NULL;

			write_single_marker(tokens_td_modes[elm->first],
				&secDat,
				elm->second,
				processor);
			if(secDat)
			{
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secDat);
			} // if(secDat)
		}
	}

	if (lstSecMarkers.size())
	{
		auto elm = lstSecMarkers.begin();
		for (; elm != lstSecMarkers.end(); elm++)
		{
			jvxConfigData* secDat = NULL;

			write_single_marker(tokens_sec_modes[elm->first],
				&secDat,
				elm->second,
				processor);
			if (secDat)
			{
				processor->addSubsectionToSection(sectionWhereToAddAllSubsections, secDat);
			} // if(secDat)
		}
	}

	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "JVX_TD_MODESELECTION", 
		sectionWhereToAddAllSubsections,
		modeTd, processor);

	JVX_GET_CONFIGURATION_INJECT_VALUE(res, "JVX_SEC_MODESELECTION",
		sectionWhereToAddAllSubsections,
		modeSec, processor);

	JVX_GET_CONFIGURATION_INJECT_STRING(res, "JVX_USECASE_TAG",
		sectionWhereToAddAllSubsections,
		this->dataTag, processor);
	
	return JVX_NO_ERROR;
}

void
jvxAcousticMeasure::write_single_marker(
	const char* sec_name,
	jvxConfigData** secDatArg, 
	set_markers& oneSet,
	IjvxConfigProcessor* processor)
{
	jvxErrorType res;
	jvxConfigData* secDat = NULL;
	processor->createEmptySection(&secDat, sec_name);
	if (secDat)
	{
		if (oneSet.markers_1.markers_x.size())
		{
			HjvxConfigProcessor_writeEntry_1dList(processor,
				secDat,
				"MARKERS1_X_VALUES",
				oneSet.markers_1.markers_x.data(),
				oneSet.markers_1.markers_x.size());

			HjvxConfigProcessor_writeEntry_1dList(processor,
				secDat,
				"MARKERS1_Y_VALUES",
				oneSet.markers_1.markers_y.data(),
				oneSet.markers_1.markers_y.size());
		}

		JVX_GET_CONFIGURATION_INJECT_VALUE(res,
			"MARKERS1_X_SHOW",
			secDat,
			oneSet.markers_1.show, processor);

		if (oneSet.markers_2.markers_x.size())
		{
			HjvxConfigProcessor_writeEntry_1dList(processor,
				secDat,
				"MARKERS2_X_VALUES",
				oneSet.markers_2.markers_x.data(),
				oneSet.markers_2.markers_x.size());

			HjvxConfigProcessor_writeEntry_1dList(processor,
				secDat,
				"MARKERS2_Y_VALUES",
				oneSet.markers_2.markers_y.data(),
				oneSet.markers_2.markers_y.size());
		}

		JVX_GET_CONFIGURATION_INJECT_VALUE(res,
			"MARKERS2_X_SHOW", secDat,
			oneSet.markers_2.show, processor);
	}
	if (secDatArg)
	{
		*secDatArg = secDat;
	}
}

// ===============================================================================
// ===============================================================================

jvxErrorType 
jvxAcousticMeasure::put_configuration_ext(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename, jvxInt32 lineno)
{
	jvxConfigData* datCfg = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxApiString astr;
	jvxInt32 valI = 0;
	jvxSize num = 0;
	jvxSize i;
	std::string nmSec;

	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "JVX_TD_MODESELECTION",
		sectionToContainAllSubsectionsForMe, &valI, processor);
	if (res == JVX_NO_ERROR)
	{
		modeTd = (jvxPlotModeEnumTimeDomain)valI;
	}

	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(res, "JVX_SEC_MODESELECTION",
		sectionToContainAllSubsectionsForMe, &valI, processor);
	if (res == JVX_NO_ERROR)
	{
		modeSec = (jvxPlotModeEnumSecondary)valI;
	}

	JVX_PUT_CONFIGURATION_EXTRACT_STRING(res, "JVX_USECASE_TAG",
		sectionToContainAllSubsectionsForMe, this->dataTag, processor);

	lstTdMarkers.clear();
	jvxSize mtd = 0;
	for (; mtd != jvxAcousticMeasure::JVX_PLOT_MODE_TD_LIMIT; mtd++)
	{
		jvxConfigData* secDat = NULL;
		processor->getReferenceEntryCurrentSection_name(
			sectionToContainAllSubsectionsForMe,
			&secDat,
			tokens_td_modes[mtd]);
		if (secDat)
		{
			set_markers oneSet;
			read_single_marker(secDat, oneSet, processor);
			lstTdMarkers[(jvxPlotModeEnumTimeDomain)mtd] = oneSet;
		}
	}
	auto elmT = lstTdMarkers.find(modeTd);
	if (elmT != lstTdMarkers.end())
	{
		timedomain_markers = elmT->second;
	}

	if (timedomain_markers.markers_1.show)
	{
		qcp_timedomain->graph(2)->setVisible(true);
		replot_markers(
			timedomain_markers.markers_1.markers_x,
			timedomain_markers.markers_1.markers_y,
			qcp_timedomain,
			2);
	}
	else
	{
		qcp_timedomain->graph(2)->setVisible(false);
	}
	if (timedomain_markers.markers_2.show)
	{
		qcp_timedomain->graph(3)->setVisible(true);
		replot_markers(
			timedomain_markers.markers_2.markers_x,
			timedomain_markers.markers_2.markers_y,
			qcp_timedomain,
			2);
	}
	else
	{
		qcp_timedomain->graph(3)->setVisible(false);
	}

	lstSecMarkers.clear();
	jvxSize mts = 0;
	for (; mts != jvxAcousticMeasure::JVX_PLOT_MODE_SEC_LIMIT; mts++)
	{
		jvxConfigData* secDat = NULL;
		processor->getReferenceEntryCurrentSection_name(
			sectionToContainAllSubsectionsForMe,
			&secDat,
			tokens_sec_modes[mts]);
		if (secDat)
		{
			set_markers oneSet;
			read_single_marker(secDat, oneSet, processor);
			lstSecMarkers[(jvxPlotModeEnumSecondary)mts] = oneSet;
		}
	}
	auto elmS = lstSecMarkers.find(modeSec);
	if (elmS != lstSecMarkers.end())
	{
		secondary_markers = elmS->second;
	}

	if (secondary_markers.markers_1.show)
	{
		qcp_secondary->graph(2)->setVisible(true);
		replot_markers(
			secondary_markers.markers_1.markers_x,
			secondary_markers.markers_1.markers_y,
			qcp_secondary,
			2);
	}
	else
	{
		qcp_secondary->graph(2)->setVisible(false);
	}
	if (secondary_markers.markers_2.show)
	{
		qcp_secondary->graph(3)->setVisible(true);
		replot_markers(
			secondary_markers.markers_2.markers_x,
			secondary_markers.markers_2.markers_y,
			qcp_secondary,
			3);
	}
	else
	{
		qcp_secondary->graph(3)->setVisible(false);
	}

	// ==================================================================

	processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
		&datCfg, "IMPORT_DATA_PLOT1");
	if (datCfg)
	{
		std::string meas_name;
		std::string chan_name;
		jvxConfigData* datSubSec = NULL;
		processor->getReferenceEntryCurrentSection_name(datCfg, &datSubSec, "IMPORT_DATA_MEASUREMENT_NAME");
		if (datSubSec)
		{
			res = processor->getAssignmentString(datSubSec, &astr);
			if (res == JVX_NO_ERROR)
			{
				meas_name = astr.std_str();
			}
		}
		datSubSec = NULL;
		processor->getReferenceEntryCurrentSection_name(datCfg, &datSubSec, "IMPORT_DATA_CHANNEL_NAME");
		if (datSubSec)
		{
			res = processor->getAssignmentString(datSubSec, &astr);
			if (res == JVX_NO_ERROR)
			{
				chan_name = astr.std_str();
			}
		}

		if (
			(!meas_name.empty()) &&
			(!chan_name.empty()))
		{
			import_data_plot(dataPlot1, meas_name, chan_name);
		}
		datCfg = NULL;
	}

	processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe,
		&datCfg, "IMPORT_DATA_PLOT2");
	if (datCfg)
	{
		std::string meas_name;
		std::string chan_name;
		jvxConfigData* datSubSec = NULL;
		processor->getReferenceEntryCurrentSection_name(datCfg, &datSubSec, "IMPORT_DATA_MEASUREMENT_NAME");
		if (datSubSec)
		{
			res = processor->getAssignmentString(datSubSec, &astr);
			if (res == JVX_NO_ERROR)
			{
				meas_name = astr.std_str();
			}
		}
		datSubSec = NULL;
		processor->getReferenceEntryCurrentSection_name(datCfg, &datSubSec, "IMPORT_DATA_CHANNEL_NAME");
		if (datSubSec)
		{
			res = processor->getAssignmentString(datSubSec, &astr);
			if (res == JVX_NO_ERROR)
			{
				chan_name = astr.std_str();
			}
		}

		if (
			(!meas_name.empty()) &&
			(!chan_name.empty()))
		{
			import_data_plot(dataPlot2, meas_name, chan_name);
		}

		datCfg = NULL;
	}

	return JVX_NO_ERROR;
}

void
jvxAcousticMeasure::read_single_marker(
	jvxConfigData* secDatArg,
	set_markers& oneSet,
	IjvxConfigProcessor* processor)
{
	jvxErrorType resL = JVX_NO_ERROR;

	jvxConfigData* secX = NULL;
	jvxConfigData* secY = NULL;
		
	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(resL,
		"MARKERS1_X_SHOW", secDatArg,
		&oneSet.markers_1.show, processor);

	processor->getReferenceEntryCurrentSection_name(
		secDatArg,
		&secX, 
		"MARKERS1_X_VALUES");
	processor->getReferenceEntryCurrentSection_name(
		secDatArg,
		&secY,
		"MARKERS1_Y_VALUES");

	if(secX && secY)
	{
		jvxSize numX = 0;
		jvxSize numY = 0;
		processor->getNumberValueLists(secX, &numX);
		processor->getNumberValueLists(secY, &numY);
		if ((numY == 1) && (numX == 1))
		{
			jvxApiValueList vlistX;
			jvxApiValueList vlistY;
			jvxSize i;
			processor->getValueList_id(secX, &vlistX, 0);
			processor->getValueList_id(secY, &vlistY, 0);
			if (vlistX.ll() == vlistY.ll())
			{
				oneSet.markers_1.markers_x.resize(vlistX.ll());
				oneSet.markers_1.markers_y.resize(vlistY.ll());
				for (i = 0; i < vlistX.ll(); i++)
				{
					jvxData valD = 0;
					vlistX.elm_at(i).toContent(&valD);
					oneSet.markers_1.markers_x[i] = valD;
				}
				for (i = 0; i < vlistY.ll(); i++)
				{
					jvxData valD = 0;
					vlistY.elm_at(i).toContent(&valD);
					oneSet.markers_1.markers_y[i] = valD;
				}
			}
		}
	}
	
	secX = NULL;
	secY = NULL;

	processor->getReferenceEntryCurrentSection_name(
		secDatArg,
		&secX,
		"MARKERS2_X_VALUES");
	processor->getReferenceEntryCurrentSection_name(
		secDatArg,
		&secY,
		"MARKERS2_Y_VALUES");

	JVX_PUT_CONFIGURATION_EXTRACT_VALUE(resL,
		"MARKERS2_X_SHOW", secDatArg,
		&oneSet.markers_2.show, processor);

	if (secX && secY)
	{
		jvxSize numX = 0;
		jvxSize numY = 0;
		processor->getNumberValueLists(secX, &numX);
		processor->getNumberValueLists(secY, &numY);
		if ((numY == 1) && (numX == 1))
		{
			jvxApiValueList vlistX;
			jvxApiValueList vlistY;
			jvxSize i;
			processor->getValueList_id(secX, &vlistX, 0);
			processor->getValueList_id(secY, &vlistY, 0);
			if (vlistX.ll() == vlistY.ll())
			{
				oneSet.markers_2.markers_x.resize(vlistX.ll());
				oneSet.markers_2.markers_y.resize(vlistY.ll());
				for (i = 0; i < vlistX.ll(); i++)
				{
					jvxData valD = 0;
					vlistX.elm_at(i).toContent(&valD);
					oneSet.markers_2.markers_x[i] = valD;
				}
				for (i = 0; i < vlistY.ll(); i++)
				{
					jvxData valD = 0;
					vlistY.elm_at(i).toContent(&valD);
					oneSet.markers_2.markers_y[i] = valD;
				}
			}
		}
	}
}
