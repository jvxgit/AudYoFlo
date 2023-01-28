#include "jvx-vst-widgets.h"

namespace VSTGUI {

jvxVstGridWidget::jvxVstGridWidget(const CRect& size, IjvxPluginWidgetClose* rep_close): CView(size), jvxPluginWidget(rep_close)
{
	
}

// =======================================================================
// =======================================================================

void 
jvxVstGridWidget::draw(CDrawContext* context)
{
	VSTGUI::CRect vSize = getViewSize();
	auto viewPos = vSize.getTopLeft();
	jvxData sz_x = vSize.getWidth();
	jvxData sz_y = vSize.getHeight();
	CDrawContext::Transform t(*context, CGraphicsTransform().translate(viewPos));

	// We start with variable transform:
	pos_trans_leftlow_y = sz_y;
	pos_trans_leftlow_x = 0;
	fac_y = -1;
	fac_x = 1;
	ftRef = kNormalFontVerySmall;
	cn = ftRef->getSize();
	cn2 = cn * 0.5;

	x_draw_min = 1.5 * cn + space_marg_x;
	y_draw_min = 1.2 * cn + space_marg_y;
	x_draw_max = (sz_x - x_draw_min);
	y_draw_max = (sz_y - y_draw_min);


	// 1) Draw background
	draw_background(context, kBlackCColor, 0, 0, sz_x, sz_y);

	// 2) Draw field in front of background
	draw_background(context, kGreyCColor, x_draw_min, y_draw_min, x_draw_max, y_draw_max);

	// 3) draw grid
	draw_grid(context, kBlackCColor, x_draw_min, y_draw_min, x_draw_max, y_draw_max);

	/*
	context->setFrameColor(kWhiteCColor);
	context->drawLine(CPoint(x_draw_min, y_draw_min), CPoint(x_draw_max, y_draw_max));
	*/

	draw_textlabel(context, kNormalFontVerySmall, kWhiteCColor, (x_draw_max + sz_x)/2, (y_draw_max + y_draw_min)/2, labelx.c_str(), -90);
	draw_textlabel(context, kNormalFontVerySmall, kWhiteCColor, (x_draw_max + x_draw_min) / 2, (y_draw_max + sz_y) / 2, labely.c_str(), 0);

	draw_inner(context);

	// setDirty();
}

// =======================================================================
// =======================================================================

void 
jvxVstGridWidget::update_margins(jvxData mm_x = 1, jvxData mm_y = 1)
{
	space_marg_x = mm_x;
	space_marg_y = mm_y;
}

void
jvxVstGridWidget::update_labels(const std::string& label_x, const std::string& label_y)
{
	this->labelx = label_x;
	this->labely = label_y;
}

void
jvxVstGridWidget::update_limits(jvxData xMinArg, jvxData xMaxArg, jvxData yMinArg, jvxData yMaxArg)
{
	xMin = xMinArg;
	xMax = xMaxArg;
	yMin = yMinArg;
	yMax = yMaxArg;
}

void
jvxVstGridWidget::update_grid_config(jvxSize numX, jvxSize numY, int numDig)
{
	numGridX = numX;
	numGridY = numY;
	nDigits = numDig;
}

void
jvxVstGridWidget::trigger_redraw()
{

	derive_grid(numGridX, xMin, xMax, true);
	merge_single_grid(true);

	derive_grid(numGridY, yMin, yMax, false);
	merge_single_grid(false);

	// Trigger plot
	this->setDirty();
}

// =======================================================================
// =======================================================================

jvxData
jvxVstGridWidget::transform_y(jvxData pos_y)
{
	return (pos_trans_leftlow_y + fac_y * pos_y);
}

jvxData
jvxVstGridWidget::transform_x(jvxData pos_x)
{
	return (pos_trans_leftlow_x + fac_x * pos_x);
}

// =======================================================================
// =======================================================================

void
jvxVstGridWidget::draw_background(CDrawContext* context, 	
	const CColor& col,
	jvxData pos_x0, jvxData pos_y0, 
	jvxData pos_x1, jvxData pos_y1)
{
	VSTGUI::CRect bgrd;

	// Transform coordinates (up/down)
	jvxData pos_y_low_left = transform_y(pos_y0);
	jvxData pos_x_low_left = transform_x(pos_x0);
	jvxData pos_y_top_right = transform_y(pos_y1);
	jvxData pos_x_top_right = transform_x(pos_x1);

	// Draw the rect
	bgrd.setBottomLeft(CPoint(pos_x_low_left, pos_y_low_left));
	bgrd.setTopRight(CPoint(pos_x_top_right, pos_y_top_right));

	context->setFrameColor(col);
	context->setFillColor(col);
	context->drawRect(bgrd, kDrawFilled);

}

void 
jvxVstGridWidget::draw_textlabel(
	CDrawContext* context,
	CFontRef ft,
	const CColor& col,
	jvxData pos_x,
	jvxData pos_y,
	const char* txt, 
	jvxData rotate,
	jvxBool pos_center_x,
	jvxBool pos_center_y)
{
	std::string txts = txt;
	jvxData pos_xx = pos_x;
	jvxData pos_yy = pos_y;

	CCoord ccx = context->getStringWidth(VSTGUI::UTF8String(txt));
	CCoord ccy = ft->getSize();
	if (pos_center_y)
	{
		pos_yy = pos_y - ccy / 2 + 0.5; // I found the "0.5" in the drawString function if VST code
	}

	if (pos_center_x)
	{
		pos_xx = pos_x -ccx / 2 ;
	}
	jvxData pos_y_low_left = transform_y(pos_yy);
	jvxData pos_x_low_left = transform_x(pos_xx);
	jvxData pos_y_ctr = transform_y(pos_y - 1); // Somehow tuning factor
	jvxData pos_x_ctr = transform_x(pos_x);
	context->setFont(ft);
	context->setFontColor(col);
	CDrawContext::Transform t(*context, CGraphicsTransform().rotate(rotate, CPoint(pos_x_ctr, pos_y_ctr)));
	context->drawString(VSTGUI::UTF8String(txt), 
		CPoint(pos_x_low_left, pos_y_low_left));
}

void
jvxVstGridWidget::draw_grid(
	CDrawContext* context,
	const CColor& col,
	jvxData pos_x0, jvxData pos_y0,
	jvxData pos_x1, jvxData pos_y1,
	jvxBool ft_italic)
{
	context->setFrameColor(kWhiteCColor);
	context->setLineStyle(kLineOnOffDash);

	CFontRef rr = kNormalFontVerySmall;
	if (ft_italic)
	{
		rr->setStyle(kItalicFace);
	}
	
	// Align grid with real pixel positions
	align_grid(pos_x0, pos_x1, true);
	align_grid(pos_y0, pos_y1, false);

	for (const grid::oneEntry& elm : grid_parameters.entries_x_posis)
	{
		jvxData pos_y_low = transform_y(pos_y0);
		jvxData pos_y_high = transform_y(pos_y1);
		jvxData pos_x = transform_x(elm.pos_axis);
		context->drawLine(CPoint(pos_x, pos_y_low), CPoint(pos_x, pos_y_high));

		// The position will be transformed in draw_textlabel and must therefore not be transformed here!
		draw_textlabel(context, kNormalFontVerySmall, kWhiteCColor, elm.pos_axis, y_draw_min / 2,
			jvx_data2String(elm.valueShown, nDigits).c_str());
	}

	for (const grid::oneEntry& elm : grid_parameters.entries_y_posis)
	{
		jvxData pos_x_low = transform_x(pos_x0);
		jvxData pos_x_high = transform_x(pos_x1);
		jvxData pos_y = transform_y(elm.pos_axis);
		context->drawLine(CPoint(pos_x_low, pos_y), CPoint(pos_x_high, pos_y));

		// The position will be transformed in draw_textlabel and must therefore not be transformed here!
		draw_textlabel(context, kNormalFontVerySmall, kWhiteCColor, 
			x_draw_min / 2,
			elm.pos_axis,
			jvx_data2String(elm.valueShown, nDigits).c_str());
	}
}

void
jvxVstGridWidget::align_grid(jvxData pos_xy0, jvxData pos_xy1, jvxBool xAxis)
{
	std::list<grid::oneEntry>* lstPtr = nullptr;
	jvxData vMin, vMax;
	if (xAxis)
	{
		vMin = xMin;
		vMax = xMax;
		lstPtr = &grid_parameters.entries_x_posis;
	}
	else
	{
		vMin = yMin;
		vMax = yMax;
		lstPtr = &grid_parameters.entries_y_posis;
	}
	for (grid::oneEntry& elm : *lstPtr)
	{
		jvxData sf = (elm.valueShown - vMin) / (vMax - vMin);
		sf = JVX_MIN(sf, 1);
		sf = JVX_MAX(sf, 0);
		elm.pos_axis = pos_xy0 + (pos_xy1 - pos_xy0) * sf;
	}
}

void
jvxVstGridWidget::derive_grid(jvxSize num_xy, jvxData val_xy0, jvxData val_xy1, jvxBool xAxis)
{
	jvxSize cnt = 0;
	if(xAxis)
	{
		grid_parameters.entries_x_posis.resize(num_xy);
		for (grid::oneEntry& elm : grid_parameters.entries_x_posis)
		{
			elm.pos_axis = 0; // <- requires additional step to align with view parameters
			elm.valueShown = val_xy0 + (jvxData)cnt * (val_xy1 - val_xy0) / (jvxData)(num_xy - 1);
			cnt++;

		}
	}
	else
	{
		grid_parameters.entries_y_posis.resize(num_xy);
		for (grid::oneEntry& elm : grid_parameters.entries_y_posis)
		{
			elm.pos_axis = 0; // <- requires additional step to align with view parameters
			elm.valueShown = val_xy0 + (jvxData)cnt * (val_xy1 - val_xy0) / (jvxData)(num_xy - 1);
			cnt++;
		}
	}
}

void 
jvxVstGridWidget::merge_single_grid(jvxBool xAxis)
{
	if (xAxis)
	{
		for (const grid::oneEntry elmM : grid_parameters.add_x_posis)
		{
			auto elmI = grid_parameters.entries_x_posis.begin();
			while (
				(elmI != grid_parameters.entries_x_posis.end()) &&
				(elmI->valueShown < elmM.valueShown)
				)
			{
				elmI++;
			}
			grid_parameters.entries_x_posis.insert(elmI, elmM);
		}
	}
	else
	{
		for (const grid::oneEntry elmM : grid_parameters.add_y_posis)
		{
			auto elmI = grid_parameters.entries_y_posis.begin();
			while (
				(elmI != grid_parameters.entries_y_posis.end()) &&
				(elmI->valueShown < elmM.valueShown)
				)
			{
				elmI++;
			}
			grid_parameters.entries_y_posis.insert(elmI, elmM);
		}
	}
}

void
jvxVstGridWidget::add_single_grid(jvxData val_xy, jvxBool xAxis)
{
	grid::oneEntry nElm;
	nElm.pos_axis = 0;
	nElm.valueShown = val_xy;
	if (xAxis)
	{
		grid_parameters.add_x_posis.push_back(nElm);
	}
	else
	{
		grid_parameters.add_y_posis.push_back(nElm);
	}
}

void
jvxVstGridWidget::clear_single_grids(jvxBool xAxis)
{
	if (xAxis)
	{
		grid_parameters.add_x_posis.clear();
	}
	else
	{
		grid_parameters.add_y_posis.clear();
	}
}

} // VSTGUI
