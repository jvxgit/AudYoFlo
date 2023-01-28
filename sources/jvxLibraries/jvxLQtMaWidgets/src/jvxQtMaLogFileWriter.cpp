#include "jvxQtMaLogFileWriter.h"

jvxQtMaLogFileWriter::jvxQtMaLogFileWriter() :QObject()
{
	idSelectLogToFile = -1;
	theComboBoxReference = NULL;
	theLogFileWriterWidget = NULL;
	widgetWrapper = NULL;
	widgetWrapper_report = NULL;
}

void
jvxQtMaLogFileWriter::update_window_subwidget()
{
	jvxSize i;
	if (theComboBoxReference)
	{
		theComboBoxReference->clear();
		theComboBoxReference->addItem("None");
		for (i = 0; i < entriesLogToFile.size(); i++)
		{
			theComboBoxReference->addItem((entriesLogToFile[i].accessString + jvxComponentIdentification_txt(entriesLogToFile[i].tp)).c_str());
		}
		theComboBoxReference->setCurrentIndex(idSelectLogToFile + 1);
	}
	if (widgetWrapper)
	{
		widgetWrapper->trigger_updateWindow("dynamic_logtofile", "");
	}
}

void
jvxQtMaLogFileWriter::update_window_subwidget_periodic()
{
	if (widgetWrapper)
	{
		widgetWrapper->trigger_updateWindow_periodic("dynamic_logtofile", "");
	}
}

void
jvxQtMaLogFileWriter::setWidgetWrapperReferences(CjvxQtSaWidgetWrapper* wPtr, IjvxQtSaWidgetWrapper_report* wPtrr)
{
	widgetWrapper = wPtr;
	widgetWrapper_report = wPtrr;
}

void
jvxQtMaLogFileWriter::setWidgetReferences(QComboBox* theCb, jvxQtSaLogFileWriter* theFW)
{
	theComboBoxReference = theCb;
	theLogFileWriterWidget = theFW;
	connect(theComboBoxReference, SIGNAL(activated(int)), this, SLOT(newSelection_logtofileaccess(int)));
}

void 
jvxQtMaLogFileWriter::inform_active_subwidget(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	jvxSize i;
	std::vector<std::string> propsCategories;
	jvx_findPropertyCategories_filter(propsCategories, theProps, "logtofile");
	for (i = 0; i < propsCategories.size(); i++)
	{
		onePossibleEntryLogFile newElm;
		newElm.accessString = propsCategories[i];
		newElm.tp = tp;
		newElm.theProps = theProps;
		entriesLogToFile.push_back(newElm);
	}
};

void
jvxQtMaLogFileWriter::inform_inactive_subwidget(const jvxComponentIdentification& tp, IjvxAccessProperties* theProps)
{
	if (idSelectLogToFile >= 0)
	{
		if (entriesLogToFile[idSelectLogToFile].tp == tp)
		{
			newSelection_logtofileaccess(0);
		}
	}

	std::vector<onePossibleEntryLogFile>::iterator elm = entriesLogToFile.begin();
	while (elm != entriesLogToFile.end())
	{
		while (elm != entriesLogToFile.end())
		{
			if (elm->tp == tp)
			{
				entriesLogToFile.erase(elm);
				elm = entriesLogToFile.begin();
				break;
			}
			elm++;
		}
	}
};

void
jvxQtMaLogFileWriter::newSelection_logtofileaccess(int sel)
{
	std::string propTargetNameStr;
	std::string idtag = "";
	std::string propname;
	std::vector<std::string> paramlst;


	if (widgetWrapper)
	{
		sel -= 1;
		if (sel != idSelectLogToFile)
		{
			if (idSelectLogToFile >= 0)
			{
				widgetWrapper->deassociateAutoWidgets("dynamic_logtofile");

				// Reconstruct original description text
				theLogFileWriterWidget->setAccessibleDescription(propTargetNameStr_reconstruct.c_str());
				idSelectLogToFile = -1;
			}

			if (sel >= 0)
			{
				QString propTargetName = theLogFileWriterWidget->accessibleDescription();
				propTargetNameStr_reconstruct = propTargetName.toLatin1().constData();
				jvx_parsePropertyLinkDescriptor(propTargetNameStr_reconstruct, idtag, propname, paramlst);
				propTargetNameStr = jvx_constructPropertyLinkDescriptor("dynamic_logtofile", entriesLogToFile[sel].accessString, paramlst);
				theLogFileWriterWidget->setAccessibleDescription(propTargetNameStr.c_str());
				widgetWrapper->associateAutoWidgets(static_cast<QWidget*>(theLogFileWriterWidget), 
					entriesLogToFile[sel].theProps,
					static_cast<IjvxQtSaWidgetWrapper_report*>(widgetWrapper_report), 
					"dynamic_logtofile", true, JVX_WW_WIDGET_SEARCH_FROM_ROOT);
				idSelectLogToFile = sel;
			}
		}
	}
}
