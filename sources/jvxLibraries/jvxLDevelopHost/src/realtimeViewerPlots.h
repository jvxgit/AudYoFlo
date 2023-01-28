#ifndef __REALTIMEVIEWERPLOTS_H__
#define __REALTIMEVIEWERPLOTS_H__

#include "jvx.h"

#include "ui_realtimeViewerPlots.h"
#include "common/CjvxRealtimeViewer.h"
#include "realtimeViewer_base.h"

class realtimeViewerPlots: public QWidget, public Ui::Form_realtimeViewerPlots, public realtimeViewer_base
{
	struct
	{
		jvxComponentIdentification tpV;
		int propertyId;
	} selectionUser;
	jvxComponentIdentification* tpAll;
	Q_OBJECT

public:
	
	realtimeViewerPlots(QWidget* parent, CjvxRealtimeViewer* theRef, IjvxHost* hostRef, std::string& descr, jvxSize idInsert, IjvxReport* report,
		jvxComponentIdentification* tpAll);

	// Two functions for init and removal of subcomponents
	void cleanBeforeDelete();
	void init();

	void removeMe_delayed(jvxSize sectionId, jvxSize groupId, jvxSize itemId);

	void updateWindow();

signals:
	void emit_removeMe_inThread(jvxSize sectionId, jvxSize groupId, jvxSize itemId);
	void emit_updateWindow_redraw_all();

public slots:

	void updateWindow_redraw_all();
	void removeMe_inThread(jvxSize sectionId, jvxSize groupId, jvxSize itemId);

	void buttonPushed_add_lin_plot();
	void buttonPushed_add_pol_plot();
	void buttonPushed_add_circ_plot();
	void buttonPushed_rem_current();

	void newText_description();
	void newSelection_tab(int id);
};

#endif