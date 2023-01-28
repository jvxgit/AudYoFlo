#ifndef __CjvwWrapperElementComboBox_H__
#define __CjvwWrapperElementComboBox_H__

#include "CjvxSaWrapperElementBase.h"

#include <QtWidgets/QComboBox>

class CjvxSaWrapperElementComboBox: public CjvxSaWrapperElementBase 
{
private:
	

	QComboBox* uiRefTp;
	jvxWwConnectionType_cb tp;
	jvxBool initialSelectionRead;
	jvxBitField latestValue;
	Q_OBJECT

public:
	CjvxSaWrapperElementComboBox(QComboBox* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId, IjvxHiddenInterface* hostRef);
	~CjvxSaWrapperElementComboBox();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

public slots:
	void combobox_activated(int);
};

#endif