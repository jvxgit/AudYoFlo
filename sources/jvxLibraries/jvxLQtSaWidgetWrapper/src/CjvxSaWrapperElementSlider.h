#ifndef __CjvwWrapperElementSlider_H__
#define __CjvwWrapperElementSlider_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

class CjvxSaWrapperElementSlider: public CjvxSaWrapperElementBase 
{
private:
	typedef enum
	{
		JVX_WW_SLIDER_VALUE_RANGE = 0,
		JVX_WW_SLIDER_VALUE = 1,
		JVX_WW_SLIDER_SWITCH = 2
	} connectionType;

	QSlider* uiRefTp;
	connectionType tp;
	jvxData deltaSteps;
	jvxData minVal;
	jvxData maxVal;
	
	QLabel* optLabelShow;
	std::string optLabelName;
	jvxWwTransformValueType optLabelShowTp;
	jvxSize numDigits;
	std::string optLabelUnit;
	std::string optLabelOn;
	std::string optLabelOff;
	jvxBool optLabelUnitSet;
	QWidget* myCrossRefWidget;

	Q_OBJECT

public:
	CjvxSaWrapperElementSlider(QSlider* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, 
		const std::string& tag_cp, const std::string& tag_key, const std::string& objectName, const std::string& prefix,
		QWidget* crossReferences, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef);
	~CjvxSaWrapperElementSlider();

	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

public slots:
	void slider_released();
	void slider_moved();
};

#endif