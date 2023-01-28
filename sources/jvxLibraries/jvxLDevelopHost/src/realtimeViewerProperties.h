#ifndef __REALTIMEVIEWERPROPERTIES_H__
#define __REALTIMEVIEWERPROPERTIES_H__

#include "jvx.h"
#include "ui_realtimeViewerProperties.h"
#include "common/CjvxRealtimeViewer.h"
#include "realtimeViewer_base.h"

class realtimeViewerProperties: public QWidget, public Ui::Form_realtimeViewerProperties, public realtimeViewer_base
{
	struct
	{
		jvxComponentIdentification tpV;
		jvxPropertyContext ctxt;

		jvxSize propertyIdShow;
		jvxSize propertyIdComponent;

		jvxInt16 whatToShow;

		int numberEntriesPerLine;
	} selectionUser;

	callbackStruct_section myCallback;
	jvxComponentIdentification* tpAll;
	Q_OBJECT

public:
	
	realtimeViewerProperties(QWidget* parent, CjvxRealtimeViewer* theRef, IjvxHost* theHostRef, std::string& descr, jvxSize sectionId, IjvxReport* report,
		jvxComponentIdentification*);
	~realtimeViewerProperties();

	virtual void get_pointer_main_class(jvxHandle** theMainClass, jvxRtvMainClassType* theType)
	{
		if(theMainClass)
		{
			*theMainClass = reinterpret_cast<jvxHandle*>(this);
		}
		if(theType)
		{
			*theType = JVX_RTV_MAIN_CLASS_RTV_PROPERTIES;
		}
	};

	// ============================================================================
	// Base class for low level control
	// ============================================================================
	void init();
	void cleanBeforeDelete();
	// ============================================================================

//	void updateWindow_insertTab(jvxRealtimeViewerType tp, int idInsert);
//	void  updateWindow_removeTab(int idRemove);
	void updateWindow();
	//void updateWindow_redraw_addgroup(int groupId);
	//void updateWindow_redraw_additem(int groupId);
	

	void removeMe_delayed(jvxSize sectionId, jvxSize groupId, jvxSize itemId);

		// ===========================================================================================
	// Callback for updates
	// ===========================================================================================
	static jvxErrorType callbackUpdateField(jvxSize SectionId, jvxSize groupId, jvxSize itemId, jvxHandle* privateData);
	//jvxRealtimeViewerType tpViewItem, jvxSize groupId, 
											  //jvxSize itemId, jvxComponentType tpComp, jvxPropertyCategoryType category,
											  //jvxSize handleId, jvxHandle* privateData);

	// In class version
	jvxErrorType cbUpdateField(jvxSize SectionId, jvxSize groupId, jvxSize itemId);
	// jvxRealtimeViewerType tpViewItem, jvxSize groupId, 
	// jvxSize itemId, jvxComponentType tpComp, jvxPropertyCategoryType category,
	// jvxSize handleId);

	void askForCompleteRebuild();
	void updateWindow_update(jvxBool fullUpdate);
	void updateWindow_detach_ui();
	void updateWindow_attach_ui();

signals:

	void emit_removeMe_inThread(int sectionId, int groupId, int itemId);
	void emit_updateWindow_redraw_all();
	void emit_updateScroll_delayed();

public slots:
	void updateScroll_delayed();

	void updateWindow_redraw_all();
	void newSelection_component(int);
	void newSelection_property(int);
	void newSelection_context(int);
	void activate_whatToShow(int);

	void removeGroup();
	void addElement();
	void addGroup();

	void buttonPushed_showall();
	void buttonPushed_printall();
    void selectionNumberPerLine();

	void removeMe_inThread(int sectionId, int groupId, int itemId);

/*
	void newText_description();
    void newSelectionButton_addProps();
    void newSelectionButton_addPlots();
    void newSelectionButton_remove();
    void newSelectionButton_start();
    void newSelectionButton_stop();
	void newSelectionTab(int id);
	*/
};

#endif