#ifndef __CjvwWrapperElementLineEdit_H__
#define __CjvwWrapperElementLineEdit_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

class CjvxSaWrapperElementLineEdit: public CjvxSaWrapperElementBase 
{
private:

	QLineEdit* uiRefTp;
	jvxInt32 numDigits;
	jvxWwTransformValueType optLabelShowTp;
	jvxData minVal;
	jvxData maxVal;
	jvxData granularity;
	std::string optButtonName;
	QWidget* myCrossRefWidget;
	QPushButton* optButtonTrigger;
	jvxBool returnModeOnly; // JVX_WW_CONFIG_LINEEDIT_REACT_MODE_RETURN_ONLY, 
	Q_OBJECT

public:
	CjvxSaWrapperElementLineEdit(QLineEdit* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, QWidget* crossRefWidget, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementLineEdit();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;

	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	void clearUiElements()override;

	virtual jvxErrorType set_config(jvxWwConfigType tp, jvxHandle* load) override;
	void setBgPal();

public slots:
	void editing_finished();
	void return_pressed();
	void transfer_triggered();
};

#endif
