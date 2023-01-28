#ifndef __CjvwWrapperElementPushButton_H__
#define __CjvwWrapperElementPushButton_H__

#include "CjvxSaWrapperElementBase.h"
#include "CjvxQtSaWidgetWrapper_types.h"
#include <QtWidgets/QPushButton>
#include <QtGui/QPixmap>

class CjvxSaWrapperElementPushButton: public CjvxSaWrapperElementBase 
{
private:

	QPushButton* uiRefTp;
	jvxWwConnectionType_pb tp;
	jvxSize option_active;
	std::string namePicOn;
	std::string namePicOff;
	QPixmap pixmapOn;
	QPixmap pixmapOff;
	jvxBool validPicOn;
	jvxBool validPicOff;
	std::string onStyleSheet;
	std::string offStyleSheet = "background-color: rgb(255,255,255);";
	std::string onShowText;
	std::string offShowText;

	std::string prefixPics;
	std::vector<QPixmap> pixmaps;
	std::vector<jvxBool> validPics;

	jvxBool buttonPushState;
	jvxSize buttonPushSel;
	jvxSize numSels;

	std::string fileEnding;
	std::string defaultFolder;

	jvxBool modeRemote;
	Q_OBJECT

public:
	CjvxSaWrapperElementPushButton(QPushButton* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyName,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key, const std::string& objectName,
		const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef);
	~CjvxSaWrapperElementPushButton();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

public slots:
	void button_pushed();
};

#endif
