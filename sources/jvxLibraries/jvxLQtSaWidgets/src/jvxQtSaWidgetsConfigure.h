#ifndef _H_JVXQTWIDGETSCONFIGURE_H__
#define _H_JVXQTWIDGETSCONFIGURE_H__

namespace jvxQtSaWidgetsConfigure
{
	typedef struct
	{
		jvxSize numPlots;
		jvxSize plotPixelsize;
		jvxSize fontPixelSize;
		std::string textX;
		std::string textY;
		std::string theTitle;
		jvxSize numPixelsPlot;
		jvxData epsyplot;
		jvxSize curveHeight;
		jvxSize ctrWidth;
		jvxSize ctrHeight;
		jvxSize axisPlotSize;
		jvxSize scale_maxh;
		jvxSize scale_maxw;
	} jvxQtWidgetsLinePlotConfigure;

	typedef struct
	{
		QColor colorTxt;
		int fontSize;
	} jvxQtNetworkConfigure;
};

#endif
