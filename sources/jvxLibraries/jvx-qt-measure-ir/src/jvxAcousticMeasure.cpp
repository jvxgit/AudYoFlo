#include "jvxAcousticMeasure.h"

JVX_QT_WIDGET_INIT_DEFINE(IjvxQtAcousticMeasurement, jvxQtAcousticMeasurement, jvxAcousticMeasure)
JVX_QT_WIDGET_TERMINATE_DEFINE(IjvxQtAcousticMeasurement, jvxQtAcousticMeasurement)

// =====================================================================
// =====================================================================
// =====================================================================

void
jvxAcousticMeasure::changed_td_ymin()
{
	td.miny = lineEdit_td_ymin->text().toData();
	td.maxy = JVX_MAX(td.maxy, td.miny + JVX_EPS_XY);
	replot_diagrams(false, false);
	this->update_window();
}

void
jvxAcousticMeasure::changed_td_ymax()
{
	td.maxy = lineEdit_td_ymax->text().toData();
	td.miny = JVX_MIN(td.maxy - JVX_EPS_XY, td.miny);
	replot_diagrams(false, false);
	this->update_window();
}

void
jvxAcousticMeasure::toggled_td_yauto(bool tog)
{
	td.autoy = tog;
	replot_diagrams(false, false);
	this->update_window();
}

void
jvxAcousticMeasure::changed_td_xmin()
{
	td.minx = lineEdit_td_xmin->text().toData();
	td.maxx = JVX_MAX(td.maxx, td.minx + JVX_EPS_XY);
	replot_diagrams(false, true);
	this->update_window();
}

void
jvxAcousticMeasure::changed_td_xmax()
{
	td.maxx = lineEdit_td_xmax->text().toData();
	td.minx = JVX_MIN(td.maxx - JVX_EPS_XY, td.minx);
	replot_diagrams(false, true);
	this->update_window();
}

void
jvxAcousticMeasure::toggled_td_xauto(bool tog)
{
	td.autox = tog;
	replot_diagrams(false, true);
	this->update_window();
}

// ======================================================

void
jvxAcousticMeasure::changed_fd_ymin()
{
	sec.miny = lineEdit_fd_ymin->text().toData();
	sec.maxy = JVX_MAX(sec.maxy, sec.miny + JVX_EPS_XY);
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();
}

void
jvxAcousticMeasure::changed_fd_ymax()
{
	sec.maxy = lineEdit_fd_ymax->text().toData();
	sec.miny = JVX_MIN(sec.maxy - JVX_EPS_XY, sec.miny);
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();

}

void
jvxAcousticMeasure::toggled_fd_yauto(bool tog)
{
	sec.autoy = tog;
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();
}

void
jvxAcousticMeasure::changed_fd_xmin()
{
	sec.minx = lineEdit_fd_xmin->text().toData();
	sec.maxx = JVX_MAX(sec.maxx, sec.minx + JVX_EPS_XY);
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();
}

void
jvxAcousticMeasure::changed_fd_xmax()
{
	sec.maxx = lineEdit_fd_xmax->text().toData();
	sec.minx = JVX_MIN(sec.maxx - JVX_EPS_XY, sec.minx);
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();
}

void
jvxAcousticMeasure::toggled_fd_xauto(bool tog)
{
	sec.autox = tog;
	replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	this->update_window();
}

// ==============================================================================

void
jvxAcousticMeasure::changed_color_p1(int sel)
{
	plot1.optionColor = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_pixels_p1(int sel)
{
	plot1.optionWidth = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_style_p1(int sel)
{
	plot1.optionStyle = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_markers_p1(int sel)
{
	plot1.optionMarker = sel;
	this->update_window();
	set_params_plots();
}

// ==============================================================================

void
jvxAcousticMeasure::changed_color_p2(int sel)
{
	plot2.optionColor = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_pixels_p2(int sel)
{
	plot2.optionWidth = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_style_p2(int sel)
{
	plot2.optionStyle = sel;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_markers_p2(int sel)
{
	plot2.optionMarker = sel;
	this->update_window();
	set_params_plots();
}


void
jvxAcousticMeasure::import_data_plot1()
{
	jvxSize i;
	jvxHandle* ptr = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	res = import_data_plot(dataPlot1, "", "");
	dataPlot12 = dataPlot1;
	dataPlot12.relink();
}


void
jvxAcousticMeasure::import_data_plot2()
{
	jvxSize i;
	jvxHandle* ptr = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	res = import_data_plot(dataPlot2, "", "");
}

void
jvxAcousticMeasure::import_data_plot12()
{
	dataPlot2 = dataPlot12;
	dataPlot2.relink();
	this->replot_diagrams(true, true);
}

void
jvxAcousticMeasure::show_plot1(bool tog)
{
	plot1.plotActive = tog;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::show_plot2(bool tog)
{
	plot2.plotActive = tog;
	this->update_window();
	set_params_plots();
}

void
jvxAcousticMeasure::changed_td_signal(int sel)
{
	modeTd = (jvxPlotModeEnumTimeDomain)sel;
	this->update_window();
	this->replot_diagrams(true, true);
}

void 
jvxAcousticMeasure::changed_axis_linlog(int sel)
{
	if (sel == 0)
	{
		sec.plotLinearx = true;
	}
	else
	{
		sec.plotLinearx = false;
	}
	this->update_window();
	this->replot_diagrams(true, true);
}

void
jvxAcousticMeasure::changed_sec_mode(int sel)
{
	lstSecMarkers[modeSec] = secondary_markers;
	modeSec = (jvxPlotModeEnumSecondary)sel;
	secondary_markers.clear();
	auto elm = lstSecMarkers.find(modeSec);
	if (elm != lstSecMarkers.end())
	{
		secondary_markers = elm->second;
	}
	this->update_window();
	this->replot_diagrams(true, true);

	this->updateMarkerPlots(
		secondary_markers.markers_1.markers_x,
		secondary_markers.markers_1.markers_y,
		qcp_secondary, 2);
	if (secondary_markers.markers_1.show)
	{
		qcp_secondary->graph(2)->setVisible(true);
	}
	else
	{
		qcp_secondary->graph(2)->setVisible(false);
	}
	this->updateMarkerPlots(
		secondary_markers.markers_2.markers_x,
		secondary_markers.markers_2.markers_y,
		qcp_secondary, 3);

	if (secondary_markers.markers_2.show)
	{
		qcp_secondary->graph(3)->setVisible(true);
	}
	else
	{ 
		qcp_secondary->graph(3)->setVisible(false);
	}
	qcp_secondary->replot();
}

void 
jvxAcousticMeasure::toggled_td_legend(bool tog)
{
	td.showLegend = tog;
	update_window();

	if (td.showLegend)
	{
		qcp_timedomain->legend->setVisible(true);
	}
	else
	{
		qcp_timedomain->legend->setVisible(false);
	}
	qcp_timedomain->replot();	
}

void 
jvxAcousticMeasure::toggled_sec_legend(bool tog)
{
	sec.showLegend = tog;
	update_window();

	if (sec.showLegend)
	{
		qcp_secondary->legend->setVisible(true);
	}
	else
	{
		qcp_secondary->legend->setVisible(false);
	}

	qcp_secondary->replot();
}

void
jvxAcousticMeasure::verify_min_gap(jvxData& minv, jvxData& maxv)
{
	if (maxv < minv + JVX_EPS_XY)
	{
		jvxData mid = (minv + maxv) / 2.0;
		minv = mid - JVX_EPS_XY / 2;
		maxv = mid + JVX_EPS_XY / 2;
	}
}

jvxBool
jvxAcousticMeasure::showThisProcessor(jvxMeasurementDataProcessorTask task)
{
	jvxBool retVal = false;
	switch (task)
	{
	case JVX_ACOUSTIC_MEASURE_TASK_EQUALIZE:
		if (
			(modeTd == JVX_PLOT_MODE_TD_IR) &&
			(modeSec == JVX_PLOT_MODE_SEC_MAG))
		{
			retVal = true;
		}
		break;
	case JVX_ACOUSTIC_MEASURE_TASK_EXTRACT_HRTFS:
		if (
			(modeTd == JVX_PLOT_MODE_TD_IR) &&
			(dataPlot1.oneChan.lBuf) && 
			(dataPlot2.oneChan.lBuf))
		{
			retVal = true;
		}
		break;
	}
	return retVal;
}

jvxErrorType 
jvxAcousticMeasure::register_data_processor(const char* descr, jvxMeasurementDataProcessorTask task, IjvxQtAcousticMeasurement_process* fld)
{
	std::list<oneRegisteredProcessor> newLst;
	std::list<oneRegisteredProcessor>* lstPtr = &newLst;
	jvxBool isNewLst = true;

	auto elm = registeredProcessors.find(task);
	if (elm != registeredProcessors.end())
	{
		lstPtr = &elm->second;
		isNewLst = false;
	}

	auto elmL = lstPtr->begin();
	for (; elmL != lstPtr->end(); elmL++)
	{
		if (elmL->proc == fld)
		{
			return JVX_ERROR_DUPLICATE_ENTRY;
		}
	}

	// Adding new element is allowed
	oneRegisteredProcessor newElm;
	newElm.descr = descr;
	newElm.proc = fld;
	newElm.task = task;

	lstPtr->push_back(newElm);

	// If this is a new list, add it!
	if (isNewLst)
	{
		registeredProcessors[task] = newLst;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxAcousticMeasure::unregister_data_processor(jvxMeasurementDataProcessorTask task, IjvxQtAcousticMeasurement_process* fld)
{
	auto elm = registeredProcessors.find(task);
	if (elm != registeredProcessors.end())
	{
		auto elmL = elm->second.begin();
		for (; elmL != elm->second.end(); elmL++)
		{
			if (elmL->proc == fld)
			{
				break;
			}
		}
		if (elmL != elm->second.end())
		{
			elm->second.erase(elmL);
		}
		if (elm->second.size() == 0)
		{
			registeredProcessors.erase(elm);
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxAcousticMeasure::invite_processor(IjvxQtAcousticMeasurement* fld)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
jvxAcousticMeasure::goodbye_processor(IjvxQtAcousticMeasurement* fld)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
jvxAcousticMeasure::import_data_plot(
	oneSetDataPlot& dtPlot,
	const std::string& nmMeas,
	const std::string& nmChan)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	jPAD ident;
	jPD trans;
	if (propRef)
	{
		jvxCallManagerProperties callGate;
		std::string prop_token = "/result_measurements_data";

		oneSetDataPlot newImport;

		if (
			(!nmMeas.empty()) && !(nmChan.empty()))
		{
			newImport.oneChan.nmChan.assign(nmChan);
			newImport.oneChan.nmMeas.assign(nmMeas);
		}

		// Obtain the number of samples		
		ident.reset(prop_token.c_str());
		trans.reset(true);
		res = propRef->get_property(callGate, 
			jPRG(&newImport.oneChan,
			1, JVX_DATAFORMAT_HANDLE),
			ident, trans);

		if (res == JVX_NO_ERROR)
		{
			if (newImport.oneChan.lBuf)
			{
				dtPlot.oneChan.lBuf = newImport.oneChan.lBuf;
				dtPlot.oneChan.nmChan.assign(newImport.oneChan.nmChan.std_str());
				dtPlot.oneChan.nmMeas.assign(newImport.oneChan.nmMeas.std_str());

				if (dtPlot.oneChan.lBuf != dtPlot.data.ir.size())
				{
					dtPlot.data.ir.resize((int)dtPlot.oneChan.lBuf);
					dtPlot.data.meas.resize((int)dtPlot.oneChan.lBuf);
					dtPlot.data.test.resize((int)dtPlot.oneChan.lBuf);
					dtPlot.data.ind_timedomain.resize((int)dtPlot.oneChan.lBuf);
				}

				dtPlot.relink();

				ident.reset(prop_token.c_str());
				trans.reset(true);
				res = propRef->get_property(callGate ,
					jPRG(&dtPlot.oneChan,
					1, JVX_DATAFORMAT_HANDLE),
					ident, trans);

				if (res == JVX_NO_ERROR)
				{
					for (i = 0; i < dtPlot.data.ind_timedomain.size(); i++)
					{
						dtPlot.data.ind_timedomain[(int)i] = (jvxData)i;
					}

					dtPlot.measurement_name = dtPlot.oneChan.nmMeas.std_str();
					dtPlot.channel_name = dtPlot.oneChan.nmChan.std_str();
					dtPlot.file_folder_read = jvx_pathExprFromFilePath(
						dtPlot.oneChan.fileLocate.std_str(), JVX_SEPARATOR_DIR_CHAR);
					this->replot_diagrams(true, true);
				}
			}
		}
	}
	return res;
}

void 
jvxAcousticMeasure::copy_td()
{
	std::string txt;
	QClipboard* clipboard = QGuiApplication::clipboard();
	pushButton_copy_td->setText("*Wait*");
	qApp->processEvents();
	switch (modeTd)
	{
	case JVX_PLOT_MODE_TD_IR:
	case JVX_PLOT_MODE_TD_IR_LOG:
		if (dataPlot1.data.ir.size())
		{
			txt += "cb_ir1 = ";
			txt += jvx_valueList2String(dataPlot1.data.ir.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.ir.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.ir.size())
		{
			txt += "cb_ir2 = ";
			txt += jvx_valueList2String(dataPlot2.data.ir.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.ir.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	case JVX_PLOT_MODE_TD_MEAS:
		if (dataPlot1.data.meas.size())
		{
			txt += "cb_meas1 = ";
			txt += jvx_valueList2String(dataPlot1.data.ir.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.ir.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.meas.size())
		{
			txt += "cb_meas2 = ";
			txt += jvx_valueList2String(dataPlot2.data.ir.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.meas.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	case JVX_PLOT_MODE_TD_TEST:
		if (dataPlot1.data.meas.size())
		{
			txt += "cb_test1 = ";
			txt += jvx_valueList2String(dataPlot1.data.test.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.test.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.meas.size())
		{
			txt += "cb_test2 = ";
			txt += jvx_valueList2String(dataPlot2.data.test.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.test.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	}
	if (!txt.empty())
	{
		qApp->processEvents();
		clipboard->setText(txt.c_str());
	}	
	pushButton_copy_td->setText("Copy");
}

void 
jvxAcousticMeasure::copy_fd()
{
	std::string txt;
	QClipboard* clipboard = QGuiApplication::clipboard();
	pushButton_copy_fd->setText("*Wait*");
	qApp->processEvents();
	switch (modeSec)
	{
	case JVX_PLOT_MODE_SEC_MAG:
		if (dataPlot1.data.show_freqdomain_y.size())
		{
			txt += "cb_mag1y = ";
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_mag1x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.show_freqdomain_y.size())
		{
			txt += "cb_mag2y = ";
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_mag2x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	case JVX_PLOT_MODE_SEC_PHASE:
		if (dataPlot1.data.show_freqdomain_y.size())
		{
			txt += "cb_phase1y = ";
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_phase1x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.show_freqdomain_y.size())
		{
			txt += "cb_phase2y = ";
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_phase2x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	case JVX_PLOT_MODE_SEC_GROUP_DELAY:
		if (dataPlot1.data.show_freqdomain_y.size())
		{
			txt += "cb_gdel1y = ";
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_gdel1x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.show_freqdomain_y.size())
		{
			txt += "cb_gdel2y = ";
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_gdel2x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	case JVX_PLOT_MODE_SEC_HIST_DELAY:
		if (dataPlot1.data.show_freqdomain_y.size())
		{
			txt += "cb_hdel1y = ";
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_hdel1x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot1.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot1.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		qApp->processEvents();
		if (dataPlot2.data.show_freqdomain_y.size())
		{
			txt += "cb_hdel2y = ";
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_y.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_y.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
			txt += "cb_hdel2x = ";
			qApp->processEvents();
			txt += jvx_valueList2String(dataPlot2.data.show_freqdomain_x.data(),
				JVX_DATAFORMAT_DATA,
				dataPlot2.data.show_freqdomain_x.size(), JVX_DATA_2STRING_CONST_FORMAT_G);
			txt += ";";
		}
		break;
	}
	if (!txt.empty())
	{
		qApp->processEvents();
		clipboard->setText(txt.c_str());
	}
	pushButton_copy_fd->setText("Copy");
}

void 
jvxAcousticMeasure::changed_data_tag()
{
}


