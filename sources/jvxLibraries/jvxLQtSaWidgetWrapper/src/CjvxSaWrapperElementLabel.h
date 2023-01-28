#ifndef __CjvwWrapperElementLabel_H__
#define __CjvwWrapperElementLabel_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QLabel>

class CjvxSaWrapperElementLabel: public CjvxSaWrapperElementBase 
{
private:

	QLabel* uiRefTp;
	jvxInt32 numDigits;
	jvxWwTransformValueType optLabelShowTp;
	jvxData minVal;
	jvxData maxVal;
	jvxWwConnectionType_la tp;
	std::string colorNames;
	std::vector<QColor> colorList;

	Q_OBJECT

public:
	CjvxSaWrapperElementLabel(QLabel* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementLabel();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

};

#endif