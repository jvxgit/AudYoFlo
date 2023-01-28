#ifndef __JVXSIMPLEVSTWIDGET_H__
#define __JVXSIMPLEVSTWIDGET_H__


#include "vstgui/lib/cview.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/ccolor.h"

#include "jvx.h"

namespace VSTGUI {

class jvxVstGridWidget : public CView, public jvxPluginWidget
{

protected:

	class grid
	{
	public:
		struct oneEntry
		{
			jvxData valueShown = 0;
			jvxData pos_axis = 0;
		};

		jvxData minX = 0;
		jvxData maxX = 10;
		jvxData minY = 0;
		jvxData maxY = 0;

		std::list<oneEntry> entries_x_posis;
		std::list<oneEntry> entries_y_posis;

		std::list<oneEntry> add_x_posis;
		std::list<oneEntry> add_y_posis;

		CColor col = kBlackCColor;
		int width = 1;
	};

	// We start with variable transform:
	jvxData pos_trans_leftlow_y = 0;
	jvxData pos_trans_leftlow_x = 0;
	jvxData fac_y = -1;
	jvxData fac_x = 1;
	CFontRef ftRef = kNormalFontVerySmall;
	CCoord cn = 0;
	CCoord cn2 = 0;
	jvxData x_draw_min = 0;
	jvxData x_draw_max = 0;
	jvxData y_draw_min = 0;
	jvxData y_draw_max = 0;
	grid grid_parameters;
	std::string labelx = "Labelx";
	std::string labely = "Labely";

	jvxData xMin = 0;
	jvxData xMax = 1;
	jvxData yMin = 0;
	jvxData yMax = 1;
	jvxSize numGridX = 2;
	jvxSize numGridY = 2;
	int nDigits = 0;
	jvxData space_marg_x = 1;
	jvxData space_marg_y = 1;
	// ==================================================
	// ==================================================

public:
	jvxVstGridWidget(const CRect& size, IjvxPluginWidgetClose* rep_close);
	void draw(CDrawContext* context) override;

	void update_margins(jvxData mm_x, jvxData mm_y);
	void update_labels(const std::string& label_x, const std::string& label_y);
	void update_limits(jvxData xMin, jvxData xMax, jvxData yMin, jvxData yMax);
	void update_grid_config(jvxSize numX, jvxSize numY, int numDig = 0);
	void add_single_grid(jvxData val_xy, jvxBool xAxis);
	void clear_single_grids(jvxBool xAxis);

	void trigger_redraw();

protected:
	
	/*
	void set_grid(
		jvxData minX, jvxData maxX,
		jvxData minY, jvxData maxY,
		const std::vector<jvxData>& entries_x,
		const std::vector<jvxData>& entries_y);
		*/
	void draw_background(
		CDrawContext* context, 
		const CColor& col, 
		jvxData pos_x0, jvxData pos_y0,
		jvxData pos_x1, jvxData pos_y1);
	
	void draw_textlabel(
		CDrawContext* context, 
		CFontRef ft, 
		const CColor& col, 
		jvxData pos_x,
		jvxData pos_y,
		const char* txt,
		jvxData rotate = 0,
		jvxBool pos_center_x = true,
		jvxBool pos_center_y = true);

	void draw_grid(
		CDrawContext* context,
		const CColor& col, 
		jvxData pos_x0, jvxData pos_y0,
		jvxData pos_x1, jvxData pos_y1,
		jvxBool ft_italic = true);

	virtual void draw_inner(CDrawContext* context) {};

	// =======================================================
	// 
	// =======================================================

protected:
	
	jvxData transform_y(jvxData pos_y);
	jvxData transform_x(jvxData pos_x);

	// Derive a uniform grid from min, max and number ticks
	void derive_grid(jvxSize num_xy, jvxData val_xy0, jvxData val_xy1, jvxBool xAxis);
	
	// Merge uniform grid with grid ticks specifically added
	void merge_single_grid(jvxBool xAxis);

	// Align the coordinates to show with the real coordinates
	void align_grid(jvxData pos_xy0, jvxData pos_xy1, jvxBool xAxis);

};


} // VSTGUI

#endif

