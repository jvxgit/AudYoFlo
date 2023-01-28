#ifndef __CjvwWrapperElementProgressBar_H__
#define __CjvwWrapperElementProgressBar_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLabel>

class CjvxSaWrapperElementProgressBar: public CjvxSaWrapperElementBase 
{
private:

	QProgressBar* uiRefTp;
	jvxData deltaSteps;
	jvxData minVal;
	jvxData maxVal;
	
	QLabel* optLabelShow;
	std::string optLabelName;
	jvxWwTransformValueType optLabelShowTp;
	jvxSize numDigits;
	std::string optLabelUnit;
	jvxBool optLabelUnitSet;
	QWidget* myCrossRefWidget;
	Q_OBJECT

public:
	CjvxSaWrapperElementProgressBar(QProgressBar* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName, std::vector<std::string> paramLst,
		const std::string& tag_cp, const std::string& tag_key, const std::string& objectName, const std::string& prefix,
		QWidget* crossReferences, jvxBool verboseLoc, jvxSize uId, IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementProgressBar();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false)override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;
};

#endif