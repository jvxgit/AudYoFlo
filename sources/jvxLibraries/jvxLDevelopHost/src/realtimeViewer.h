#ifndef __REALTIMEVIEWER_H__
#define __REALTIMEVIEWER_H__

#include "jvx.h"

#include "ui_realtimeViewer.h"

#include "common/CjvxRealtimeViewer.h"

#include <QtCore/QTimer>

#include "realtimeViewer_helpers.h"

class CrealtimeViewer: public QWidget, public Ui::Form_realtimeViewer, public CjvxRealtimeViewer
{

private:

	struct
	{
		std::string description;
		jvxSize idSelectTab;
	} selectionUser;

	struct
	{
		IjvxHost* theHostRef;
		IjvxReport* report;
	} references;

	int period_ms;

	struct
	{
		jvxTimeStampData timeBase;
		jvxData deltaT_period;
		jvxData deltaT_run;

		jvxTick timeStamp_enter;
		jvxTick timeStamp_leave;

	} timing;

	QTimer* myTimer;

	jvxBool autoStart;

	jvxRealtimeViewerType configurationType;
	jvxComponentIdentification* tpAll;
	Q_OBJECT

public:
	
	CrealtimeViewer(QWidget* parent, jvxRealtimeViewerType cfg, jvxComponentIdentification* tpAll);
	~CrealtimeViewer();

	void setPeriod_ms(jvxInt32 period_ms);

	void init();
	void setHostRef(IjvxHost* theHostRef, IjvxReport* report);


	void updateWindow_insertTab(jvxRealtimeViewerType tp, int idInsert);
	void updateWindow_removeTab(int idRemove);

	void updateWindow_destroy();

	// fullUpdate allows to set the values only OR to also read lists etc for modification of UI element contents
	void updateWindow_update(jvxBool fullUpdate);
	void updateWindow_rebuild(jvxSize selectAfterRefresh);
	void updateWindow_content();

	void updateWindow();
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost);

	jvxErrorType put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* theReader, jvxHandle* ir, IjvxHost* theHost, 
		const char* fName, int lineno, std::vector<std::string>& warnings);

	void createAllWidgetsFromConfiguration();
	void updateAllWidgetsOnStateChange();

typedef jvxErrorType (*initPrivateDataItem)();

signals:

public slots:
    void newSelectionButton_addView();
    void newSelectionButton_remove();
    void newSelectionButton_start();
    void newSelectionButton_stop();
	void newSelectionTab(int id);

	void timerExpired();
	void newText_name();
	void toggledAutoStart(bool);

};

#endif
	
