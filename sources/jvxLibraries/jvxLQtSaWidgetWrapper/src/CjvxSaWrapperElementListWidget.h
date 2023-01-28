#ifndef __CJVWWRAPPERELEMENTLISTWIDGET_H__
#define __CJVWWRAPPERELEMENTLISTWIDGET_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>

class CjvxSaWrapperElementListWidget: public CjvxSaWrapperElementBase 
{
private:

	QListWidget* uiRefTp;
	jvxContext ctxt;
	jvxWwLineSeparation_lw lineSeparatorType;

	QWidget* myCrossRefWidget;

	std::string optButtonName;
	QPushButton* optButtonTrigger;
	
	std::string optCheckBoxName;
	QCheckBox* optCheckBoxClick;
	
	jvxInt32 numLinesShow;
	std::string collectData;
	jvxBool followNewest;

	Q_OBJECT

public:
	CjvxSaWrapperElementListWidget(QListWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementListWidget();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;

	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	void clearUiElements()override;
	void updateShowListWidget();
	void follow_list();

	virtual jvxErrorType set_config(jvxWwConfigType tp, jvxHandle* load) override;

	jvxBool local_command(std::string& local_command) override;

public slots:
	void trigger_clear();
	void clicked_follow();
};

#endif
