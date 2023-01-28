#ifndef __CJVXSAWRAPPERELEMENTJVXNETWORKSLAVEDEVICE_H__
#define __CJVXSAWRAPPERELEMENTJVXNETWORKSLAVEDEVICE_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "jvxQtSaNetworkSlaveDevice.h"

// #define JVX_CONNECT_ELEMENTS
class CjvxSaWrapperElementJvxNetworkSlaveDevice: public CjvxSaWrapperElementBase 
{
private:

	jvxQtSaNetworkSlaveDevice* uiRefTp;

	jvxQtSaNetworkSlaveDevice::theWidgetRefs theReferences;

	Q_OBJECT

public:
	CjvxSaWrapperElementJvxNetworkSlaveDevice(jvxQtSaNetworkSlaveDevice* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
		IjvxAccessProperties* propRefIn, std::string propertyName, std::vector<std::string> paramLst,
		const std::string& tag_cp, const std::string& tag_key, const std::string& objectName, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementJvxNetworkSlaveDevice();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL) override;

public slots:

	void onButtonListen();
	void onEditPort();

	void onNewBuffersize();
	void onNewSamplerate();
	void onNewNumInChannels();
	void onNewNumOutChannels();

	void onNewFormat(int);
	void onClickAudioAuto();

#ifdef JVX_CONNECT_ELEMENTS
	void newTextDestHost();
	void newTextDestPort();
	void pushedButtonGo();
	void autoConnectClicked(bool);
	void autoStartClicked(bool);
	void toggledMode(int);
	void toggledLog(bool);
#endif
};

#endif