#include "jvxAcousticMeasure.h"

// =============================================================================================
#define TIMEOUT_SINGLECLICK 100000
#define TIMEOUT_LONGCLICK 1000000

void
jvxAcousticMeasure::mouse_press_td(QMouseEvent* event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_timedomain->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_timedomain->yAxis->pixelToCoord(pt.y());

	reset_marker_td_edit();
	if ((mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM) ||
		(mouseMode == JVX_MOUSE_MODE_MEASURE) ||
		(mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1) ||
		(mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2))
	{
		td.start_tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
		if (event->button() == Qt::RightButton)
		{
			td.start_select = true;
			td.start_x = pos_x;
			td.start_y = pos_y;
		}
		else
		{
			td.start_select = true;
			td.start_x = pos_x;
			td.start_y = pos_y;
		}
	}
}

void
jvxAcousticMeasure::mouse_release_td(QMouseEvent* event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_timedomain->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_timedomain->yAxis->pixelToCoord(pt.y());

	jvxTick stop_tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	jvxTick deltaT = (stop_tick - td.start_tick);
	if (td.start_tick == 0)
	{
		// Invalid click
		return;
	}

	td.start_tick = 0;
	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		if (event->button() == Qt::RightButton)
		{
			if (td.start_select)
			{
				if (td.autox == false)
				{
					jvxData deltax = pos_x - td.start_x;
					td.minx -= deltax;
					td.maxx -= deltax;
					verify_min_gap(td.minx, td.maxx);
				}
				if (td.autoy == false)
				{
					jvxData deltay = pos_y - td.start_y;
					td.miny -= deltay;
					td.maxy -= deltay;
					verify_min_gap(td.miny, td.maxy);
				}

				td.start_select = false;
				replot_diagrams(true, true);
				this->update_window();
			}
		}
		else
		{
			if (td.start_select)
			{
				jvxData minx = JVX_MIN(td.start_x, pos_x);
				jvxData maxx = JVX_MAX(td.start_x, pos_x);
				jvxData miny = JVX_MIN(td.start_y, pos_y);
				jvxData maxy = JVX_MAX(td.start_y, pos_y);

				td.autox = false;
				td.autoy = false;
				td.minx = minx;
				td.maxx = maxx;
				td.miny = miny;
				td.maxy = maxy;
				td.start_select = false;

				verify_min_gap(td.minx, td.maxx);
				verify_min_gap(td.miny, td.maxy);

				replot_diagrams(true, true);
				this->update_window();
			}
		}
		// std::cout << "Left Mouse Release Event at position [" << pos_x << ", " << pos_y << "]" << std::endl;
	}
	else if (mouseMode == JVX_MOUSE_MODE_MEASURE)
	{
		if (td.start_select)
		{
			if (event->button() == Qt::RightButton)
			{
				jvxData deltax = fabs(td.start_x - pos_x);
				label_s0_td->setText("--Meas X--");
				label_s1_td->setText(jvx_data2String(td.start_x, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s2_td->setText(jvx_data2String(pos_x, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s3_td->setText(jvx_data2String(deltax, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			}
			else
			{
				jvxData deltay = fabs(td.start_y - pos_y);
				label_s0_td->setText("--Meas Y--");
				label_s1_td->setText(jvx_data2String(td.start_y, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s2_td->setText(jvx_data2String(pos_y, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s3_td->setText(jvx_data2String(deltay, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
			}
			td.start_select = false;
		}
	}
	else
	{

		if (
			(deltaT > TIMEOUT_SINGLECLICK) &&
			(deltaT < TIMEOUT_LONGCLICK))
		{
			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
			{
				if (event->button() == Qt::RightButton)
				{
					removeSingleMarker(pos_x, timedomain_markers.markers_1.markers_x,
						timedomain_markers.markers_1.markers_y,
						qcp_timedomain, 2);
				}
				else
				{
					timedomain_markers.markers_1.markers_x.push_back(pos_x);
					timedomain_markers.markers_1.markers_y.push_back(pos_y);

					updateMarkerPlots(timedomain_markers.markers_1.markers_x,
						timedomain_markers.markers_1.markers_y,
						qcp_timedomain, 2);
				}
			}

			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2)
			{
				if (event->button() == Qt::RightButton)
				{
					removeSingleMarker(pos_x, timedomain_markers.markers_2.markers_x,
						timedomain_markers.markers_2.markers_y,
						qcp_timedomain, 3);
				}
				else
				{
					timedomain_markers.markers_2.markers_x.push_back(pos_x);
					timedomain_markers.markers_2.markers_y.push_back(pos_y);

					updateMarkerPlots(timedomain_markers.markers_2.markers_x,
						timedomain_markers.markers_2.markers_y,
						qcp_timedomain, 3);
				}
			}
		}

		if (deltaT > TIMEOUT_LONGCLICK)
		{
			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
			{
				if (event->button() == Qt::RightButton)
				{
					activate_marker_edit(JVX_EDIT_POINT_TD_MARKER1,
						pos_x, timedomain_markers.markers_1.markers_x,
						timedomain_markers.markers_1.markers_y);
				}
				else
				{
					moveSingleMarker(td.start_x, td.start_y,
						pos_x, pos_y,
						timedomain_markers.markers_1.markers_x,
						timedomain_markers.markers_1.markers_y,
						qcp_timedomain, 2);
				}
			}

			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2)
			{
				if (event->button() == Qt::RightButton)
				{
					activate_marker_edit(JVX_EDIT_POINT_TD_MARKER2,
						pos_x, timedomain_markers.markers_2.markers_x,
						timedomain_markers.markers_2.markers_y);
				}
				else
				{
					moveSingleMarker(td.start_x, td.start_y,
						pos_x, pos_y,
						timedomain_markers.markers_2.markers_x,
						timedomain_markers.markers_2.markers_y,
						qcp_timedomain, 3);
				}
			}
		}
	}
}

void
jvxAcousticMeasure::mouse_move_td(QMouseEvent* event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_timedomain->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_timedomain->yAxis->pixelToCoord(pt.y());

	std::string positxt = "[" + jvx_data2String(pos_x, JVX_DATA_2STRING_CONST_FORMAT_G) + ", " + jvx_data2String(pos_y, JVX_DATA_2STRING_CONST_FORMAT_G) + "]";
	label_posi_td->setText(positxt.c_str());
}

void
jvxAcousticMeasure::mouse_dblclick_td(QMouseEvent* event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_timedomain->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_timedomain->yAxis->pixelToCoord(pt.y());
	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		td.start_select = false;
		td.autox = true;
		td.autoy = true;
		replot_diagrams(true, true);
		this->update_window();
	}
	if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
	{
	}
	//std::cout << "Mouse DblClick Event at position [" << pos_x << ", " << pos_y << "]" << std::endl;
}

void
jvxAcousticMeasure::mouse_wheel_td(QWheelEvent* event)
{
	QPointF pt = event->position();
	jvxData pos_x = qcp_timedomain->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_timedomain->yAxis->pixelToCoord(pt.y());

	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		int dd = event->angleDelta().y();

		if (td.autox == false)
		{
			jvxData delta_max = td.maxx - pos_x;
			jvxData delta_min = pos_x - td.minx;
			if (dd > 0)
			{
				delta_max *= 2.0;
				delta_min *= 2.0;
			}
			else
			{
				delta_max *= 0.5;
				delta_min *= 0.5;
			}
			td.maxx = pos_x + delta_max;
			td.minx = pos_x - delta_min;
			verify_min_gap(td.minx, td.maxx);
		}
		if (td.autoy == false)
		{
			jvxData delta_max = td.maxy - pos_y;
			jvxData delta_min = pos_y - td.miny;
			if (dd > 0)
			{
				delta_max *= 2.0;
				delta_min *= 2.0;
			}
			else
			{
				delta_max *= 0.5;
				delta_min *= 0.5;
			}
			td.maxy = pos_y + delta_max;
			td.miny = pos_y - delta_min;
			verify_min_gap(td.miny, td.maxy);
		}
		replot_diagrams(true, true);
		this->update_window();
	}
}

// ========================================================================

void
jvxAcousticMeasure::mouse_press_sec(QMouseEvent *event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_secondary->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_secondary->yAxis->pixelToCoord(pt.y());

	reset_marker_sec_edit();
	if ((mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM) ||
		(mouseMode == JVX_MOUSE_MODE_MEASURE) ||
		(mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1) ||
		(mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2))
	{
		sec.start_tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
		if (event->button() == Qt::RightButton)
		{
			sec.start_select = true;
			sec.start_x = pos_x;
			sec.start_y = pos_y;
		}
		else
		{
			sec.start_select = true;
			sec.start_x = pos_x;
			sec.start_y = pos_y;
		}
	}
}

void
jvxAcousticMeasure::mouse_release_sec(QMouseEvent *event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_secondary->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_secondary->yAxis->pixelToCoord(pt.y());

	jvxTick stop_tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
	jvxTick deltaT = (stop_tick - sec.start_tick);
	if (sec.start_tick == 0)
	{
		// Invalid click
		return;
	}

	sec.start_tick = 0;

	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		if (event->button() == Qt::RightButton)
		{
			if (sec.start_select)
			{
				if (sec.autox == false)
				{
					jvxData deltax = pos_x - sec.start_x;
					sec.minx -= deltax;
					sec.maxx -= deltax;
					verify_min_gap(sec.minx, sec.maxx);
				}
				if (sec.autoy == false)
				{
					jvxData deltay = pos_y - sec.start_y;
					sec.miny -= deltay;
					sec.maxy -= deltay;
					verify_min_gap(sec.miny, sec.maxy);
				}

				sec.start_select = false;
				replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
				this->update_window();
			}
		}
		else
		{
			if (sec.start_select)
			{
				jvxData minx = JVX_MIN(sec.start_x, pos_x);
				jvxData maxx = JVX_MAX(sec.start_x, pos_x);
				jvxData miny = JVX_MIN(sec.start_y, pos_y);
				jvxData maxy = JVX_MAX(sec.start_y, pos_y);

				sec.autox = false;
				sec.autoy = false;
				sec.minx = minx;
				sec.maxx = maxx;
				sec.miny = miny;
				sec.maxy = maxy;
				sec.start_select = false;
				verify_min_gap(sec.minx, sec.maxx);
				verify_min_gap(sec.miny, sec.maxy);

				replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
				this->update_window();
			}
			// std::cout << "Left Mouse Release Event at position [" << pos_x << ", " << pos_y << "]" << std::endl;
		}
	}
	else if (mouseMode == JVX_MOUSE_MODE_MEASURE)
	{
		if (sec.start_select)
		{
			if (event->button() == Qt::RightButton)
			{
				jvxData deltax = fabs(sec.start_x - pos_x);
				label_s0_sec->setText("--Meas X--");
				label_s1_sec->setText(jvx_data2String(sec.start_x, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s2_sec->setText(jvx_data2String(pos_x, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s3_sec->setText(("->" + jvx_data2String(deltax, JVX_DATA_2STRING_CONST_FORMAT_G)).c_str());
			}
			else
			{
				jvxData deltay = fabs(sec.start_y - pos_y);
				label_s0_sec->setText("--Meas Y--");
				label_s1_sec->setText(jvx_data2String(sec.start_y, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s2_sec->setText(jvx_data2String(pos_y, JVX_DATA_2STRING_CONST_FORMAT_G).c_str());
				label_s3_sec->setText(("->" + jvx_data2String(deltay, JVX_DATA_2STRING_CONST_FORMAT_G)).c_str());
			}
			sec.start_select = false;
		}
	}
	else
	{
		if (
			(deltaT > TIMEOUT_SINGLECLICK) &&
			(deltaT < TIMEOUT_LONGCLICK))
		{
			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
			{
				if (event->button() == Qt::RightButton)
				{
					removeSingleMarker(pos_x, secondary_markers.markers_1.markers_x,
						secondary_markers.markers_1.markers_y,
						qcp_secondary, 2);
				}
				else
				{
					secondary_markers.markers_1.markers_x.push_back(pos_x);
					secondary_markers.markers_1.markers_y.push_back(pos_y);

					updateMarkerPlots(secondary_markers.markers_1.markers_x,
						secondary_markers.markers_1.markers_y,
						qcp_secondary, 2);
				}
			}

			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2)
			{
				if (event->button() == Qt::RightButton)
				{
					removeSingleMarker(pos_x, secondary_markers.markers_2.markers_x,
						secondary_markers.markers_2.markers_y,
						qcp_secondary, 3);
				}
				else
				{
					secondary_markers.markers_2.markers_x.push_back(pos_x);
					secondary_markers.markers_2.markers_y.push_back(pos_y);

					updateMarkerPlots(secondary_markers.markers_2.markers_x,
						secondary_markers.markers_2.markers_y,
						qcp_secondary, 3);
				}
			}
		}

		if (deltaT > TIMEOUT_LONGCLICK)
		{
			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
			{
				if (event->button() == Qt::RightButton)
				{
					activate_marker_edit(JVX_EDIT_POINT_SEC_MARKER1,
						pos_x, secondary_markers.markers_1.markers_x,
						secondary_markers.markers_1.markers_y);
				}
				else
				{
					moveSingleMarker(sec.start_x, sec.start_y,
						pos_x, pos_y,
						secondary_markers.markers_1.markers_x,
						secondary_markers.markers_1.markers_y,
						qcp_secondary, 2);
				}
			}

			if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2)
			{
				if (event->button() == Qt::RightButton)
				{
					activate_marker_edit(JVX_EDIT_POINT_SEC_MARKER2,
						pos_x, secondary_markers.markers_2.markers_x,
						secondary_markers.markers_2.markers_y);
				}
				else
				{
					moveSingleMarker(sec.start_x, sec.start_y,
						pos_x, pos_y,
						secondary_markers.markers_2.markers_x,
						secondary_markers.markers_2.markers_y,
						qcp_secondary, 3);
				}
			}
		}
	}
}

void
jvxAcousticMeasure::mouse_move_sec(QMouseEvent *event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_secondary->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_secondary->yAxis->pixelToCoord(pt.y());
	std::string positxt = "[" + jvx_data2String(pos_x, JVX_DATA_2STRING_CONST_FORMAT_G) + ", " + jvx_data2String(pos_y, JVX_DATA_2STRING_CONST_FORMAT_G) + "]";
	label_posi_sec->setText(positxt.c_str());
}

void
jvxAcousticMeasure::mouse_dblclick_sec(QMouseEvent *event)
{
	QPoint pt = event->pos();
	jvxData pos_x = qcp_secondary->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_secondary->yAxis->pixelToCoord(pt.y());
	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		sec.start_select = false;
		sec.autox = true;
		sec.autoy = true;
		replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
		this->update_window();
	}
	//std::cout << "Mouse DblClick Event at position [" << pos_x << ", " << pos_y << "]" << std::endl;
}

void
jvxAcousticMeasure::mouse_wheel_sec(QWheelEvent *event)
{
	QPointF pt = event->position();
	jvxData pos_x = qcp_secondary->xAxis->pixelToCoord(pt.x());
	jvxData pos_y = qcp_secondary->yAxis->pixelToCoord(pt.y());
	int dd = event->angleDelta().y();

	if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
	{
		if (sec.autox == false)
		{
			jvxData delta_max = sec.maxx - pos_x;
			jvxData delta_min = pos_x - sec.minx;
			if (dd > 0)
			{
				delta_max *= 2.0;
				delta_min *= 2.0;
			}
			else
			{
				delta_max *= 0.5;
				delta_min *= 0.5;
			}
			sec.maxx = pos_x + delta_max;
			sec.minx = pos_x - delta_min;
			verify_min_gap(sec.minx, sec.maxx);
		}
		if (sec.autoy == false)
		{
			jvxData delta_max = sec.maxy - pos_y;
			jvxData delta_min = pos_y - sec.miny;
			if (dd > 0)
			{
				delta_max *= 2.0;
				delta_min *= 2.0;
			}
			else
			{
				delta_max *= 0.5;
				delta_min *= 0.5;
			}
			sec.maxy = pos_y + delta_max;
			sec.miny = pos_y - delta_min;
			verify_min_gap(sec.miny, sec.maxy);
		}
		replot_freqdomain(false, NULL, NULL, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED);
	}
	this->update_window();
}

void 
jvxAcousticMeasure::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_1)
	{
		if (mouseMode == JVX_MOUSE_MODE_EDIT_DIAGRAM)
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
		}
		else
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
			switchMouseMode(JVX_MOUSE_MODE_EDIT_DIAGRAM);
		}
	}
	if (event->key() == Qt::Key_2)
	{
		if (mouseMode == JVX_MOUSE_MODE_MEASURE)
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
		}
		else
		{ 
			switchMouseMode(JVX_MOUSE_MODE_NONE);
			switchMouseMode(JVX_MOUSE_MODE_MEASURE);
		}
	}
	if (event->key() == Qt::Key_3)
	{
		if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER1)
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
		}
		else
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
			switchMouseMode(JVX_MOUSE_MODE_EDIT_MARKER1);
		}
	}
	if (event->key() == Qt::Key_4)
	{
		if (mouseMode == JVX_MOUSE_MODE_EDIT_MARKER2)
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
		}
		else
		{
			switchMouseMode(JVX_MOUSE_MODE_NONE);
			switchMouseMode(JVX_MOUSE_MODE_EDIT_MARKER2);
		}
	}
}

/*
void
jvxAcousticMeasure::keyReleaseEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_1)
	{
		std::cout << "Hier Aus" << std::endl;
		switchMouseMode(JVX_MOUSE_MODE_NONE);
	}
}
*/
void
jvxAcousticMeasure::switchMouseMode(jvxPlotMouseMode newMode)
{
	QPalette pal;
	mouseMode = newMode;
	switch (mouseMode)
	{
	case JVX_MOUSE_MODE_NONE:
		td.start_select = false;
		sec.start_select = false;
		label_td_mode->setText("");
		pal = frame_meas_td->palette();
		pal.setColor(QPalette::Window, Qt::white);
		frame_meas_td->setPalette(pal);
		label_s0_td->setText("");
		label_s1_td->setText("");
		label_s2_td->setText("");
		label_s3_td->setText("");
		label_sec_mode->setText("");
		pal = frame_meas_sec->palette();
		pal.setColor(QPalette::Window, Qt::white);
		frame_meas_sec->setPalette(pal);
		label_s0_sec->setText("");
		label_s1_sec->setText("");
		label_s2_sec->setText("");
		label_s3_sec->setText("");
		label_explain->setText("");
		break;
	case JVX_MOUSE_MODE_EDIT_DIAGRAM:
		label_td_mode->setText("-edit-");
		label_sec_mode->setText("-edit-");
		label_explain->setText("Edit Mode: Left Click - Sel Region : Left Dbl Click - zoom out : Right Click - Move View");
		break;
	case JVX_MOUSE_MODE_MEASURE:
		pal = frame_meas_td->palette();
		pal.setColor(QPalette::Window, Qt::lightGray);
		frame_meas_td->setPalette(pal);
		label_td_mode->setText("-measure-");
		pal = frame_meas_sec->palette();
		pal.setColor(QPalette::Window, Qt::lightGray);
		frame_meas_sec->setPalette(pal);
		label_sec_mode->setText("-measure-");
		label_explain->setText("Measure Mode: Left Click - Neasure Distances");
		break;
	case JVX_MOUSE_MODE_EDIT_MARKER1:
		label_td_mode->setText("-marker1-");
		label_sec_mode->setText("-marker1-");
		label_explain->setText("Marker 1 Mode: Left Click Short - Add Marker : Left Click Long - Move Point : Right Click Short - Remove Marker : Right Clck Long - Activate Edit");
		break;
	case JVX_MOUSE_MODE_EDIT_MARKER2:
		label_td_mode->setText("-marker2-");
		label_sec_mode->setText("-marker2-");
		label_explain->setText("Marker 2 Mode: Left Click Short - Add Marker : Left Click Long - Move Point : Right Click Short - Remove Marker : Right Clck Long - Activate Edit");
		break;
	}
}

