#include "jvx-qcp-qt-helpers.h"

void jvx_qcp_set_color_graph(
	QCustomPlot* qcp, jvxSize gId, QColor col,
	Qt::PenStyle ps, QCPGraph::LineStyle ls, QCPScatterStyle ss,
	jvxSize lWidth)
{
	QPen pen = qcp->graph((int)gId)->pen();
	QBrush br = pen.brush();
	br.setColor(col);
	pen.setBrush(br);
	pen.setWidthF(lWidth);
	pen.setStyle(ps);
	qcp->graph((int)gId)->setPen(pen);
	qcp->graph((int)gId)->setLineStyle(ls);
	qcp->graph((int)gId)->setScatterStyle(ss);
}