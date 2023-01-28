#ifndef __CJVXMAWRAPPERELEMENTTREEWIDGET_H__
#define __CJVXMAWRAPPERELEMENTTREEWIDGET_H__

#include "CjvxSaWrapperElementBase.h"
#include "jvxSaWidgetWrapper_elements.h"
#include <QtWidgets/QTreeWidget>
#include <QtGui/QPixmap>

class CjvxMaWrapperElementTreeWidget: public CjvxSaWrapperElementBase
{
private:

	std::vector<std::string> theTreeWidgetEntryLists;
	jvxSize numEntries = 0;
	jvxSize numEntriesRTUpdate = 0;
	std::string token_search;
	/*
	typedef enum
	{
		JVX_WW_PUSHBUTTON_CBOOL = 0,
		JVX_WW_PUSHBUTTON_SELLIST = 1,
		JVX_WW_PUSH_BUTTON_OPEN_DIRECTORY = 2,
		JVX_WW_PUSH_BUTTON_OPEN_FILE_LOAD = 3,
		JVX_WW_PUSH_BUTTON_OPEN_FILE_SAVE = 4
	} connectionType;
	*/
	QTreeWidget* uiRefTp;
	

	/*
	connectionType tp;
	std::string namePicOn;
	std::string namePicOff;
	QPixmap pixmapOn;
	QPixmap pixmapOff;
	jvxBool validPicOn;
	jvxBool validPicOff;

	std::string prefixPics;
	std::vector<QPixmap> pixmaps;
	std::vector<jvxBool> validPics;
	*/
	
	Q_OBJECT

public:
	CjvxMaWrapperElementTreeWidget(QTreeWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId, IjvxHiddenInterface* hostRef);
	~CjvxMaWrapperElementTreeWidget();
	
	//void reportPropertySet(const char* tag, const char* descror);
	void reportPropertySetItem(const char* tag, const char* descror, jvxWwReportType tp, QTreeWidgetItem* thisisme, jvxErrorType res_in_call);

	void processLeefs(QTreeWidgetItem *theItem, int cnt, const std::string& prefix, jvxSize& numEntries, jvxSize& numEntriesRTUpdate);
	void updateWindowUiElement(QTreeWidgetItem *theItem, int cnt, jvxBool call_periodic_update);
	jvxBool updateRealtime() override;

	static bool hasValidTags(QTreeWidget* theTree, const std::string &tag);
	static bool hasValidTags(QTreeWidgetItem *theItem, const std::string& tag, int cnt, const std::string& prefix);

	static jvxBool getAllTagInformation(QTreeWidgetItem* theItem, std::string& compTag, std::string& propName, std::vector<std::string>& lst, jvxPropertyDescriptor& propD, basePropInfos& myBasePropIs);
	void removeCurrentWidget(CjvxQtSaWidgetWrapper_elementbase* w, bool forceImmediateDelete = false);

	virtual void editingCompleted(CjvxQtSaWidgetWrapper_elementbase* theWidgetToClose);
	void propertyReference(IjvxAccessProperties** locPropRef);

	virtual void clearUiElements() override;
	void clearUiElements(QTreeWidgetItem* theItem, jvxBool& removeme);

	CjvxQtSaWidgetWrapper* getBackwardReference();

	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL) override;

	virtual bool matchesWildcard(const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType)override;
	bool matchesWildcard(QTreeWidgetItem *theTLItem, const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType);
	bool matchesWildcardCore(QTreeWidgetItem *theItem, const std::string& wildcard, jvxSize groupid, jvxWwMatchIdType matchIdType);

public slots:
	void treeWidgetItemDblClicked(QTreeWidgetItem*, int);
	void treeWidgetContextMenuRequest(const QPoint& pos);
	// void treeWidgetPressed(QTreeWidgetItem*, int);


};

#endif
