#ifndef __CjvwWrapperElementCheckBox_H__
#define __CjvwWrapperElementCheckBox_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QCheckBox>

class CjvxSaWrapperElementAbstractButton: public CjvxSaWrapperElementBase 
{
private:
	typedef enum
	{
		JVX_WW_CHECKBOX_CBOOL = 0,
		JVX_WW_CHECKBOX_SELLIST = 1
	} connectionType;

	//QCheckBox* uiRefTp;
	QAbstractButton* uiRefTp;
	connectionType tp;
	Q_OBJECT

public:
	CjvxSaWrapperElementAbstractButton(QAbstractButton* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef);
	~CjvxSaWrapperElementAbstractButton();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false)override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

public slots:
	void checkbox_toggled(bool);
};

#endif
