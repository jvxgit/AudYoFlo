#ifndef __CjvwWrapperElementJvxNetworkTechnology_H__
#define __CjvwWrapperElementJvxNetworkTechnology_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "jvxQtSaNetworkTechnology.h"

class CjvxSaWrapperElementJvxNetworkTechnology: public CjvxSaWrapperElementBase 
{
private:


	jvxQtSaNetworkTechnology* uiRefTp;

	QComboBox* cb_tech;

	Q_OBJECT

public:
	CjvxSaWrapperElementJvxNetworkTechnology(jvxQtSaNetworkTechnology* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName, std::vector<std::string> paramLst,
		const std::string& tag_cp, const std::string& tag_key, const std::string& objectName, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef = NULL);
	~CjvxSaWrapperElementJvxNetworkTechnology();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext, jvxSize* passedId)override;

public slots:
	
	void selectedTechnology(int);
};

#endif