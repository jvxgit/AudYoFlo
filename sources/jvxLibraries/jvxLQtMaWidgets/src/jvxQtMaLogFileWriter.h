#ifndef __jvxQtMaWidgetLogFile_H__
#define __jvxQtMaWidgetLogFile_H__

#include <QWidget>
#include <QComboBox>
#include "jvx.h"
#include "jvxQtSaLogFileWriter.h"
#include "CjvxQtSaWidgetWrapper.h"

class jvxQtMaLogFileWriter: public QObject
{
protected:
	typedef struct
	{
		jvxComponentIdentification tp;
		IjvxAccessProperties* theProps;
		std::string accessString;
	} onePossibleEntryLogFile;

	std::vector<onePossibleEntryLogFile> entriesLogToFile;
	jvxInt32 idSelectLogToFile;
	std::string propTargetNameStr_reconstruct;

	QComboBox* theComboBoxReference;
	jvxQtSaLogFileWriter* theLogFileWriterWidget;
	CjvxQtSaWidgetWrapper* widgetWrapper;
	IjvxQtSaWidgetWrapper_report* widgetWrapper_report;

	Q_OBJECT
public:
	jvxQtMaLogFileWriter();

	void update_window_subwidget();
	void update_window_subwidget_periodic();
	void setWidgetWrapperReferences(CjvxQtSaWidgetWrapper* wPtr, IjvxQtSaWidgetWrapper_report* wPtrr);
	void setWidgetReferences(QComboBox* theCb, jvxQtSaLogFileWriter* theFW);
	void inform_active_subwidget(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);
	void inform_inactive_subwidget(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps);

public slots:

	void newSelection_logtofileaccess(int sel);
};

#endif