#ifndef __JVXVSTCURVEWIDGET_H__
#define __JVXVSTCURVEWIDGET_H__

#include "jvx-vst-widgets.h"

namespace VSTGUI {

	class jvxVstCurveGridWidget : public jvxVstGridWidget
	{
	protected:

		jvxSize num = 0;
		jvxData* plot_x = nullptr;

		jvxSize num_p = 0;
		struct onePlotCurve
		{
			jvxData* plot_y = nullptr;
			VSTGUI::CColor col;
		};
		std::vector<onePlotCurve> curves;
		jvxCBitField plots_active = 0;

	public:

		jvxVstCurveGridWidget(const CRect& size, IjvxPluginWidgetClose* rep_close);

		void init_plot(jvxSize numValues, jvxSize num_plots, jvxCBitField update_plots);
		jvxSize num_plots();

		jvxErrorType request_plot_data(jvxData** buf, jvxSize* num, jvxSize idx, jvxBool x_data);
		
		jvxErrorType update_plot_data(
			jvxData* plotx,
			jvxData* ploty,
			jvxSize numEntries,
			jvxSize idx,
			VSTGUI::CColor col);

		void draw_inner(CDrawContext* context)override;
	
	protected:
		void plot_curve(
			CDrawContext* context,
			jvxData* plot_x, jvxData* plot_y, 
			jvxSize num, CColor col);
	};
}

#endif