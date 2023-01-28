#ifndef __CjvwWrapperElementFrame_H__
#define __CjvwWrapperElementFrame_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>

class CjvxSaWrapperElementFrame: public CjvxSaWrapperElementBase 
{
private:

	QWidget* uiRefTp;
	jvxWwConnectionType_cb tp;
	jvxBool initialSelectionRead;
	std::vector<QLabel*> lstSelLabels;
	std::vector<QPushButton*> lstSelButtons;
	std::vector<QCheckBox*> lstSelCheckboxes;
	jvxSize wrapAroundCount;
	jvxWwUiElementType_fr elmTp;
	jvxSize index_selection;
	jvxBitField maskSelection;
	jvxSize minSize_w;
	jvxSize minSize_h;
	jvxSize maxSize_w;
	jvxSize maxSize_h;

	Q_OBJECT

public:
	CjvxSaWrapperElementFrame(QWidget* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementFrame();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;
	
public slots:
	void slot_button_pushed();
	void slot_checkbox_toggled(bool tog);
};

#endif