#ifndef __JVX_QCP_QT_HELPERS_H__
#define __JVX_QCP_QT_HELPERS_H__

#include "jvx.h"
#include "qcustomplot.h"

void jvx_qcp_set_color_graph(
	QCustomPlot* qcp, jvxSize gId = 0, QColor col = Qt::black, Qt::PenStyle ps = Qt::SolidLine,
	QCPGraph::LineStyle ls = QCPGraph::lsLine, QCPScatterStyle ss = QCPScatterStyle::ssCircle,
	jvxSize lWidth = 1);
	
#endif
	
