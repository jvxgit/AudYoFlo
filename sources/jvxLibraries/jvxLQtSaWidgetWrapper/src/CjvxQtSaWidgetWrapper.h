#ifndef __JVXQTWIDGETWRAPPER_H__
#define __JVXQTWIDGETWRAPPER_H__

#include "jvx.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include "CjvxSaWrapperElementBase.h"
#include "CjvxQtSaWidgetWrapper_keymap.h"
#include "CjvxQtSaWidgetWrapper_types.h"

#define JVX_SA_WIDGETWRAPPER_GENERIC_PREFIX  "___GEN_PREFIX___"

typedef enum
{
	JVX_HTTPAPI_DELAYED_UPDATE_TERMINATE // argument is a "jvxSize*" to pass unique id in http transfer
} jvxSaWidgetWrapperspecificReport;

// Forward declaration
class CjvxQtSaWidgetWrapper_treewidget_slots;

JVX_INTERFACE IjvxQtSaWidgetWrapper_report
{
public:
	virtual ~IjvxQtSaWidgetWrapper_report(){};

	virtual jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, jvxErrorType res_in_call) = 0;
	virtual jvxErrorType reportPropertyGet(const char* tag, const char* propDescrptior, jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, jvxDataFormat format, jvxErrorType res_in_call) = 0;
	virtual jvxErrorType reportAllPropertiesAssociateComplete(const char* tag) = 0;
	virtual jvxErrorType reportPropertySpecific(jvxSaWidgetWrapperspecificReport, jvxHandle* props) = 0;
};

class CjvxQtSaWidgetWrapper: public QObject, public IjvxAccessProperties_delayed_report
{
	friend class CjvxQtSaWidgetWrapper_treewidget_slots;
private:

	Q_OBJECT

	typedef struct
	{
		QWidget* theWidget;
		// The wrapper element
		CjvxSaWrapperElementBase* theUiWrap;
		std::string accessDescription_orig;
	} oneAssociation;

	/*
	typedef struct
	{
		jvxSize uId;
		jvxSize gId;
	} one_ass_prop;
	*/

	struct oneAssociationList
	{
		std::string selector;
		std::vector<oneAssociation> theAssociations;
		IjvxAccessProperties* propRef = nullptr;
		jvxBitField thePropAFeatures;
		IjvxQtSaWidgetWrapper_report* report = nullptr;
		std::string tag_key;
		std::list<jvxSize> ass_property_ids;
		jvxSize uniqueId = 0;

		jvxSize procId = 0;
		jvxSize procIdSP = 0;
		jvxSize procIdP = 0;

		jvxBool inFinalStage = false;
	} ;

	std::vector<oneAssociationList> theLists;

	jvxSize g_uniqueId;
	jvxBool verbose_output;

	struct 
	{
		jvxWwMatchIdType matchIdType;
		jvxBool distributeSet;
	} config;

	std::list<jvxSize> ass_property_ids_collect_on_erase;

public:

	struct
	{
		keyValueList pb;
		keyValueList la;
		keyValueList le;
		keyValueList lw;
		keyValueList cb;
		keyValueList slpba;
		keyValueList ao;
		keyValueList fr;
	} keymaps;

	CjvxQtSaWidgetWrapper();
	~CjvxQtSaWidgetWrapper();


	jvxErrorType associateAutoWidgets(QWidget* theWidget, IjvxAccessProperties* propRefIn,
		IjvxQtSaWidgetWrapper_report* reportLoc = NULL,
		std::string selector_lst = "", jvxBool verboseLoc = true,
		jvxWwSearchMode startWithWidgetRoot = JVX_WW_WIDGET_SEARCH_ALL_CHILDREN, 
		std::string prefix = "",
		std::string tag = "",
		IjvxHiddenInterface* hostRef = NULL);
	jvxErrorType deassociateAutoWidgets(std::string tag = "");

	jvxErrorType set_config(jvxWwConfigType tp, const char* tag, jvxHandle* load);

	jvxErrorType reportPropertySet(const char* tag, const char* propDescrptior, jvxSize groupid, 
		jvxWwReportType howtoreport, CjvxSaWrapperElementBase* thisisme, jvxErrorType res_in_call);
	jvxErrorType reportPropertyGet(const char* tag, const char* descror, 
		jvxHandle* ptrFld, jvxSize offset, jvxSize numElements, 
		jvxDataFormat format, CjvxSaWrapperElementBase* thisisme, jvxErrorType res_in_call);
	jvxErrorType reportPropertyInitComplete(const char* tag, jvxSize uId);
	
	jvxErrorType trigger_updatePropertyStatus(const std::string& selector_lst, const std::string& propWildcard = "", jvxSize groupid = 0,
		jvxPropertyCallContext purpose = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* uIdExt = NULL);
	jvxErrorType trigger_updateWindow(const std::string& selector_lst, const std::string& propWildcard = "", jvxSize groupid = 0,
		jvxPropertyCallContext purpose = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* uIdExt = NULL);
	jvxErrorType trigger_updateProperty(const std::string& selector_lst, jvxPropertyCategoryType cat, jvxSize propId, jvxBool onlyContent,
		jvxPropertyCallContext purpose = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* uIdExt = NULL);
	jvxErrorType trigger_updateWindow_core(const std::string& selector_lst,
			std::vector<oneAssociationList>::iterator& elm, jvxSize* uIdExt, jvxPropertyCallContext purpose,
			const std::string& propertyWidcard, jvxSize groupid);

	jvxErrorType trigger_updateWindow_periodic(const std::string& selector_lst, const std::string& wildcard = "", jvxSize groupid = 0,
		jvxPropertyCallContext purpose = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* uIdExt = NULL);

	jvxErrorType trigger_local_command(const std::string& selector_lst, const std::string& propertyWidcard, jvxSize groupid,
			std::string& local_command);

	static void setPropPrefixAllSubWidgets(QWidget* theWidget, std::string newValue)
	{
		jvxSize i;
		QList<QWidget *> widgets = theWidget->findChildren<QWidget *>();
		for(i = 0; i < widgets.count(); i++)
		{
			QString qstr = widgets[(int)i]->accessibleName();
			if(!qstr.isEmpty())
			{
				widgets[(int)i]->setAccessibleName(newValue.c_str());
			}
		}
		theWidget->setAccessibleName(newValue.c_str());
	};

	jvxErrorType report_nonblocking_delayed_update_complete(jvxSize uniqueId, jvxHandle* privatePtr);
	jvxErrorType report_nonblocking_delayed_update_terminated(jvxSize uniqueId, jvxHandle *);

	jvxErrorType triggerDirect(const std::string& selector_lst, const std::string& propertyDescr,
		jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, const std::string& uiFilterWildcard = "");

private:
	void browseWidgets(oneAssociationList& addto, QWidget* theWidget, 
		const std::string& tag, const std::string& tag_key, IjvxAccessProperties* propRefIn,
		QWidget* crossReferences, jvxBool verboseLoc, const std::string& prefix,
		IjvxHiddenInterface* hostRef);

	void processLeefs(QTreeWidgetItem *theItem, int cnt);
	void initKeymappers();


signals:
	void emit_updateWindow(jvxHandle* privatePtr);
	void emit_updateWindow_periodic(jvxHandle* privatePtr);
	void emit_updateStatus(jvxHandle* privatePtr);
	void emit_updateProperty(jvxHandle* privatePtr);
	void emit_reportPropertyInitComplete(QString tag, jvxSize uId);
	void emit_report_all_associated_no_content(QString tag);
	void emit_finalizeTransfer(jvxSize uniqueId, jvxHandle* privatePtr);

public slots:
	void slot_updateWindow(jvxHandle* privatePtr);
	void slot_updateWindow_periodic(jvxHandle* privatePtr);
	void slot_updateStatus(jvxHandle* privatePtr);
	void slot_updateProperty(jvxHandle* privatePtr);
	void slot_reportPropertyInitComplete(QString tag, jvxSize uId);
	void slot_report_all_associated_no_content(QString tag);
	void slot_finalizeTransfer(jvxSize uniqueId, jvxHandle* privatePtr);
};
#endif
