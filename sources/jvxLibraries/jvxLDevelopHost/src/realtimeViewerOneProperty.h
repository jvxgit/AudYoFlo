#ifndef __REALTIMEVIEWERONEPROPERTY_H__
#define __REALTIMEVIEWERONEPROPERTY_H__

#include "ui_realtimeViewerOneProperty.h"
#include "realtimeViewer_base.h"

#include <QtGui/QFocusEvent>

#if defined (QT_NAMESPACE)
namespace QT_NAMESPACE {
#endif

class QLabel;

#if defined (QT_NAMESPACE)
}
#endif

class realtimeViewerProperties;

class QComboBox_ext;
class QLineEdit_ext;
class QSlider_ext;
class QCheckBox_ext;

class realtimeViewerOneProperty: public QWidget, public Ui::Form_oneProperty, public realtimeViewer_base
{
	friend QComboBox_ext;
	friend QLineEdit_ext;
	friend QSlider_ext;
	friend QCheckBox_ext;

	struct
	{
		jvxComponentIdentification tpV;
		jvxSize inPropId;
		std::string description;
	} itemProps;

	struct
	{
		jvxBool update;
		realtimeViewerProperties* parentRef;
	} local;

	Q_OBJECT

public:
	
	realtimeViewerOneProperty(realtimeViewerProperties* parent, IjvxHost* hostRef, CjvxRealtimeViewer* rtvRef, IjvxReport* report, jvxSize sectionId, jvxSize groupId, jvxSize itemId);

	~realtimeViewerOneProperty();

	virtual void get_pointer_main_class(jvxHandle** theMainClass, jvxRtvMainClassType* theType)
	{
		if(theMainClass)
		{
			*theMainClass = reinterpret_cast<jvxHandle*>(this);
		}
		if(theType)
		{
			*theType = JVX_RTV_MAIN_CLASS_RTV_ONE_PROPERTY;
		}
	};

	// ==========================================================================
	// Base class functions
	// ==========================================================================
	void init();
	void cleanBeforeDelete();
	// ==========================================================================

	void cleanContent();

	void updateWindow_construct();
	
	void updateWindow_contents(jvxBool& requiresRedraw, jvxBool fullUpdate = false);
	
	void updateWindow_update();
	
	void updatePropertyIfValid(jvxSize sectionId, jvxSize groupId, jvxSize itemId);

	void newSelection_combo(jvxBitField bitset, jvxBool startedWithRightClick, jvxSize mode);
	void newPosition_slider(int val);
	void newText_lineEdit(QString txt, jvxBool& doReset);
	void toggled_checkBox(bool checkState);


	void deleteUiElementsInWidget();

public slots:

	void newSelection_update(bool checked);
    void buttonPushed_remove();
	void button_select_all();
	void button_unselect_all();
	void button_up(); 
	void button_down(); 
	//void checkbox_toggled(bool);
};



#endif
