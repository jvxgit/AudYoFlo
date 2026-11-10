#ifndef _JVX_PLOT_OPTIONS_QCPPLOTOPTIONS_H__
#define _JVX_PLOT_OPTIONS_QCPPLOTOPTIONS_H__

class oneStyleOption
{
public:
	oneStyleOption(const std::string& nmArg, QCPGraph::LineStyle styleArg)
	{
		nm = nmArg;
		style = styleArg;
	};
	std::string nm;
	QCPGraph::LineStyle style;
} ;	

class oneMarkerOption
{
public:
	oneMarkerOption(const std::string& nmArg, QCPScatterStyle::ScatterShape markerArg)
	{
		nm = nmArg;
		marker = markerArg;
	};

	std::string nm;
	QCPScatterStyle::ScatterShape marker;
};

#endif
