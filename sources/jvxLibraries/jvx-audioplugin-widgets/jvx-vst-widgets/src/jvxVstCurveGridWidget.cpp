#include "jvx-vst-widgets.h"

namespace VSTGUI {

	jvxVstCurveGridWidget::jvxVstCurveGridWidget(const CRect& size, IjvxPluginWidgetClose* rep_close) : jvxVstGridWidget(size, rep_close)
	{

	}

	void 
		jvxVstCurveGridWidget::init_plot(jvxSize numValues, jvxSize num_curves,
			jvxCBitField updatePlots)
	{
		jvxSize i;
		std::vector<onePlotCurve> old_curves;
		if (
			(numValues != num)||(num_curves != num_p))
		{
			if (num)
			{
				JVX_DSP_SAFE_DELETE_FIELD(plot_x);
				plot_x = nullptr;
				for (i = 0; i < num_p; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD(curves[i].plot_y);
				}
				old_curves = curves;
				curves.clear();
			}
			num = 0;
			num_p = 0;

			if (numValues && num_curves)
			{
				num_p = num_curves;
				num = numValues;
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(plot_x, jvxData, num);
				curves.resize(num_p);
				
				for (i = 0; i < num_p; i++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(curves[i].plot_y, jvxData, num);
					if (i < old_curves.size())
					{
						curves[i].col = old_curves[i].col;
					}
				}
			}
		}
		plots_active = updatePlots;
	}
	
	jvxSize 
		jvxVstCurveGridWidget::num_plots()
	{
		return curves.size();
	}

	jvxErrorType
	jvxVstCurveGridWidget::update_plot_data(
		jvxData* plotx, 
		jvxData* ploty, 
		jvxSize numEntries, 
		jvxSize idx,
		VSTGUI::CColor col)
	{
		if (idx < num_p)
		{
			if (numEntries == 0)
			{
				curves[idx].col = col;
				return JVX_NO_ERROR;
			}
			
			if (num == numEntries)
			{
				if (plotx)
				{
					memcpy(plot_x, plotx, sizeof(jvxData) * num);
				}
				if(ploty)
				{
					memcpy(curves[idx].plot_y, ploty, sizeof(jvxData) * num);
					curves[idx].col = col;
				}
				return JVX_NO_ERROR;
			}
		}	
		return JVX_ERROR_INVALID_ARGUMENT;
	}


	jvxErrorType
	jvxVstCurveGridWidget::request_plot_data(jvxData** buf_ret, jvxSize* num_ret, jvxSize idx, jvxBool x_data)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (x_data)
		{
			if (idx == 0)
			{
				if (buf_ret)
				{
					*buf_ret = plot_x;
				}
				if (num_ret)
				{
					*num_ret = num;
				}
				res = JVX_NO_ERROR;
			}
		}
		else
		{ 
			if (idx < num_p)
			{
				if (buf_ret)
				{
					*buf_ret = curves[idx].plot_y;
				}
				if (num_ret)
				{
					*num_ret = num;
				}
				res = JVX_NO_ERROR;
			}
		}
		return res;
	}

	void 
	jvxVstCurveGridWidget::draw_inner(CDrawContext* context)
	{
		jvxSize i;

		for (i = 0; i < curves.size(); i++)
		{
			if (jvx_bitTest(plots_active, i))
			{
				this->plot_curve(context, plot_x, curves[i].plot_y, num, curves[i].col);
			}
		}
	}

	void
	jvxVstCurveGridWidget::plot_curve(CDrawContext* context,
		jvxData* plot_x, jvxData* plot_y, jvxSize num, CColor col)
	{
		jvxSize i;
		
		CPoint start0(transform_x(x_draw_min), transform_y(y_draw_min));
		CPoint stop0(transform_x(x_draw_max), transform_y(y_draw_max));
		
		context->setFrameColor(col);
		context->setLineWidth(2);
		
		/*
		//context->drawLine(CPoint(x_draw_min, y_draw_min), CPoint(x_draw_max, y_draw_max));		
		context->drawLine(start, stop);
		return;
		*/
		jvxData xOld;
		jvxData yOld;
		jvxData xN = (x_draw_max - x_draw_min) / (xMax - xMin);
		jvxData yN = (y_draw_max - y_draw_min) / (yMax - yMin);
		if(num > 0)
		{
			xOld = plot_x[0];
			xOld = JVX_MAX(xMin, JVX_MIN(xOld, xMax));
			xOld = (xOld - xMin) * xN + x_draw_min;
			yOld = plot_y[0];
			yOld = JVX_MAX(yMin, JVX_MIN(yOld, yMax));
			yOld = (yOld - yMin) * yN + y_draw_min;

			for (i = 1; i < num; i++)
			{
				jvxData tt0 = plot_x[i];
				tt0 = JVX_MAX(xMin, JVX_MIN(tt0, xMax));
				tt0 = (tt0 - xMin) * xN + x_draw_min;
								
				jvxData tt1 = plot_y[i];
				tt1 = JVX_MAX(yMin, JVX_MIN(tt1, yMax));
				tt1 = (tt1 - yMin) * yN + y_draw_min;
				CPoint start(transform_x(xOld), transform_y(yOld));
				CPoint stop(transform_x(tt0), transform_y(tt1));

				xOld = tt0;
				yOld = tt1;

				context->drawLine(start, stop);
				// context->drawLine(start0, stop0);
			}
		}		
	}
}
