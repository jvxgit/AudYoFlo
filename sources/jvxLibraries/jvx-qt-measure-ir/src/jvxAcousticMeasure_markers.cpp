#include "jvxAcousticMeasure.h"
#include "jvx_qsort_ext.h"

#include <QtWidgets/QFileDialog>

static int cmpfunc(const void* a, const void* b)
{
	if (*(jvxData*)a > *(jvxData*)b)
	{
		return (1);
	}
	if (*(jvxData*)a < *(jvxData*)b)
	{
		return -1;
	}
	return 0;
}

void
jvxAcousticMeasure::toggled_show_td_marker1(bool tog)
{
	timedomain_markers.markers_1.show = tog;
	if (timedomain_markers.markers_1.show)
	{
		qcp_timedomain->graph(2)->setVisible(true);
		replot_markers(timedomain_markers.markers_1.markers_x, timedomain_markers.markers_1.markers_y, qcp_timedomain, 2);
	}
	else
	{
		qcp_timedomain->graph(2)->setVisible(false);
		qcp_timedomain->replot();
	}
	reset_marker_td_edit();
}

void
jvxAcousticMeasure::toggled_show_td_marker2(bool tog)
{
	timedomain_markers.markers_2.show = tog;
	if (timedomain_markers.markers_2.show)
	{
		qcp_timedomain->graph(3)->setVisible(true);
		replot_markers(timedomain_markers.markers_2.markers_x, timedomain_markers.markers_2.markers_y, qcp_timedomain, 3);
	}
	else
	{
		qcp_timedomain->graph(3)->setVisible(false);
		qcp_timedomain->replot();
	}
	reset_marker_td_edit();
}

void
jvxAcousticMeasure::clicked_clear_td_marker1()
{
	timedomain_markers.markers_1.markers_x.clear();
	timedomain_markers.markers_1.markers_y.clear();
	updateMarkerPlots(
		timedomain_markers.markers_1.markers_x,
		timedomain_markers.markers_1.markers_y,
		qcp_timedomain, 2);
	reset_marker_td_edit();
}

void
jvxAcousticMeasure::clicked_clear_td_marker2()
{
	timedomain_markers.markers_2.markers_x.clear();
	timedomain_markers.markers_2.markers_y.clear();
	updateMarkerPlots(
		timedomain_markers.markers_2.markers_x,
		timedomain_markers.markers_2.markers_y,
		qcp_timedomain, 3);
	reset_marker_td_edit();
}

void
jvxAcousticMeasure::activated_process_td(int)
{
}

void
jvxAcousticMeasure::clicked_process_td(bool)
{
}

// ==================================================================

void
jvxAcousticMeasure::toggled_show_sec_marker1(bool tog)
{
	secondary_markers.markers_1.show = tog;
	if (secondary_markers.markers_1.show)
	{
		qcp_secondary->graph(2)->setVisible(true);
		replot_markers(secondary_markers.markers_1.markers_x, secondary_markers.markers_1.markers_y, qcp_secondary, 2);
	}
	else
	{
		qcp_secondary->graph(2)->setVisible(false);
		qcp_secondary->replot();
	}
	reset_marker_sec_edit();
}

void
jvxAcousticMeasure::toggled_show_sec_marker2(bool tog)
{
	secondary_markers.markers_2.show = tog;
	if (secondary_markers.markers_2.show)
	{
		qcp_secondary->graph(3)->setVisible(true);
		replot_markers(secondary_markers.markers_2.markers_x, secondary_markers.markers_2.markers_y, qcp_secondary, 3);
	}
	else
	{
		qcp_secondary->graph(3)->setVisible(false);
		qcp_secondary->replot();
	}
	reset_marker_sec_edit();
}

void
jvxAcousticMeasure::clicked_clear_sec_marker1()
{
	secondary_markers.markers_1.markers_x.clear();
	secondary_markers.markers_1.markers_y.clear();
	updateMarkerPlots(
		secondary_markers.markers_1.markers_x,
		secondary_markers.markers_1.markers_y,
		qcp_secondary, 2);
	reset_marker_sec_edit();
}

void
jvxAcousticMeasure::clicked_clear_sec_marker2()
{
	secondary_markers.markers_2.markers_x.clear();
	secondary_markers.markers_2.markers_y.clear();
	updateMarkerPlots(
		secondary_markers.markers_2.markers_x,
		secondary_markers.markers_2.markers_y,
		qcp_secondary, 3);
	reset_marker_sec_edit();
}

void
jvxAcousticMeasure::activated_process_sec(int)
{
}

void
jvxAcousticMeasure::clicked_process_sec(bool)
{
}

// ===================================================================

void 
jvxAcousticMeasure::apply_td_direct_xy()
{
	jvxData newValX = lineEdit_td_newx->text().toData();
	jvxData newValY = lineEdit_td_newy->text().toData();

	jvxEditPointMarker markerTp = JVX_EDIT_POINT_SEC_INVALID;
	jvxSize idx = JVX_SIZE_UNSELECTED;
	QVariant varTp = pushButton_td_newcoord->property("MARKERTP");

	if(varTp.isValid())
	{ 
		markerTp = (jvxEditPointMarker)varTp.toInt();
	}
	// , QVariant((int)markerSelect)
	QVariant varIdx = pushButton_td_newcoord->property("MARKERIDX");
	if (varIdx.isValid())
	{
		idx = varIdx.toInt();
	}
	switch (markerTp)
	{
	case JVX_EDIT_POINT_TD_MARKER1:
		if (idx < timedomain_markers.markers_1.markers_x.size())
		{
			timedomain_markers.markers_1.markers_x[(int)idx] = newValX;
			timedomain_markers.markers_1.markers_y[(int)idx] = newValY;
			updateMarkerPlots(
				timedomain_markers.markers_1.markers_x,
				timedomain_markers.markers_1.markers_y,
				qcp_timedomain,
				2);
		}
		reset_marker_td_edit();
		break;
	case JVX_EDIT_POINT_TD_MARKER2:
		if (idx < timedomain_markers.markers_2.markers_x.size())
		{
			timedomain_markers.markers_2.markers_x[(int)idx] = newValX;
			timedomain_markers.markers_2.markers_y[(int)idx] = newValY;
			updateMarkerPlots(
				timedomain_markers.markers_2.markers_x,
				timedomain_markers.markers_2.markers_y,
				qcp_timedomain,
				3);
		}
		reset_marker_td_edit();
		break;
	}
}

void
jvxAcousticMeasure::apply_copy_td_point()
{
	jvxEditPointMarker markerTp = JVX_EDIT_POINT_SEC_INVALID;
	jvxSize idx = JVX_SIZE_UNSELECTED;
	QClipboard* clipboard = QGuiApplication::clipboard();
	std::string txt;
	QVariant varTp = pushButton_td_newcoord->property("MARKERTP");
	if (varTp.isValid())
	{
		markerTp = (jvxEditPointMarker)varTp.toInt();
	}
	QVariant varIdx = pushButton_td_newcoord->property("MARKERIDX");
	if (varIdx.isValid())
	{
		idx = varIdx.toInt();
	}
	// , QVariant((int)markerSelect)
	switch (markerTp)
	{
	case JVX_EDIT_POINT_TD_MARKER1:
		
		if (idx < timedomain_markers.markers_1.markers_x.size())
		{
			txt = "cb_pt1 = [";
			txt += jvx_data2String(timedomain_markers.markers_1.markers_x[(int)idx]);
			txt += " ";
			txt += jvx_data2String(timedomain_markers.markers_1.markers_y[(int)idx]);
			txt += "];";
		}
		reset_marker_td_edit();
		break;
	case JVX_EDIT_POINT_TD_MARKER2:

		if (idx < timedomain_markers.markers_1.markers_x.size())
		{
			txt = "cb_pt2 = [";
			txt += jvx_data2String(timedomain_markers.markers_2.markers_x[(int)idx]);
			txt += " ";
			txt += jvx_data2String(timedomain_markers.markers_2.markers_y[(int)idx]);
			txt += "];";
		}
		reset_marker_td_edit();
		break;
	}
	if (!txt.empty())
	{
		clipboard->setText(txt.c_str());
	}
}

void
jvxAcousticMeasure::apply_td_move_xy()
{
	jvxData newValX = lineEdit_td_movex->text().toData();
	jvxData newValY = lineEdit_td_movey->text().toData();

	jvxEditPointMarker markerTp = JVX_EDIT_POINT_SEC_INVALID;
	jvxSize idx;
	QVariant varTp = pushButton_td_newcoord->property("MARKERTP");

	if (varTp.isValid())
	{
		markerTp = (jvxEditPointMarker)varTp.toInt();
	}
	// , QVariant((int)markerSelect)
	switch (markerTp)
	{
	case JVX_EDIT_POINT_TD_MARKER1:
		for (idx = 0; idx < timedomain_markers.markers_1.markers_x.size(); idx++)
		{
			timedomain_markers.markers_1.markers_x[(int)idx] += newValX;
			timedomain_markers.markers_1.markers_y[(int)idx] += newValY;
		}
		updateMarkerPlots(
				timedomain_markers.markers_1.markers_x,
				timedomain_markers.markers_1.markers_y,
				qcp_timedomain,
				2);
		
		reset_marker_td_edit();
		break;
	case JVX_EDIT_POINT_TD_MARKER2:
		for (idx = 0; idx < timedomain_markers.markers_2.markers_x.size(); idx++)
		{
			timedomain_markers.markers_2.markers_x[(int)idx] += newValX;
			timedomain_markers.markers_2.markers_y[(int)idx] += newValY;
		}
		updateMarkerPlots(
			timedomain_markers.markers_2.markers_x,
			timedomain_markers.markers_2.markers_y,
			qcp_timedomain,
			3);
		reset_marker_td_edit();
		break;
	}
}

// =============================================================================

void
jvxAcousticMeasure::apply_sec_direct_xy()
{
	jvxData newValX = lineEdit_sec_newx->text().toData();
	jvxData newValY = lineEdit_sec_newy->text().toData();

	jvxEditPointMarker markerTp = JVX_EDIT_POINT_SEC_INVALID;
	jvxSize idx;
	QVariant varTp = pushButton_sec_newcoord->property("MARKERTP");

	if (varTp.isValid())
	{
		markerTp = (jvxEditPointMarker)varTp.toInt();
	}
	// , QVariant((int)markerSelect)
	QVariant varIdx = pushButton_sec_newcoord->property("MARKERIDX");
	if (varIdx.isValid())
	{
		idx = varIdx.toInt();
	}
	switch (markerTp)
	{
	case JVX_EDIT_POINT_SEC_MARKER1:
		if (idx < secondary_markers.markers_1.markers_x.size())
		{
			secondary_markers.markers_1.markers_x[(int)idx] = newValX;
			secondary_markers.markers_1.markers_y[(int)idx] = newValY;
			updateMarkerPlots(
				secondary_markers.markers_1.markers_x,
				secondary_markers.markers_1.markers_y,
				qcp_secondary,
				2);
		}
		reset_marker_sec_edit();
		break;
	case JVX_EDIT_POINT_SEC_MARKER2:
		if (idx < secondary_markers.markers_2.markers_x.size())
		{
			secondary_markers.markers_2.markers_x[(int)idx] = newValX;
			secondary_markers.markers_2.markers_y[(int)idx] = newValY;
			updateMarkerPlots(
				secondary_markers.markers_2.markers_x,
				secondary_markers.markers_2.markers_y,
				qcp_secondary,
				3);
		}
		reset_marker_sec_edit();
		break;
	}
}

void
jvxAcousticMeasure::apply_sec_move_xy()
{
	jvxData newValX = lineEdit_sec_movex->text().toData();
	jvxData newValY = lineEdit_sec_movey->text().toData();

	jvxEditPointMarker markerTp = JVX_EDIT_POINT_SEC_INVALID;
	jvxSize idx;
	QVariant varTp = pushButton_sec_newcoord->property("MARKERTP");

	if (varTp.isValid())
	{
		markerTp = (jvxEditPointMarker)varTp.toInt();
	}
	// , QVariant((int)markerSelect)
	switch (markerTp)
	{
	case JVX_EDIT_POINT_SEC_MARKER1:
		for (idx = 0; idx <secondary_markers.markers_1.markers_x.size(); idx++)
		{
			secondary_markers.markers_1.markers_x[(int)idx] += newValX;
			secondary_markers.markers_1.markers_y[(int)idx] += newValY;
		}
		updateMarkerPlots(
			secondary_markers.markers_1.markers_x,
			secondary_markers.markers_1.markers_y,
			qcp_secondary,
			2);

		reset_marker_sec_edit();
		break;
	case JVX_EDIT_POINT_SEC_MARKER2:
		for (idx = 0; idx < secondary_markers.markers_2.markers_x.size(); idx++)
		{
			secondary_markers.markers_2.markers_x[(int)idx] += newValX;
			secondary_markers.markers_2.markers_y[(int)idx] += newValY;
		}
		updateMarkerPlots(
			secondary_markers.markers_2.markers_x,
			secondary_markers.markers_2.markers_y,
			qcp_secondary,
			3);
		reset_marker_sec_edit();
		break;
	}
}

// =====================================================================================

void
jvxAcousticMeasure::updateMarkerPlots(
	QVector<jvxData>& markers_x,
	QVector<jvxData>& markers_y,
	QCustomPlot* qcp, jvxSize gId)
{
	/*
	std::cout << "===================================" << std::endl;
	for (int i = 0; i < timedomain_markers.markers_1.markers_x.size(); i++)
	{
		std::cout << timedomain_markers.markers_1.markers_x[i] << " -- " << timedomain_markers.markers_1.markers_y[i] << std::endl;
	}
	*/

	jvx_qsort_ext(markers_x.data(),
		markers_x.size(),
		sizeof(jvxData), cmpfunc,
		markers_y.data());

	/*
	for (int i = 0; i < timedomain_markers.markers_1.markers_x.size(); i++)
	{
		std::cout << timedomain_markers.markers_1.markers_x[i] << " -- " << timedomain_markers.markers_1.markers_y[i] << std::endl;
	}
	*/

	replot_markers(markers_x, markers_y, qcp, gId);
}

void
jvxAcousticMeasure::removeSingleMarker(jvxData pos_x,
	QVector<jvxData>& markers_x,
	QVector<jvxData>& markers_y,
	QCustomPlot* qcp, jvxSize gId)
{
	jvxSize i;
	jvxSize idxNear = JVX_SIZE_UNSELECTED;
	jvxData distMin = JVX_DATA_MAX_POS;
	for (i = 0; i < markers_x.size(); i++)
	{
		jvxData distLoc = fabs(pos_x - markers_x[i]);
		if (distLoc < distMin)
		{
			idxNear = i;
			distMin = distLoc;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(idxNear))
	{
		jvxSize cnt = 0;
		QVector<jvxData> newx;
		QVector<jvxData> newy;

		newx.resize(markers_x.size() - 1);
		newy.resize(markers_x.size() - 1);

		for (i = 0; i < markers_x.size(); i++)
		{
			if (i != idxNear)
			{
				newx[(int)cnt] = markers_x[(int)i];
				newy[(int)cnt] = markers_y[(int)i];
				cnt++;
			}
		}
		markers_x = newx;
		markers_y = newy;
		replot_markers(markers_x,
			markers_y,
			qcp, gId);
	}
}

void
jvxAcousticMeasure::moveSingleMarker(
	jvxData start_x,
	jvxData start_y,
	jvxData pos_x,
	jvxData pos_y,
	QVector<jvxData>& markers_x,
	QVector<jvxData>& markers_y,
	QCustomPlot* qcp,
	jvxSize gId)
{
	jvxSize i;
	jvxSize idxNear = JVX_SIZE_UNSELECTED;
	jvxData distMin = JVX_DATA_MAX_POS;
	for (i = 0; i < markers_x.size(); i++)
	{
		jvxData distLoc = fabs(pos_x - markers_x[(int)i]);
		if (distLoc < distMin)
		{
			idxNear = i;
			distMin = distLoc;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(idxNear))
	{
		markers_x[(int)idxNear] = pos_x;
		markers_y[(int)idxNear] = pos_y;

		updateMarkerPlots(markers_x,
			markers_y,
			qcp, gId);
	}
}

void
jvxAcousticMeasure::replot_markers(
	const QVector<jvxData>& markers_x,
	const QVector<jvxData>& markers_y,
	QCustomPlot* qcp,
	jvxSize gId)
{
	qcp->graph(gId)->setData(markers_x, markers_y, true);
	qcp->replot();
}

void
jvxAcousticMeasure::activate_marker_edit(
	jvxEditPointMarker markerSelect,
	jvxData pos_x,
	QVector<jvxData>& markers_x,
	QVector<jvxData>& markers_y)
{
	jvxSize i;
	jvxSize idxNear = JVX_SIZE_UNSELECTED;
	jvxData distMin = JVX_DATA_MAX_POS;
	for (i = 0; i < markers_x.size(); i++)
	{
		jvxData distLoc = fabs(pos_x - markers_x[i]);
		if (distLoc < distMin)
		{
			idxNear = i;
			distMin = distLoc;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(idxNear))
	{
		switch (markerSelect)
		{
		case JVX_EDIT_POINT_TD_MARKER1:
		case JVX_EDIT_POINT_TD_MARKER2:
			pushButton_td_newcoord->setProperty("MARKERTP", QVariant((int)markerSelect));
			pushButton_td_newcoord->setProperty("MARKERIDX", QVariant((int)idxNear));
			pushButton_td_newcoord->setEnabled(true);
			pushButton_td_move->setProperty("MARKERTP", QVariant((int)markerSelect));
			pushButton_td_move->setEnabled(true);
			lineEdit_td_newy->setEnabled(true);
			lineEdit_td_newy->setText(jvx_data2String(markers_y[(int)idxNear], JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			lineEdit_td_newx->setEnabled(true);
			lineEdit_td_newx->setText(jvx_data2String(markers_x[(int)idxNear], JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			lineEdit_td_movey->setEnabled(true);
			lineEdit_td_movey->setText("0");
			lineEdit_td_movex->setEnabled(true);
			lineEdit_td_movex->setText("0");
			pushButton_td_copy_point->setProperty("MARKERTP", QVariant((int)markerSelect));
			pushButton_td_copy_point-> setProperty("MARKERIDX", QVariant((int)idxNear));
			pushButton_td_copy_point->setEnabled(true);
			break;

		case JVX_EDIT_POINT_SEC_MARKER1:
		case JVX_EDIT_POINT_SEC_MARKER2:
			pushButton_sec_newcoord->setProperty("MARKERTP", QVariant((int)markerSelect));
			pushButton_sec_newcoord->setProperty("MARKERIDX", QVariant((int)idxNear));
			pushButton_sec_newcoord->setEnabled(true);
			pushButton_sec_move->setProperty("MARKERTP", QVariant((int)markerSelect));
			pushButton_sec_move->setEnabled(true);
			lineEdit_sec_newy->setEnabled(true);
			lineEdit_sec_newy->setText(jvx_data2String(markers_y[(int)idxNear], JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			lineEdit_sec_newx->setEnabled(true);
			lineEdit_sec_newx->setText(jvx_data2String(markers_x[(int)idxNear], JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			lineEdit_sec_movey->setEnabled(true);
			lineEdit_sec_movey->setText("0");
			lineEdit_sec_movex->setEnabled(true);
			lineEdit_sec_movex->setText("0");
			break;
		}
	}
}

void
jvxAcousticMeasure::reset_marker_td_edit()
{
	pushButton_td_newcoord->setEnabled(false);
	lineEdit_td_newy->setEnabled(false);
	lineEdit_td_newx->setEnabled(false);
	lineEdit_td_newy->setText("");
	lineEdit_td_newx->setText("");
	pushButton_td_move->setEnabled(false);
	lineEdit_td_movey->setEnabled(false);
	lineEdit_td_movex->setEnabled(false);
	lineEdit_td_movey->setText("");
	lineEdit_td_movex->setText("");
	pushButton_td_copy_point->setEnabled(false);
}

void
jvxAcousticMeasure::reset_marker_sec_edit()
{
	pushButton_sec_newcoord->setEnabled(false);
	lineEdit_sec_newy->setEnabled(false);
	lineEdit_sec_newx->setEnabled(false);
	lineEdit_sec_newy->setText("");
	lineEdit_sec_newx->setText("");
	pushButton_sec_move->setEnabled(false);
	lineEdit_sec_movey->setEnabled(false);
	lineEdit_sec_movex->setEnabled(false);
	lineEdit_sec_movey->setText("");
	lineEdit_sec_movex->setText("");
}

const char* marker_expr_td[jvxAcousticMeasure::JVX_PLOT_MODE_TD_LIMIT] =
{
	"TD_IR",
	"TD_IR_LOG",
	"TD_MEAS",
	"TD_TEST"
};

const char* marker_expr_fd[jvxAcousticMeasure::JVX_PLOT_MODE_SEC_LIMIT] =
{
	"SEC_MAG",
	"SEC_PHASE",
	"SEC_GROUP_DELAY",
	"SEC_HIST_DELAY"
};

void
jvxAcousticMeasure::marker_export_fd()
{
	std::string folder = dataPlot1.file_folder_read;
	if (folder.empty())
	{
		folder = ".";
	}

	QString qstr = QFileDialog::getSaveFileName(
		nullptr,
		tr("Export Marker Specification"),
		folder.c_str(),
		"*.jma");

	if (!qstr.isEmpty())
	{
		IjvxToolsHost* toolsHostRef = reqInterface< IjvxToolsHost>(theHostRef);
		if (toolsHostRef)
		{
			refComp<IjvxConfigProcessor> refInst;
			refInst = reqRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR);
			if (refInst.cpPtr)
			{
				std::string fPath = qstr.toLatin1().data();
				fPath = jvx_replaceCharacter(fPath, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
				fPath = jvx_absoluteToRelativePath(fPath, false);

				jvxConfigData* cfgDat = nullptr;
				refInst.cpPtr->createEmptySection(&cfgDat, "MARKERS_EXPORT_FD");
				if (cfgDat)
				{
					jvxConfigData* cfgLst = nullptr;

					// Save current markers
					lstSecMarkers[modeSec] = secondary_markers;

					auto elm = lstSecMarkers.begin();
					for (; elm != lstSecMarkers.end(); elm++)
					{
						assert(elm->first < jvxAcousticMeasure::JVX_PLOT_MODE_SEC_LIMIT);
						if (
							(elm->second.markers_1.markers_x.size()) ||
							(elm->second.markers_2.markers_x.size()))
						{
							std::string tok_save = "MARKERS_EXPORT_";
							tok_save += marker_expr_fd[elm->first];
							if (elm->second.markers_1.markers_x.size())
							{
								assert(elm->second.markers_1.markers_x.size() == elm->second.markers_1.markers_y.size());

								std::string tok_save_ll = tok_save + "_1";
								refInst.cpPtr->createAssignmentValueList(&cfgLst, tok_save_ll.c_str());
								jvxValue vv;
								jvxSize i;
								for (i = 0; i < elm->second.markers_1.markers_x.size(); i++)
								{
									vv.assign(elm->second.markers_1.markers_x[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 0, vv);
									vv.assign(elm->second.markers_1.markers_y[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 1, vv);
								}
								refInst.cpPtr->addSubsectionToSection(cfgDat, cfgLst);
							}
							if (elm->second.markers_2.markers_x.size())
							{
								assert(elm->second.markers_2.markers_x.size() == elm->second.markers_2.markers_y.size());

								std::string tok_save_ll = tok_save + "_2";
								refInst.cpPtr->createAssignmentValueList(&cfgLst, tok_save_ll.c_str());
								jvxValue vv;
								jvxSize i;
								for (i = 0; i < elm->second.markers_2.markers_x.size(); i++)
								{
									vv.assign(elm->second.markers_2.markers_x[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 0, vv);
									vv.assign(elm->second.markers_2.markers_y[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 1, vv);
								}
								refInst.cpPtr->addSubsectionToSection(cfgDat, cfgLst);
							}
						}
					}

					jvxApiString astr;
					refInst.cpPtr->printConfiguration(cfgDat, &astr, true);

					jvx_writeContentToFile(fPath, astr.std_str());

					refInst.cpPtr->removeHandle(cfgDat);
					cfgDat = nullptr;

				}
				retRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR, refInst);
			}
			retInterface< IjvxToolsHost>(theHostRef, toolsHostRef);
		}
	}
}

void
jvxAcousticMeasure::marker_export_td()
{
	std::string folder = dataPlot1.file_folder_read;
	if (folder.empty())
	{
		folder = ".";
	}

	QString qstr = QFileDialog::getSaveFileName(
		nullptr,
		tr("Save Marker Specification"),
		folder.c_str(),
		"*.jma");

	if (!qstr.isEmpty())
	{
		IjvxToolsHost* toolsHostRef = reqInterface< IjvxToolsHost>(theHostRef);
		if (toolsHostRef)
		{
			refComp<IjvxConfigProcessor> refInst;
			refInst = reqRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR);
			if (refInst.cpPtr)
			{
				std::string fPath = qstr.toLatin1().data();
				fPath = jvx_replaceCharacter(fPath, JVX_SEPARATOR_DIR_CHAR_THE_OTHER, JVX_SEPARATOR_DIR_CHAR);
				fPath = jvx_absoluteToRelativePath(fPath, false);

				jvxConfigData* cfgDat = nullptr;
				refInst.cpPtr->createEmptySection(&cfgDat, "MARKERS_EXPORT_TD");
				if (cfgDat)
				{
					jvxConfigData* cfgLst = nullptr;

					lstTdMarkers[modeTd] = timedomain_markers;
					auto elm = lstTdMarkers.begin();
					for (; elm != lstTdMarkers.end(); elm++)
					{
						assert(elm->first < jvxAcousticMeasure::JVX_PLOT_MODE_TD_LIMIT);
						if (
							(elm->second.markers_1.markers_x.size()) ||
							(elm->second.markers_2.markers_x.size()))
						{
							std::string tok_save = "MARKERS_EXPORT_";
							tok_save += marker_expr_td[elm->first];
							if (elm->second.markers_1.markers_x.size())
							{
								assert(elm->second.markers_1.markers_x.size() == elm->second.markers_1.markers_y.size());

								std::string tok_save_ll = tok_save + "_1";
								refInst.cpPtr->createAssignmentValueList(&cfgLst, tok_save_ll.c_str());
								jvxValue vv;
								jvxSize i;
								for (i = 0; i < elm->second.markers_1.markers_x.size(); i++)
								{
									vv.assign(elm->second.markers_1.markers_x[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 0, vv);
									vv.assign(elm->second.markers_1.markers_y[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 1, vv);
								}
								refInst.cpPtr->addSubsectionToSection(cfgDat, cfgLst);
							}
							if (elm->second.markers_2.markers_x.size())
							{
								assert(elm->second.markers_2.markers_x.size() == elm->second.markers_2.markers_y.size());

								std::string tok_save_ll = tok_save + "_2";
								refInst.cpPtr->createAssignmentValueList(&cfgLst, tok_save_ll.c_str());
								jvxValue vv;
								jvxSize i;
								for (i = 0; i < elm->second.markers_2.markers_x.size(); i++)
								{
									vv.assign(elm->second.markers_2.markers_x[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 0, vv);
									vv.assign(elm->second.markers_2.markers_y[i]);
									refInst.cpPtr->addAssignmentValueToList(cfgLst, 1, vv);
								}
								refInst.cpPtr->addSubsectionToSection(cfgDat, cfgLst);
							}
						}
					}

					jvxApiString astr;
					refInst.cpPtr->printConfiguration(cfgDat, &astr, true);

					jvx_writeContentToFile(fPath, astr.std_str());

					refInst.cpPtr->removeHandle(cfgDat);
					cfgDat = nullptr;

				}
				retRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR, refInst);
			}
			retInterface< IjvxToolsHost>(theHostRef, toolsHostRef);
		}
	}
}

void
jvxAcousticMeasure::marker_import_fd()
{
	jvxBool updatePlots = false;
	std::string folder = dataPlot1.file_folder_read;
	if (folder.empty())
	{
		folder = ".";
	}
	QString qstr = QFileDialog::getOpenFileName(
		nullptr,
		tr("Import Marker Specification"),
		folder.c_str(),
		"*.jma");

	if (!qstr.isEmpty())
	{
		IjvxToolsHost* toolsHostRef = reqInterface< IjvxToolsHost>(theHostRef);
		if (toolsHostRef)
		{
			refComp<IjvxConfigProcessor> refInst;
			refInst = reqRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR);
			if (refInst.cpPtr)
			{
				std::string fname = qstr.toLatin1().data();
				std::string content;
				jvx_readContentFromFile(
					fname.c_str(),
					content);

				jvxConfigData* cfgDat = nullptr;
				jvxErrorType res = refInst.cpPtr->parseTextField(content.c_str(), fname.c_str(), 0);
				if (res == JVX_NO_ERROR)
				{
					refInst.cpPtr->getConfigurationHandle(&cfgDat);
					if (cfgDat)
					{
						auto elm = lstSecMarkers.begin();
						for (; elm != lstSecMarkers.end(); elm++)
						{
							assert(elm->first < jvxAcousticMeasure::JVX_PLOT_MODE_SEC_LIMIT);
							if (
								(elm->second.markers_1.markers_x.size()) ||
								(elm->second.markers_2.markers_x.size()))
							{
								jvxConfigData* cfgSec = nullptr;
								std::string tok_save = "MARKERS_EXPORT_";
								tok_save += marker_expr_fd[elm->first];
								std::string tok_save_ll = tok_save + "_1";

								refInst.cpPtr->getReferenceEntryCurrentSection_name(
									cfgDat, &cfgSec,
									tok_save_ll.c_str());
								if (cfgSec)
								{
									jvxSize i;
									jvxSize numL = 0;
									jvxSize numEx = 0;
									jvxSize numEy = 0;
									refInst.cpPtr->getNumberValueLists(cfgSec, &numL);
									if (numL == 2)
									{
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEx, 0);
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEy, 0);
										if (numEx == numEy)
										{
											jvxApiValueList avlstx;
											jvxApiValueList avlsty;
											refInst.cpPtr->getValueList_id(cfgSec, &avlstx, 0);
											refInst.cpPtr->getValueList_id(cfgSec, &avlsty, 1);
											elm->second.markers_1.markers_x.resize(numEx);
											elm->second.markers_1.markers_y.resize(numEy);
											for (i = 0; i < numEx; i++)
											{
												avlstx.elm_at(i).toContent(&elm->second.markers_1.markers_x[i]);
												avlsty.elm_at(i).toContent(&elm->second.markers_1.markers_y[i]);
											}
										}
									}
								}

								tok_save_ll = tok_save + "_2";

								refInst.cpPtr->getReferenceEntryCurrentSection_name(
									cfgDat, &cfgSec,
									tok_save_ll.c_str());
								if (cfgSec)
								{
									jvxSize i;
									jvxSize numL = 0;
									jvxSize numEx = 0;
									jvxSize numEy = 0;
									refInst.cpPtr->getNumberValueLists(cfgSec, &numL);
									if (numL == 2)
									{
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEx, 0);
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEy, 0);
										if (numEx == numEy)
										{
											jvxApiValueList avlstx;
											jvxApiValueList avlsty;
											refInst.cpPtr->getValueList_id(cfgSec, &avlstx, 0);
											refInst.cpPtr->getValueList_id(cfgSec, &avlsty, 1);
											elm->second.markers_2.markers_x.resize(numEx);
											elm->second.markers_2.markers_y.resize(numEy);
											for (i = 0; i < numEx; i++)
											{
												avlstx.elm_at(i).toContent(&elm->second.markers_2.markers_x[i]);
												avlsty.elm_at(i).toContent(&elm->second.markers_2.markers_y[i]);
											}
										}
									}
								}
			
								if (modeSec == elm->first)
								{
									updatePlots = true;
									secondary_markers = lstSecMarkers[modeSec];
								}
							}
						}
					}
					refInst.cpPtr->removeHandle(cfgDat);
					cfgDat = nullptr;
				}
			}
			retRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR, refInst);
		}
		retInterface< IjvxToolsHost>(theHostRef, toolsHostRef);

		if (updatePlots)
		{
			// This plots the markers
			toggled_show_sec_marker1(secondary_markers.markers_1.show);
			toggled_show_sec_marker2(secondary_markers.markers_2.show);
		}
	}
}

void
jvxAcousticMeasure::marker_import_td()
{
	jvxBool updatePlots = false;
	std::string folder = dataPlot1.file_folder_read;
	if (folder.empty())
	{
		folder = ".";
	}
	QString qstr = QFileDialog::getOpenFileName(
		nullptr,
		tr("Import Marker Specification"),
		folder.c_str(),
		"*.jma");

	if (!qstr.isEmpty())
	{
		IjvxToolsHost* toolsHostRef = reqInterface< IjvxToolsHost>(theHostRef);
		if (toolsHostRef)
		{
			refComp<IjvxConfigProcessor> refInst;
			refInst = reqRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR);
			if (refInst.cpPtr)
			{
				std::string fname = qstr.toLatin1().data();
				std::string content;
				jvx_readContentFromFile(
					fname.c_str(),
					content);

				jvxConfigData* cfgDat = nullptr;
				jvxErrorType res = refInst.cpPtr->parseTextField(content.c_str(), fname.c_str(), 0);
				if (res == JVX_NO_ERROR)
				{
					refInst.cpPtr->getConfigurationHandle(&cfgDat);
					if (cfgDat)
					{
						auto elm = lstTdMarkers.begin();
						for (; elm != lstTdMarkers.end(); elm++)
						{
							assert(elm->first < jvxAcousticMeasure::JVX_PLOT_MODE_TD_LIMIT);
							if (
								(elm->second.markers_1.markers_x.size()) ||
								(elm->second.markers_2.markers_x.size()))
							{
								jvxConfigData* cfgSec = nullptr;
								std::string tok_save = "MARKERS_EXPORT_";
								tok_save += marker_expr_td[elm->first];
								std::string tok_save_ll = tok_save + "_1";

								refInst.cpPtr->getReferenceEntryCurrentSection_name(
									cfgDat, &cfgSec,
									tok_save_ll.c_str());
								if (cfgSec)
								{
									jvxSize i;
									jvxSize numL = 0;
									jvxSize numEx = 0;
									jvxSize numEy = 0;
									refInst.cpPtr->getNumberValueLists(cfgSec, &numL);
									if (numL == 2)
									{
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEx, 0);
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEy, 0);
										if (numEx == numEy)
										{
											jvxApiValueList avlstx;
											jvxApiValueList avlsty;
											refInst.cpPtr->getValueList_id(cfgSec, &avlstx, 0);
											refInst.cpPtr->getValueList_id(cfgSec, &avlsty, 1);
											elm->second.markers_1.markers_x.resize(numEx);
											elm->second.markers_1.markers_y.resize(numEy);
											for (i = 0; i < numEx; i++)
											{
												avlstx.elm_at(i).toContent(&elm->second.markers_1.markers_x[i]);
												avlsty.elm_at(i).toContent(&elm->second.markers_1.markers_y[i]);
											}
										}
									}
								}

								tok_save_ll = tok_save + "_2";

								refInst.cpPtr->getReferenceEntryCurrentSection_name(
									cfgDat, &cfgSec,
									tok_save_ll.c_str());
								if (cfgSec)
								{
									jvxSize i;
									jvxSize numL = 0;
									jvxSize numEx = 0;
									jvxSize numEy = 0;
									refInst.cpPtr->getNumberValueLists(cfgSec, &numL);
									if (numL == 2)
									{
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEx, 0);
										refInst.cpPtr->getValueListSize_id(cfgSec, &numEy, 0);
										if (numEx == numEy)
										{
											jvxApiValueList avlstx;
											jvxApiValueList avlsty;
											refInst.cpPtr->getValueList_id(cfgSec, &avlstx, 0);
											refInst.cpPtr->getValueList_id(cfgSec, &avlsty, 1);
											elm->second.markers_2.markers_x.resize(numEx);
											elm->second.markers_2.markers_y.resize(numEy);
											for (i = 0; i < numEx; i++)
											{
												avlstx.elm_at(i).toContent(&elm->second.markers_2.markers_x[i]);
												avlsty.elm_at(i).toContent(&elm->second.markers_2.markers_y[i]);
											}
										}
									}
								}

								if (modeTd == elm->first)
								{
									updatePlots = true;
									timedomain_markers = lstTdMarkers[modeTd];
								}
							}
						}
					}
					refInst.cpPtr->removeHandle(cfgDat);
					cfgDat = nullptr;
				}
			}
			retRefTool<IjvxConfigProcessor>(toolsHostRef, JVX_COMPONENT_CONFIG_PROCESSOR, refInst);
		}
		retInterface< IjvxToolsHost>(theHostRef, toolsHostRef);

		if (updatePlots)
		{
			// This plots the markers
			toggled_show_td_marker1(timedomain_markers.markers_1.show);
			toggled_show_td_marker2(timedomain_markers.markers_2.show);
		}
	}
}