#include "CjvxQtSaWidgetWrapper.h"
#include "CjvxSaWrapperElementAbstractButton.h"
#include "CjvxSaWrapperElementComboBox.h"
#include "CjvxSaWrapperElementSlider.h"
#include "CjvxSaWrapperElementProgressBar.h"
#include "CjvxSaWrapperElementLineEdit.h"
#include "CjvxSaWrapperElementListWidget.h"
#include "CjvxSaWrapperElementLabel.h"
#include "CjvxSaWrapperElementFrame.h"
#include "CjvxSaWrapperElementPushButton.h"
#include "CjvxMaWrapperElementTreeWidget.h"
#include "CjvxSaWrapperElementJvxLogFileWriter.h"
#include "CjvxSaWrapperElementJvxNetworkMasterDevice.h"
#include "CjvxSaWrapperElementJvxNetworkSlaveDevice.h"
#include "CjvxSaWrapperElementJvxNetworkTechnology.h"

void
jvx_init_keymapper_min(keyValueList& theElm)
{
	jvxInt32 defInt = 0;
	jvxData defFloat = 0;
	jvxBool defBool = false;
	std::string deps = "";

	defInt = JVX_WW_REPORT_ELEMENT;
	theElm.registerElement("REPORT", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	theElm.registerTranslator("REPORT", "JVX_WW_REPORT_ELEMENT", JVX_WW_REPORT_ELEMENT);
	theElm.registerTranslator("REPORT", "JVX_WW_REPORT_LOCAL", JVX_WW_REPORT_LOCAL);
	theElm.registerTranslator("REPORT", "JVX_WW_REPORT_LOCAL_GLOBAL", JVX_WW_REPORT_LOCAL_GLOBAL);
	theElm.registerTranslator("REPORT", "JVX_WW_REPORT_GLOBAL", JVX_WW_REPORT_GLOBAL);

	deps = ""; 
	theElm.registerElement("DEPENDS", JVX_WW_KEY_VALUE_TYPE_STRING, &deps);

	defBool = false;
	theElm.registerElement("REPORT_GET", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	defBool = true;
	theElm.registerElement("CONTENT_ONLY", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	defBool = false;
	theElm.registerElement("RTUPDATE", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	defBool = false;
	theElm.registerElement("VERBOSE", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	defBool = false;
	theElm.registerElement("TTIPDEBUG", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);	

	defInt = 0;
	theElm.registerElement("GROUPID", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);

	defInt = 0;
	theElm.registerElement("GROUPID_EMIT", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);

	defInt = 1;
	theElm.registerElement("DBG_LEVEL", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);

	defInt = 0;
	theElm.registerElement("GROUPID_TRIGGER", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);

	defBool = false;
	theElm.registerElement("SUPERSEDE_ENABLE", JVX_WW_KEY_VALUE_TYPE_BOOL, &defInt);
}

void
CjvxQtSaWidgetWrapper::initKeymappers()
{
	std::string defStr;
	jvxInt32 defInt = 0;
	jvxData defFloat = 0;
	jvxBool defBool = false;

	// Pushbuttons
	jvx_init_keymapper_min(keymaps.pb);
	defStr = "background-color: rgb(178, 178, 178);";
	keymaps.pb.registerElement("SSON", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defStr = "background-color: rgb(255, 255, 255);";
	keymaps.pb.registerElement("SSOFF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defStr = "";
	keymaps.pb.registerElement("PICON", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.pb.registerElement("PICOFF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.pb.registerElement("PICPREFIX", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.pb.registerElement("TXTON", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.pb.registerElement("TXTOFF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);

	defInt = JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY;
	keymaps.pb.registerElement("BEHAVIOR", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSHBUTTON_DERIVE_TYPE", JVX_WW_PUSHBUTTON_DERIVE_TYPE);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSHBUTTON_CBOOL", JVX_WW_PUSHBUTTON_CBOOL);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSHBUTTON_SELLIST", JVX_WW_PUSHBUTTON_SELLIST);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY", JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD", JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD);
	keymaps.pb.registerTranslator("BEHAVIOR", "JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE", JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE);
	defInt = -1;
	keymaps.pb.registerElement("OPTIONACTIVE", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt); 
	
	defStr = "";
	keymaps.pb.registerElement("FILE_ENDING", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defStr = "./";
	keymaps.pb.registerElement("DEFAULT_FOLDER", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);

	// LineEdit
	jvx_init_keymapper_min(keymaps.le);

	defInt = 4;
	keymaps.le.registerElement("NUMDIGITS", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	defStr = "";
	keymaps.le.registerElement("LABELUNIT", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defFloat = 1;
	keymaps.le.registerElement("SCALEFAC", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defInt = JVX_WW_SHOW_DIRECT;
	keymaps.le.registerElement("LABELTRANSFORM", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.le.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DIRECT", JVX_WW_SHOW_DIRECT);
	keymaps.le.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB10", JVX_WW_SHOW_DB10);
	keymaps.le.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB20", JVX_WW_SHOW_DB20);
	keymaps.le.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_SCALED", JVX_WW_SHOW_SCALED);
	// defFloat = 0;
	defFloat = JVX_DATA_MAX_NEG;
	keymaps.le.registerElement("MINVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	// defFloat = 1;
	defFloat = JVX_DATA_MAX_POS;
	keymaps.le.registerElement("MAXVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defFloat = 0;
	keymaps.le.registerElement("GRANULARITY", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defStr = "";
	keymaps.le.registerElement("BUTTONREF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defBool = false;
	keymaps.le.registerElement("EDITRETURNONLY", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	// ListWidget
	jvx_init_keymapper_min(keymaps.lw);

	defInt = 10;
	keymaps.lw.registerElement("NUM_LINES_SHOW", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	defStr = "";
	keymaps.lw.registerElement("BUTTONREF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.lw.registerElement("CHECKBOXREF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defInt = JVX_WW_LINE_SEPARATE_LINEFEED;
	keymaps.lw.registerElement("LINESEPTP", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.lw.registerTranslator("LINESEPTP", "JVX_WW_LINE_SEPARATE_LINEFEED", JVX_WW_LINE_SEPARATE_LINEFEED);
	keymaps.lw.registerTranslator("LINESEPTP", "JVX_WW_LINE_SEPARATE_CARRRETURN", JVX_WW_LINE_SEPARATE_CARRRETURN);
	keymaps.lw.registerTranslator("LINESEPTP", "JVX_WW_LINE_SEPARATE_CARRRETURN_LINEFEED", JVX_WW_LINE_SEPARATE_CARRRETURN_LINEFEED);


	// Label
	jvx_init_keymapper_min(keymaps.la);
	defInt = 4;
	keymaps.la.registerElement("NUMDIGITS", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	keymaps.la.registerElement("LABELUNIT", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defFloat = 1;
	keymaps.la.registerElement("SCALEFAC", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defInt = JVX_WW_SHOW_DIRECT;
	keymaps.la.registerElement("LABELTRANSFORM", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.la.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DIRECT", JVX_WW_SHOW_DIRECT);
	keymaps.la.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB10", JVX_WW_SHOW_DB10);
	keymaps.la.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB20", JVX_WW_SHOW_DB20);
	keymaps.la.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_SCALED", JVX_WW_SHOW_SCALED);
	defFloat = 0;
	keymaps.la.registerElement("MINVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defFloat = 1;
	keymaps.la.registerElement("MAXVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	keymaps.la.registerElement("BEHAVIOR", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.la.registerTranslator("BEHAVIOR", "JVX_WW_LABEL_TEXT", JVX_WW_LABEL_TEXT);
	keymaps.la.registerTranslator("BEHAVIOR", "JVX_WW_LABEL_HIDE_ON_TRUE", JVX_WW_LABEL_HIDE_ON_TRUE);
	keymaps.la.registerTranslator("BEHAVIOR", "JVX_WW_LABEL_GREEN_GRAY", JVX_WW_LABEL_GREEN_GRAY);
	keymaps.la.registerTranslator("BEHAVIOR", "JVX_WW_LABEL_ALERT", JVX_WW_LABEL_ALERT);
	keymaps.la.registerTranslator("BEHAVIOR", "JVX_WW_LABEL_CUSTOM_COLOR", JVX_WW_LABEL_CUSTOM_COLOR);
	defStr = "";
	keymaps.la.registerElement("COLORS", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);


	// Slider and progressbar
	jvx_init_keymapper_min(keymaps.slpba);
	defStr = "";
	defFloat = 1;
	keymaps.slpba.registerElement("DELTASTEPS", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defInt = 2;
	keymaps.slpba.registerElement("NUMDIGITS", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	defInt = 0;
	defFloat = 0;
	keymaps.slpba.registerElement("MINVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defFloat = 1;
	keymaps.slpba.registerElement("MAXVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defStr = "";
	keymaps.slpba.registerElement("LABELREF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	keymaps.slpba.registerElement("LABELUNIT", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defStr = "On";
	keymaps.slpba.registerElement("LABELON", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defStr = "Off";
	keymaps.slpba.registerElement("LABELOFF", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defFloat = 1;
	keymaps.slpba.registerElement("SCALEFAC", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defInt = JVX_WW_SHOW_DIRECT;
	keymaps.slpba.registerElement("LABELTRANSFORM", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.slpba.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DIRECT", JVX_WW_SHOW_DIRECT);
	keymaps.slpba.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB10", JVX_WW_SHOW_DB10);
	keymaps.slpba.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB20", JVX_WW_SHOW_DB20);
	keymaps.slpba.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_SCALED", JVX_WW_SHOW_SCALED);
	
	defBool = false;
	keymaps.slpba.registerElement("ACTIVEMOVE", JVX_WW_KEY_VALUE_TYPE_BOOL, &defBool);

	// ComboBox
	jvx_init_keymapper_min(keymaps.cb);
	defStr = "";
	defInt = JVX_WW_COMBOBOX_ONLY_SELECTION;
	keymaps.cb.registerElement("BEHAVIOR", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);	
	keymaps.cb.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_ONLY_SELECTION", JVX_WW_COMBOBOX_ONLY_SELECTION);
	keymaps.cb.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_READ_ONCE", JVX_WW_COMBOBOX_READ_ONCE);
	keymaps.cb.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_READ_ALWAYS", JVX_WW_COMBOBOX_READ_ALWAYS);

	// ComboBox
	jvx_init_keymapper_min(keymaps.fr);
	
	defInt = JVX_WW_COMBOBOX_ONLY_SELECTION;
	keymaps.fr.registerElement("BEHAVIOR", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.fr.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_ONLY_SELECTION", JVX_WW_COMBOBOX_ONLY_SELECTION);
	keymaps.fr.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_READ_ONCE", JVX_WW_COMBOBOX_READ_ONCE);
	keymaps.fr.registerTranslator("BEHAVIOR", "JVX_WW_COMBOBOX_READ_ALWAYS", JVX_WW_COMBOBOX_READ_ALWAYS);
	defInt = 0;
	keymaps.fr.registerElement("WRAPAROUNDCNT", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	defStr = "";
	defInt = JVX_WW_FRAME_ELEMENTS_LABELS;
	keymaps.fr.registerElement("UIELEMENTTP", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.fr.registerTranslator("UIELEMENTTP", "JVX_WW_FRAME_ELEMENTS_LABELS", JVX_WW_FRAME_ELEMENTS_LABELS);
	keymaps.fr.registerTranslator("UIELEMENTTP", "JVX_WW_FRAME_ELEMENTS_BUTTONS", JVX_WW_FRAME_ELEMENTS_BUTTONS);
	keymaps.fr.registerTranslator("UIELEMENTTP", "JVX_WW_FRAME_ELEMENTS_CHECKBOXES", JVX_WW_FRAME_ELEMENTS_CHECKBOXES);
	defStr = "";
	keymaps.fr.registerElement("VIEWSELECTIONBITMASK", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);
	defInt = 15;
	keymaps.fr.registerElement("MINSIZEW", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	keymaps.fr.registerElement("MINSIZEH", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	keymaps.fr.registerElement("MAXSIZEW", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	keymaps.fr.registerElement("MAXSIZEH", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);

	// All others
	jvx_init_keymapper_min(keymaps.ao);
	defInt = 2; 
	keymaps.ao.registerElement("NUMDIGITS", JVX_WW_KEY_VALUE_TYPE_INT32, &defInt);
	defInt = JVX_WW_SHOW_DIRECT;
	keymaps.ao.registerElement("LABELTRANSFORM", JVX_WW_KEY_VALUE_TYPE_ENUM, &defInt);
	keymaps.ao.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DIRECT", JVX_WW_SHOW_DIRECT);
	keymaps.ao.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB10", JVX_WW_SHOW_DB10);
	keymaps.ao.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_DB20", JVX_WW_SHOW_DB20);
	keymaps.ao.registerTranslator("LABELTRANSFORM", "JVX_WW_SHOW_SCALED", JVX_WW_SHOW_SCALED);
	defFloat = 0;
	keymaps.ao.registerElement("MINVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	defFloat = 1;
	keymaps.ao.registerElement("MAXVAL", JVX_WW_KEY_VALUE_TYPE_DATA, &defFloat);
	keymaps.ao.registerElement("LABELUNIT", JVX_WW_KEY_VALUE_TYPE_STRING, &defStr);

}

/*
void browseWidgets(QWidget* theWidget);

void browseLayout(QLayout* theLayout)
{
	jvxSize i;
	jvxSize num;
	QString myClassName;
	QString myObjectName;
	std::string myClassNameStr;
	std::string myObjectNameStr;

	myClassName = theLayout->metaObject()->className();
	myClassNameStr = myClassName.toLatin1().constData();
	myObjectName = theLayout->objectName();
	myObjectNameStr = myObjectName.toLatin1().constData();


	if(myClassNameStr == "QHBoxLayout")
	{
		std::cout << "Layout: " << myClassNameStr << " " << myObjectNameStr << std::endl; 
		QHBoxLayout* me = qobject_cast<QHBoxLayout*>(theLayout);
		assert(me);
		QLayout* theSubLayout = me->layout();
		/ *
		if(theSubLayout)
		{
			browseLayout(theSubLayout);
		}
		 * /
		num = me->count();
		for(i = 0; i < num; i++)
		{
			QLayoutItem* it = me->itemAt(i);
			if(it)
			{
				QLayout* ll = it->layout();
				if(ll)
				{
					browseLayout(ll);
				}
				QWidget*ww = it->widget();
				if(ww)
				{
					browseWidgets(ww);
				}
			}
		}
	}
	else if(myClassNameStr == "QVBoxLayout")
	{
		std::cout << "Layout: " << myClassNameStr << " " << myObjectNameStr << std::endl; 
		QVBoxLayout* me = qobject_cast<QVBoxLayout*>(theLayout);
		assert(me);
		QLayout* theSubLayout = me->layout();
		/ *
		if(theSubLayout)
		{
			browseLayout(theSubLayout);
		}
		* /
		num = me->count();
		for(i = 0; i < num; i++)
		{
			QLayoutItem* it = me->itemAt(i);
			if(it)
			{
				QLayout* ll = it->layout();
				if(ll)
				{
					browseLayout(ll);
				}
				QWidget*ww = it->widget();
				if(ww)
				{
					browseWidgets(ww);
				}
			}
		}
	}
	else if(myClassNameStr == "QGridLayout")
	{
		std::cout << "Layout: " << myClassNameStr << " " << myObjectNameStr << std::endl; 
		QGridLayout* me = qobject_cast<QGridLayout*>(theLayout);
		assert(me);
		QLayout* theSubLayout = me->layout();
		/ *
		if(theSubLayout)
		{
			browseLayout(theSubLayout);
		}
		* /
		num = me->count();
		for(i = 0; i < num; i++)
		{
			QLayoutItem* it = me->itemAt(i);
			if(it)
			{
				QLayout* ll = it->layout();
				if(ll)
				{
					browseLayout(ll);
				}
				QWidget*ww = it->widget();
				if(ww)
				{
					browseWidgets(ww);
				}
			}
		}	
	}
	else
	{
		std::cout << "--> Unhandled Layouttype: " << myClassNameStr << " " << myObjectNameStr << std::endl; 
	}
}
*/
void 
CjvxQtSaWidgetWrapper::browseWidgets(oneAssociationList& addto, QWidget* theWidget, 
	const std::string& selector_lst, const std::string& tag, IjvxAccessProperties* propRefIn, QWidget* crossReferences,
	jvxBool verboseLoc, const std::string& prefix, IjvxHiddenInterface* hostRef)
{
	QString myClassName;
	QString myObjectName;
	std::string myClassNameStr;
	std::string myObjectNameStr;
	QString propTargetName;
	std::string propTargetNameStr;
	QString propPrefixName;
	std::string propPrefixNameStr;
	std::string idtag = "";
	std::string propname;
	std::vector<std::string> paramlst;

	this->config.matchIdType = JVX_WW_MATCH_GROUPID_BITFIELD;
	config.distributeSet = true;

	myClassName = theWidget->metaObject()->className();
	myClassNameStr = myClassName.toLatin1().constData();

	myObjectName = theWidget->objectName();
	myObjectNameStr = myObjectName.toLatin1().constData();

	propTargetName = theWidget->accessibleDescription();
	propTargetNameStr = propTargetName.toLatin1().constData(); 

	propPrefixName = theWidget->accessibleName();
	propPrefixNameStr = jvx_makePathExpr(prefix, propPrefixName.toLatin1().constData(), true);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
	if (this->verbose_output)
	{
		std::cout << "	## " << myClassNameStr << "-->" << myObjectNameStr << " --> " << propTargetNameStr << "(" << propPrefixNameStr << ")." << std::endl;
	}
#endif

	if (!propTargetNameStr.empty())
	{
		jvxSize uId = g_uniqueId++;
		jvx_parsePropertyLinkDescriptor(propTargetNameStr, idtag, propname, paramlst);
		if (idtag == "jvx_properties")
		{
			if (myClassNameStr == "QTreeWidget")
			{
				oneAssociation theAssoc;
				theAssoc.theWidget = theWidget;

				QTreeWidget* theTree = qobject_cast<QTreeWidget*>(theWidget);

				if (CjvxMaWrapperElementTreeWidget::hasValidTags(theTree, tag))
				{
					CjvxMaWrapperElementTreeWidget* treeWidgetWrap = new CjvxMaWrapperElementTreeWidget(theTree, this, 
						propRefIn, propname, paramlst, tag, selector_lst, myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (treeWidgetWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(treeWidgetWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						addto.theAssociations.push_back(theAssoc);

						if (treeWidgetWrap->verboseOut())
						{
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						std::cout << "Warning: Widget NOT connected since property is not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						delete treeWidgetWrap;
					}
				}
			}
		}
		else
		{
			paramlst.clear();
			//propTargetNameStr = "audio_node:/a/b:G, 9, KJA";
			//propTargetNameStr = "audio_node:/a/b:";
			jvx_parsePropertyLinkDescriptor(propTargetNameStr, idtag, propname, paramlst);
			
			// Add a leading "/" if missing
			propname = jvx_makePathExpr(propname, "", true);

			jvxBool reasonToTryConnect = false;
			jvxBool isWildConnect = false;

			if (tag.empty())
			{
				reasonToTryConnect = true;
			}
			
			if (jvx_compareStringsWildcard(idtag, tag))
			{
				if (jvx_isStringsWildcard(idtag))
				{
					// replace wildcard
					idtag = tag;
					isWildConnect = true;
				}
				reasonToTryConnect = true;
			}

			if(reasonToTryConnect)
			{
				oneAssociation theAssoc;
				theAssoc.theWidget = theWidget;
				//theAssoc.paramlist = paramlst;


				if (
					(myClassNameStr == "QCheckBox") ||
					(myClassNameStr == "QRadioButton"))
				{
					CjvxSaWrapperElementAbstractButton* checkBoxWrap = new CjvxSaWrapperElementAbstractButton(
						qobject_cast<QAbstractButton*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (checkBoxWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(checkBoxWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (checkBoxWrap->verboseOut())
						{
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << "; dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}

						addto.ass_property_ids.push_back(uId);
						
#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete checkBoxWrap;
					}
				}
				else if (myClassNameStr == "QComboBox")
				{
					CjvxSaWrapperElementComboBox* comboBoxWrap = new CjvxSaWrapperElementComboBox(
						qobject_cast<QComboBox*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (comboBoxWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(comboBoxWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (comboBoxWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif
					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete comboBoxWrap;
					}
				}
				else if (myClassNameStr == "QSlider")
				{
					CjvxSaWrapperElementSlider* sliderWrap = new CjvxSaWrapperElementSlider(
						qobject_cast<QSlider*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, crossReferences, verboseLoc, uId, hostRef);
					if (sliderWrap->get_widget_status()  > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(sliderWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (sliderWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;

						addto.ass_property_ids.push_back(uId);
						
#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete sliderWrap;
					}
				}
				else if (myClassNameStr == "QProgressBar")
				{
					CjvxSaWrapperElementProgressBar* pbWrap = new CjvxSaWrapperElementProgressBar(
						qobject_cast<QProgressBar*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, crossReferences, verboseLoc, uId, hostRef);
					if (pbWrap->get_widget_status()  > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(pbWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (pbWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete pbWrap;
					}
				}
				else if (myClassNameStr == "QLineEdit")
				{
					CjvxSaWrapperElementLineEdit* lineEditWrap = new CjvxSaWrapperElementLineEdit(
						qobject_cast<QLineEdit*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, crossReferences, verboseLoc, uId, hostRef);
					if (lineEditWrap->get_widget_status()  > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(lineEditWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (lineEditWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete lineEditWrap;
					}
				}
				else if (myClassNameStr == "QListWidget")
				{
					CjvxSaWrapperElementListWidget* listWidgetWrap = new CjvxSaWrapperElementListWidget(
						qobject_cast<QListWidget*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, crossReferences, verboseLoc, uId, hostRef);
					if (listWidgetWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(listWidgetWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (listWidgetWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete listWidgetWrap;
					}
				}
				else if (myClassNameStr == "QLabel")
				{
					CjvxSaWrapperElementLabel* labelWrap = new CjvxSaWrapperElementLabel(
						qobject_cast<QLabel*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (labelWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(labelWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (labelWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif
					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete labelWrap;
					}
				}
				else if (myClassNameStr == "QPushButton")
				{
					CjvxSaWrapperElementPushButton* buttonWrap = new CjvxSaWrapperElementPushButton(
						qobject_cast<QPushButton*>(theWidget), this, propRefIn, propname, paramlst, 
						idtag, selector_lst, myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (buttonWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(buttonWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (buttonWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete buttonWrap;
					}
				}
				else if ((myClassNameStr == "QFrame") ||
						(myClassNameStr == "QGroupBox"))
				{
					CjvxSaWrapperElementFrame* labelWrap = new CjvxSaWrapperElementFrame(theWidget, 
						this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (labelWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(labelWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (labelWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete labelWrap;
					}
				}

				// Next, the custom widgets
				else if (myClassNameStr == "jvxQtSaLogFileWriter")
				{
					CjvxSaWrapperElementJvxLogFileWriter* jvxLogFileWriterWrap = new CjvxSaWrapperElementJvxLogFileWriter(
						qobject_cast<jvxQtSaLogFileWriter*>(theWidget), this, propRefIn, propname, paramlst, 
						idtag, selector_lst, myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (jvxLogFileWriterWrap->get_widget_status() > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(jvxLogFileWriterWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (jvxLogFileWriterWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						
						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete jvxLogFileWriterWrap;
					}
				}
				else if (myClassNameStr == "jvxQtSaNetworkMasterDevice")
				{
					CjvxSaWrapperElementJvxNetworkMasterDevice* jvxLogFileNetworkMDevWrap = new CjvxSaWrapperElementJvxNetworkMasterDevice(
						qobject_cast<jvxQtSaNetworkMasterDevice*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
						myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
					if (jvxLogFileNetworkMDevWrap->get_widget_status()  > JVX_STATE_NONE)
					{
						theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(jvxLogFileNetworkMDevWrap);
						theAssoc.accessDescription_orig = propTargetNameStr;
						theWidget->setAccessibleDescription("");

						addto.theAssociations.push_back(theAssoc);
						if (jvxLogFileNetworkMDevWrap->verboseOut())
							std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;

						addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
						if (this->verbose_output)
						{
							std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
						}
#endif

					}
					else
					{
						if (!isWildConnect)
						{
							std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
						}
						delete jvxLogFileNetworkMDevWrap;
					}
				}
				else if (myClassNameStr == "jvxQtSaNetworkSlaveDevice")
				{
				CjvxSaWrapperElementJvxNetworkSlaveDevice* jvxLogFileNetworkSDevWrap = new CjvxSaWrapperElementJvxNetworkSlaveDevice(
					qobject_cast<jvxQtSaNetworkSlaveDevice*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
					myObjectNameStr, propPrefixNameStr, verboseLoc, uId, hostRef);
				if (jvxLogFileNetworkSDevWrap->get_widget_status() > JVX_STATE_NONE)
				{
					theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(jvxLogFileNetworkSDevWrap);
					theAssoc.accessDescription_orig = propTargetNameStr;
					theWidget->setAccessibleDescription("");

					addto.theAssociations.push_back(theAssoc);
					if (jvxLogFileNetworkSDevWrap->verboseOut())
						std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
					
					addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
					if (this->verbose_output)
					{
						std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
					}
#endif

				}
				else
				{
					if (!isWildConnect)
					{
						std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
					}
					delete jvxLogFileNetworkSDevWrap;
				}
				}
				else if (myClassNameStr == "jvxQtSaNetworkTechnology")
				{
				CjvxSaWrapperElementJvxNetworkTechnology* jvxLogFileNetworkTechWrap = new CjvxSaWrapperElementJvxNetworkTechnology(
					qobject_cast<jvxQtSaNetworkTechnology*>(theWidget), this, propRefIn, propname, paramlst, idtag, selector_lst,
					myObjectNameStr, propPrefixNameStr, verboseLoc, uId);
				if (jvxLogFileNetworkTechWrap->get_widget_status() > JVX_STATE_NONE)
				{
					theAssoc.theUiWrap = static_cast<CjvxSaWrapperElementBase*>(jvxLogFileNetworkTechWrap);
					theAssoc.accessDescription_orig = propTargetNameStr;
					theWidget->setAccessibleDescription("");
					addto.theAssociations.push_back(theAssoc);

					if (jvxLogFileNetworkTechWrap->verboseOut())
						std::cout << "Widget connected: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << 
							idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
					
					addto.ass_property_ids.push_back(uId);

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
					if (this->verbose_output)
					{
						std::cout << "Adding uId " << uId << " for selector <" << selector_lst << ">." << std::endl;
					}
#endif

				}
				else
				{
					if (!isWildConnect)
					{
						std::cout << "Widget NOT connected since not ready: " << myClassNameStr << " " << myObjectNameStr << "--> tag = " << idtag << ";  dyn prefix = " << propPrefixNameStr << "; property = " << propTargetNameStr << std::endl;
					}
					delete jvxLogFileNetworkTechWrap;
				}
				}
				else
				{
					std::cout << "Assigned class " << myClassNameStr << " to property description " << propTargetNameStr << ":  Sorry, do not have any rule for this class type!" << std::endl;
				}
			}
		}
	}
	else
	{
		// 
	}
}

CjvxQtSaWidgetWrapper::CjvxQtSaWidgetWrapper()
{
	this->g_uniqueId = 1;
	this->verbose_output = false;
	initKeymappers();
	connect(this, SIGNAL(emit_updateWindow(jvxHandle*)), this, SLOT(slot_updateWindow(jvxHandle*)), Qt::QueuedConnection); 
	connect(this, SIGNAL(emit_updateWindow_periodic(jvxHandle*)), this, SLOT(slot_updateWindow_periodic(jvxHandle*)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_updateStatus(jvxHandle*)), this, SLOT(slot_updateStatus(jvxHandle*)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_updateProperty(jvxHandle*)), this, SLOT(slot_updateProperty(jvxHandle*)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_reportPropertyInitComplete(QString, jvxSize)), this, SLOT(slot_reportPropertyInitComplete(QString, jvxSize)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_report_all_associated_no_content(QString)), this, SLOT(slot_report_all_associated_no_content(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(emit_finalizeTransfer(jvxSize, jvxHandle*)), this, SLOT(slot_finalizeTransfer(jvxSize, jvxHandle*)), Qt::QueuedConnection);
}

CjvxQtSaWidgetWrapper::~CjvxQtSaWidgetWrapper()
{
}

jvxErrorType 
CjvxQtSaWidgetWrapper::associateAutoWidgets(QWidget* theWidget, IjvxAccessProperties* propRefIn,
	IjvxQtSaWidgetWrapper_report* report, std::string selector_lst, jvxBool verboseLoc, jvxWwSearchMode wwSearchMode,
	std::string prefix, std::string tag_key, IjvxHiddenInterface* hostRef)
{
	jvxSize i;
	QString myClassName;
	QString myObjectName;
	std::string myClassNameStr;
	std::string myObjectNameStr;
	QWidget* startHere = NULL;
	QWidget* parRef = NULL;

	if (!propRefIn)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);
	if (elm == theLists.end())
	{
		oneAssociationList theNewElement;
		theNewElement.selector = selector_lst;
		theNewElement.propRef = propRefIn;
		theNewElement.propRef->get_features(&theNewElement.thePropAFeatures);
		theNewElement.report = report;
		theNewElement.uniqueId = 1;
		theNewElement.procId = 0;
		theNewElement.procIdSP = 0;
		theNewElement.procIdP = 0;

		startHere = theWidget;
		switch (wwSearchMode)
		{
		case JVX_WW_WIDGET_SEARCH_FROM_ROOT:
			parRef = startHere->parentWidget();

			while (parRef)
			{
				startHere = parRef;
				parRef = startHere->parentWidget();
			}
			break;
		default:
			break;
		}

		myClassName = startHere->metaObject()->className();
		myClassNameStr = myClassName.toLatin1().constData();

		myObjectName = startHere->objectName();
		myObjectNameStr = myObjectName.toLatin1().constData();

		//std::cout << myClassNameStr << "-###->" << myObjectNameStr << std::endl;
		if (tag_key.empty())
		{
			tag_key = selector_lst;
		}
		theNewElement.tag_key = tag_key;

		QList<QWidget *> widgets;
		switch (wwSearchMode)
		{
		case JVX_WW_WIDGET_SEARCH_FROM_ROOT:
		case JVX_WW_WIDGET_SEARCH_ALL_CHILDREN:
			widgets = startHere->findChildren<QWidget *>();
			break;
		case JVX_WW_WIDGET_SEARCH_CURRENT_AND_CHILDREN:
			widgets = startHere->findChildren<QWidget *>();
			widgets.push_front(theWidget);
			break;
		case JVX_WW_WIDGET_SEARCH_CURRENT_ONLY:
			widgets.push_front(theWidget);
			break;
		default:
			break;
		}

		for (i = 0; i < widgets.count(); i++)
		{
			browseWidgets(theNewElement, widgets[(int)i], selector_lst, tag_key, propRefIn, theWidget, verboseLoc, prefix, hostRef);
		}
		theLists.push_back(theNewElement);
		if (theNewElement.theAssociations.size() == 0)
		{
			// If there were no direct associations, trigger the complete callback anyway!
			emit emit_report_all_associated_no_content(selector_lst.c_str());
		}
		return JVX_NO_ERROR;
	}
	
	return JVX_ERROR_DUPLICATE_ENTRY;
}

void
CjvxQtSaWidgetWrapper::slot_report_all_associated_no_content(QString selector_lst)
{
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst.toLatin1().data());
	if (elm != theLists.end())
	{
		if (elm->report)
		{
			elm->report->reportAllPropertiesAssociateComplete(selector_lst.toLatin1().data());
		}
	}
}

jvxErrorType 
CjvxQtSaWidgetWrapper::deassociateAutoWidgets(std::string selector_lst)
{
	jvxSize i;
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);
	if(elm != theLists.end())
	{
		for(i = 0; i < elm->theAssociations.size(); i++)
		{
			// Reconstruct original widget description text
			elm->theAssociations[i].theWidget->setAccessibleDescription(elm->theAssociations[i].accessDescription_orig.c_str());

			elm->theAssociations[i].theUiWrap->clearUiElements();
			delete(elm->theAssociations[i].theUiWrap);
		}
		elm->theAssociations.clear();
		if (elm->ass_property_ids.size())
		{
			auto elmId = elm->ass_property_ids.begin();
			for (; elmId != elm->ass_property_ids.end(); elmId++)
			{
				ass_property_ids_collect_on_erase.push_back(*elmId);
			}
		}
		theLists.erase(elm);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxQtSaWidgetWrapper::reportPropertySet(const char* selector_lst, const char* descror, jvxSize groupid,
	jvxWwReportType howtoreport, CjvxSaWrapperElementBase* thisisme, jvxErrorType res_in_call)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::vector<oneAssociationList>::iterator elm;
	switch(howtoreport)
	{
	case JVX_WW_REPORT_ELEMENT:
	case JVX_WW_REPORT_LOCAL:
	case JVX_WW_REPORT_LOCAL_GLOBAL:

		// Always run the readback
		thisisme->updateWindow_core();
	}
	
	switch (howtoreport)
	{
	case JVX_WW_REPORT_GLOBAL:
	case JVX_WW_REPORT_LOCAL_GLOBAL:
		elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
			theLists, selector_lst);
		if (elm != theLists.end())
		{
			if (elm->report)
			{
				res = elm->report->reportPropertySet(selector_lst, descror, groupid, res_in_call);
				// After a set we always need a "get"
				if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
				{
					thisisme->updateWindow_core();
				}
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}

	if (1)
	{
		elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
			theLists, selector_lst);
		if (elm != theLists.end())
		{
			auto elmC = theLists.begin();
			for (; elmC != theLists.end(); elmC++)
			{
				if (elm != elmC)
				{
					if (elm->propRef == elmC->propRef)
					{
						jvxPropertyCallContext purpose = JVX_WIDGET_RWAPPER_UPDATE_NORMAL;
						this->trigger_updateWindow_core(elmC->selector, elmC, NULL, purpose, descror, groupid); // <- check that specific widget AND group id
					}
				}
			}
		}
	}

	// If we want this, we need to leave this callback, therefore, only the "request_command" works
	//res = elm->report->reportPropertySet(tag, descror, JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL);
	return res;
}

jvxErrorType
CjvxQtSaWidgetWrapper::reportPropertyGet(const char* selector_lst, const char* descror,
	jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, CjvxSaWrapperElementBase* thisisme, jvxErrorType res_in_call)
{
	jvxErrorType res = JVX_NO_ERROR;

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);
	if (elm != theLists.end())
	{
		if (elm->report)
		{
			res = elm->report->reportPropertyGet(selector_lst, descror, ptrFld, offset, numElements, format, res_in_call);
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	return res;
}

jvxErrorType
CjvxQtSaWidgetWrapper::reportPropertyInitComplete(const char* selector_lst, jvxSize uId)
{
	QString qtag = selector_lst;

#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
	if (this->verbose_output)
	{
		std::cout << "Report init complete for id " << uId << ", <" << selector_lst << ">." << std::endl;
	}
#endif

	emit emit_reportPropertyInitComplete(qtag, uId);
	return JVX_NO_ERROR;
}

void 
CjvxQtSaWidgetWrapper::slot_reportPropertyInitComplete(QString selector_lst, jvxSize uId)
{
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst.toLatin1().data());
	if (elm != theLists.end())
	{
		std::list<jvxSize>::iterator elmp = std::find(elm->ass_property_ids.begin(), elm->ass_property_ids.end(), uId);
		if (elmp == elm->ass_property_ids.end())
		{
			auto elm_cleared = std::find(
				ass_property_ids_collect_on_erase.begin(),
				ass_property_ids_collect_on_erase.end(), uId);
			if (elm_cleared == ass_property_ids_collect_on_erase.end())
			{
				std::cout << "+++ Error: invalid id " << uId << " received for tag " << selector_lst.toLatin1().data() << std::endl;
			}
			else
			{
#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
				if (this->verbose_output)
				{
					std::cout << "Erasing id " << uId << " received for tag " <<
						selector_lst.toLatin1().data() << " from list of erased ids." << std::endl;
				}
#endif
			}
		}
		else
		{
#ifdef JVX_DEBUG_WIDGETWRAPPER_VERBOSE
			if (this->verbose_output)
			{
				std::cout << "Erasing id " << uId << " received for tag " << selector_lst.toLatin1().data() << std::endl;
			}
#endif
			elm->ass_property_ids.erase(elmp);
		}
		if (elm->ass_property_ids.size() == 0)
		{
			if (elm->report)
			{
				elm->report->reportAllPropertiesAssociateComplete(selector_lst.toLatin1().data());
			}
		}
	}
}

jvxErrorType 
CjvxQtSaWidgetWrapper::trigger_updateProperty(const std::string& selector_lst, 
	jvxPropertyCategoryType cat, jvxSize propId, 
	jvxBool onlyContent,
	jvxPropertyCallContext purpose, 
	jvxSize* uIdExt)
{
	jvxSize i;
	jvxBool atLeastOneUpdated = false;
	jvxBool localUpdate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyCallPrivate* myPriv = NULL;
	jvxSize* theUidPtr = NULL;
	jvxCallManagerProperties callGate;
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);

	if (elm != theLists.end())
	{
		theUidPtr = &elm->procIdSP;
		if (uIdExt)
		{
			theUidPtr = uIdExt;
		}
		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{

			//std::cout << "Property ID = " << propId << std::endl;
			myPriv = new jvxPropertyCallPrivate;
			CjvxSaWrapperElementBase::init_propertyPrivate(myPriv, JVX_WIDGET_RWAPPER_UPDATE_PROPERTY, selector_lst, propId, cat, onlyContent);

			if (uIdExt)
			{
				myPriv->passExternalUId = true;
				myPriv->externalUId = uIdExt;
			}

			res = elm->propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), myPriv, jPSCH(theUidPtr));
			if (res == JVX_ERROR_COMPONENT_BUSY)
			{
				std::cout << __FUNCTION__ << ", widget wrapper: warning: operation has been triggered before and is still waiting for results." << std::endl;
				return JVX_ERROR_COMPONENT_BUSY;
			}
			else if (res != JVX_NO_ERROR)
			{
				assert(0);
			}
		}
		
		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			if (elm->theAssociations[i].theUiWrap->matchesIdCat(propId, cat))
			{
				localUpdate = elm->theAssociations[i].theUiWrap->updateWindowUiElement_ifavail(purpose, theUidPtr);
				atLeastOneUpdated = (atLeastOneUpdated || localUpdate);
			}
		}
		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{
			jvxErrorType resL = elm->propRef->property_stop_delayed_group(callGate, reinterpret_cast<jvxHandle**>(&myPriv), jPSCH(theUidPtr));
			if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
			{
				if (theUidPtr)
				{
					*theUidPtr = 0;
				}
				if (myPriv)
				{
					std::cout << "Remove since no property added" << std::endl;
					delete myPriv;
				}
				return JVX_ERROR_ABORT;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxQtSaWidgetWrapper::trigger_updateWindow_core(const std::string& selector_lst,
	std::vector<oneAssociationList>::iterator& elm, jvxSize* uIdExt, jvxPropertyCallContext purpose,
	const std::string& propertyWidcard, jvxSize groupid)
{
	jvxSize i;

	jvxSize* theUidPtr = NULL;
	theUidPtr = &elm->procId;
	jvxPropertyCallPrivate* myPriv = NULL;
	jvxBool localUpdate;
	jvxBool atLeastOneUpdated = false;
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;

	if (uIdExt)
	{
		theUidPtr = uIdExt;
	}

	// If system operates with delayed response and call is "NORMAL", we need to start the group
	JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
	{
		// std::cout << __FUNCTION__ << ": New request for <" << elm->selector << ">." << std::endl;
		myPriv = new jvxPropertyCallPrivate;
		CjvxSaWrapperElementBase::init_propertyPrivate(myPriv, JVX_WIDGET_RWAPPER_UPDATE_WINDOW, selector_lst, propertyWidcard, groupid);

		if (uIdExt)
		{
			myPriv->passExternalUId = true;
			myPriv->externalUId = uIdExt;
		}

		res = elm->propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), myPriv, jPSCH(theUidPtr));
		if (res == JVX_ERROR_COMPONENT_BUSY)
		{
			std::cout << __FUNCTION__ << ", widget wrapper: warning: operation has been triggered before and is still waiting for results." << std::endl;
			return JVX_ERROR_COMPONENT_BUSY;
		}
		else if (res != JVX_NO_ERROR)
		{
			assert(0);
		}
	}
	else
	{
		if (purpose == JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE)
		{
			// Here, we arrive from within the delayed update function. Typically,
			// the id is still set and we can just procede. If the value is 0
			// something is wrong and we need to cancel the delayed readout
			// 
			// std::cout << __FUNCTION__ << ": Finalizing request for <" << elm->selector << ">." << std::endl;
			if (*theUidPtr == 0)
			{
				// Here, we have ended if the signal is emitted at the wrong time - if the element still is in final stage
				// We just need to ignore this call
				return JVX_ERROR_ABORT;
			}
		}
	}

	for (i = 0; i < elm->theAssociations.size(); i++)
	{
		if (elm->theAssociations[i].theUiWrap->matchesWildcard(propertyWidcard, groupid, this->config.matchIdType))
		{
			localUpdate = elm->theAssociations[i].theUiWrap->updateWindowUiElement_ifavail(purpose, theUidPtr);
			atLeastOneUpdated = (atLeastOneUpdated || localUpdate);

		}
	}
	JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
	{
		jvxErrorType resL = elm->propRef->property_stop_delayed_group(callGate, reinterpret_cast<jvxHandle**>(&myPriv), jPSCH(theUidPtr));
		if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
		{
			if (theUidPtr)
			{
				*theUidPtr = 0;
			}
			if (myPriv)
			{
				delete myPriv;
			}
		}
	}

	// std::cout << __FUNCTION__ << ": Leaving request for <" << elm->selector << ">." << std::endl;

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxQtSaWidgetWrapper::trigger_updateWindow(const std::string& selector_lst, const std::string& propertyWidcard, jvxSize groupid,
	jvxPropertyCallContext purpose,
	jvxSize* uIdExt)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);

	if(elm != theLists.end())
	{
		res = trigger_updateWindow_core(selector_lst,
			elm, uIdExt, purpose,
			propertyWidcard, groupid);
	}
	return res;
}

jvxErrorType
CjvxQtSaWidgetWrapper::trigger_local_command(const std::string& selector_lst, const std::string& propertyWidcard, jvxSize groupid,
	std::string& local_command)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyCallPrivate* myPriv = NULL;
	jvxSize* theUidPtr = NULL;

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);

	if (elm != theLists.end())
	{
		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			if (elm->theAssociations[i].theUiWrap->matchesWildcard(propertyWidcard, groupid, this->config.matchIdType))
			{
				elm->theAssociations[i].theUiWrap->local_command(local_command);
			}
		}
			
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxQtSaWidgetWrapper::trigger_updatePropertyStatus(const std::string& selector_lst, const std::string& propertyWidcard, jvxSize groupid,
	jvxPropertyCallContext purpose,
	jvxSize* uIdExt)
{
	jvxSize i;
	jvxBool atLeastOneUpdated = false;
	jvxBool localUpdate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyCallPrivate* myPriv = NULL;
	jvxSize* theUidPtr = NULL;
	jvxCallManagerProperties callGate;

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);


	if (elm != theLists.end())
	{
		theUidPtr = &elm->procId;
		if (uIdExt)
		{
			theUidPtr = uIdExt;
		}
		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{
			myPriv = new jvxPropertyCallPrivate;
			CjvxSaWrapperElementBase::init_propertyPrivate(myPriv, JVX_WIDGET_RWAPPER_UPDATE_STATUS, selector_lst, propertyWidcard, groupid);

			if (uIdExt)
			{
				myPriv->passExternalUId = true;
				myPriv->externalUId = uIdExt;
			}

			res = elm->propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), myPriv, jPSCH(theUidPtr));
			if (res == JVX_ERROR_COMPONENT_BUSY)
			{
				std::cout << __FUNCTION__ << ", widget wrapper: warning: operation has been triggered before and is still waiting for results." << std::endl;
				return JVX_ERROR_COMPONENT_BUSY;
			}
			else if (res != JVX_NO_ERROR)
			{
				assert(0);
			}
		}

		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			if (elm->theAssociations[i].theUiWrap->matchesWildcard(propertyWidcard, groupid, this->config.matchIdType))
			{
				localUpdate = elm->theAssociations[i].theUiWrap->updatePropertyStatus_ifavail(purpose, theUidPtr);
				atLeastOneUpdated = (atLeastOneUpdated || localUpdate);
					
			}
		}
		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{
			jvxErrorType resL = elm->propRef->property_stop_delayed_group(callGate,
				reinterpret_cast<jvxHandle**>(&myPriv), jPSCH(theUidPtr));
			if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
			{
				if (theUidPtr)
				{
					*theUidPtr = 0;
				}
				if (myPriv)
				{
					delete myPriv;
				}
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxQtSaWidgetWrapper::trigger_updateWindow_periodic(const std::string& selector_lst, const std::string& propertyWidcard, jvxSize groupid,
	jvxPropertyCallContext purpose,
	jvxSize* uIdExt)
{
	jvxSize i;
	jvxBool atLeastOneUpdated = false;
	jvxBool localUpdate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxPropertyCallPrivate* myPriv = NULL;
	jvxSize* theUidPtr = NULL;
	jvxCallManagerProperties callGate;
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(theLists, selector_lst);
	if(elm != theLists.end())
	{		
		theUidPtr = &elm->procIdP;
		if (uIdExt)
		{
			theUidPtr = uIdExt;
		}
		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{
			myPriv = new jvxPropertyCallPrivate;
			CjvxSaWrapperElementBase::init_propertyPrivate(myPriv, JVX_WIDGET_RWAPPER_UPDATE_WINDOW_PERIODIC, selector_lst, propertyWidcard, groupid);

			if (uIdExt)
			{
				myPriv->passExternalUId = true;
				myPriv->externalUId = uIdExt;
			}

			res = elm->propRef->property_start_delayed_group(callGate, static_cast<IjvxAccessProperties_delayed_report*>(this), myPriv, jPSCH(theUidPtr));
			if (res == JVX_ERROR_COMPONENT_BUSY)
			{
				std::cout << __FUNCTION__ << ", widget wrapper: warning: operation has been triggered before and is still waiting for results." << std::endl;
				return JVX_ERROR_COMPONENT_BUSY;
			}
			else if (res != JVX_NO_ERROR)
			{
				assert(0);
			}
		}

		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			if (elm->theAssociations[i].theUiWrap->updateRealtime())
			{
				if (elm->theAssociations[i].theUiWrap->matchesWildcard(propertyWidcard, groupid, this->config.matchIdType))
				{
					localUpdate = elm->theAssociations[i].theUiWrap->trigger_updateWindow_periodic_ifavail(purpose, theUidPtr);
					atLeastOneUpdated = (atLeastOneUpdated || localUpdate);
				}
			}
		}

		JVX_IF_NONBLOCKING_RESPONSE(elm->thePropAFeatures, purpose)
		{
			jvxErrorType resL = elm->propRef->property_stop_delayed_group(callGate,
				reinterpret_cast<jvxHandle**>(&myPriv), jPSCH(theUidPtr));
			if (resL == JVX_ERROR_ELEMENT_NOT_FOUND)
			{
				if (theUidPtr)
				{
					*theUidPtr = 0;
				}
				if (myPriv)
				{
					delete myPriv;
				}
			}
		}

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

// =====================================================================================================

jvxErrorType
CjvxQtSaWidgetWrapper::report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;

	/* We end up here if the HTTP request was issued for multiple widgets. A
	   SET and a single GET are always in single element context. */
	switch (asUpd->delayedCallPurpose)
	{
	case JVX_WIDGET_RWAPPER_UPDATE_WINDOW:
		emit emit_updateWindow(privatePtr);
		break;
	case JVX_WIDGET_RWAPPER_UPDATE_STATUS:
		emit emit_updateStatus(privatePtr);
		break;
	case JVX_WIDGET_RWAPPER_UPDATE_WINDOW_PERIODIC:
		emit emit_updateWindow_periodic(privatePtr);
		break;
	case JVX_WIDGET_RWAPPER_UPDATE_PROPERTY:
		emit emit_updateProperty(privatePtr);
		break;
	default:
		assert(0);
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxQtSaWidgetWrapper::report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle *privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;

	/*
	if (!asUpd->passExternalUId)
	{
		std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(theLists, asUpd->selector_lst);
		if (elm != theLists.end())
		{
			if (uniqueId == elm->procId)
			{
				elm->procId = 0;
			}
			if (uniqueId == elm->procIdP)
			{
				elm->procIdP = 0;
			}
			if (uniqueId == elm->procIdSP)
			{
				elm->procIdSP = 0;
			}
		}
	}
	*/

	// std::cout << "Private Pointer (I):" << privatePtr << std::endl;

	/* We end up here if the HTTP request was issued for multiple widgets. A
	   SET and a single GET are always in single element context. */
	emit emit_finalizeTransfer(uniqueId, privatePtr);
	return JVX_NO_ERROR;
}

void
CjvxQtSaWidgetWrapper::slot_finalizeTransfer(jvxSize uniqueId, jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;

	// std::cout << "Private Pointer (II):" << privatePtr << std::endl;

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(theLists, asUpd->selector_lst);
	if (elm != theLists.end())
	{
		// Reset the process id.
		switch (asUpd->delayedCallPurpose)
		{
		case JVX_WIDGET_RWAPPER_UPDATE_WINDOW:
		case JVX_WIDGET_RWAPPER_UPDATE_STATUS:
			if (asUpd->passExternalUId)
			{
				elm->report->reportPropertySpecific(JVX_HTTPAPI_DELAYED_UPDATE_TERMINATE, asUpd->externalUId);
			}
			else
			{
				assert(uniqueId == elm->procId);
				elm->procId = 0;
			}
			break;
		case JVX_WIDGET_RWAPPER_UPDATE_PROPERTY:
			if (asUpd->passExternalUId)
			{
				elm->report->reportPropertySpecific(JVX_HTTPAPI_DELAYED_UPDATE_TERMINATE, asUpd->externalUId);
			}
			else
			{
				assert(uniqueId == elm->procIdSP);
				elm->procIdSP = 0;
			}
			break;
		case JVX_WIDGET_RWAPPER_UPDATE_WINDOW_PERIODIC:
			if (asUpd->passExternalUId)
			{
				elm->report->reportPropertySpecific(JVX_HTTPAPI_DELAYED_UPDATE_TERMINATE, asUpd->externalUId);
			}
			else
			{
				assert(uniqueId == elm->procIdP);
				elm->procIdP = 0;
			}
			break;
		default:
			assert(0);
		}
	}
	else
	{
		std::cout << __FUNCTION__ << ": Could not associate identifier <" << asUpd->selector_lst << "> in http transfer. Possibly the connection was closed." << std::endl;
		// no worries, seems that connection was stopped
		// assert(0);
	}

	/* Delete the private field */
	jvxPropertyCallPrivate* priv = (jvxPropertyCallPrivate*)privatePtr;
	delete priv;
}

// =================================================================================================================0

void
CjvxQtSaWidgetWrapper::slot_updateWindow(jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;
	if (asUpd)
	{
		if (asUpd->passExternalUId)
		{
			trigger_updateWindow(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE, asUpd->externalUId);
			return;
		}
	}
	trigger_updateWindow(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

void
CjvxQtSaWidgetWrapper::slot_updateProperty(jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;
	if(asUpd)
	{
		if (asUpd->passExternalUId)
		{
			trigger_updateProperty(asUpd->selector_lst, asUpd->prop_cat, asUpd->prop_id, asUpd->prop_conly, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE, asUpd->externalUId);
			return;
		}
	}
	trigger_updateProperty(asUpd->selector_lst, asUpd->prop_cat, asUpd->prop_id, asUpd->prop_conly, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

void
CjvxQtSaWidgetWrapper::slot_updateWindow_periodic(jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;
	if (asUpd)
	{
		if (asUpd->passExternalUId)
		{
			trigger_updateWindow_periodic(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE, asUpd->externalUId);
			return;
		}
	}
	trigger_updateWindow_periodic(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE);
}

// =================================================================================================================0

void
CjvxQtSaWidgetWrapper::slot_updateStatus(jvxHandle* privatePtr)
{
	jvxPropertyCallPrivate* asUpd = (jvxPropertyCallPrivate*)privatePtr;
	if (asUpd)
	{
		if (asUpd->passExternalUId)
		{
			trigger_updatePropertyStatus(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE, asUpd->externalUId);
			return;
		}
	}
	trigger_updatePropertyStatus(asUpd->selector_lst, asUpd->wildcard, asUpd->groupid_emit, JVX_WIDGET_RWAPPER_UPDATE_DELAYED_RESPONSE, asUpd->externalUId);
}

// =================================================================================================================0

jvxErrorType
CjvxQtSaWidgetWrapper::triggerDirect(const std::string& selector_lst, const std::string& propertyDescr,
	jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, const std::string& uiFilterWildcard)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);
	if (elm != theLists.end())
	{
		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			jvxErrorType resL = elm->theAssociations[i].theUiWrap->triggerDirect(propertyDescr, prop, form, offset, numElements, uiFilterWildcard);
			if ((resL == JVX_ERROR_ELEMENT_NOT_FOUND) || (resL == JVX_ERROR_COMPONENT_BUSY))
			{
			}
			else
			{
				res = JVX_NO_ERROR;
			}
		}
		return res;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxQtSaWidgetWrapper::set_config(jvxWwConfigType tp, const char* selector_lst, jvxHandle* load)
{
	jvxSize i;
	switch (tp)
	{
	case JVX_WW_CONFIG_MATCH_TYPE:
		config.matchIdType = *((jvxWwMatchIdType*)load);
		return JVX_NO_ERROR;
	case JVX_WW_CONFIG_DISTRIBUTE_ALL:
		config.distributeSet = *((jvxBool*)load);
		return JVX_NO_ERROR;
	}

	std::vector<oneAssociationList>::iterator elm = jvx_findItemSelectorInList<oneAssociationList, const std::string&>(
		theLists, selector_lst);
	if (elm != theLists.end())
	{
		for (i = 0; i < elm->theAssociations.size(); i++)
		{
			jvxErrorType resL = elm->theAssociations[i].theUiWrap->set_config(tp, load);
		}
	}
	return JVX_NO_ERROR;
}
