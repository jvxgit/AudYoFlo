#ifndef __CJVXSAWRAPPERELEMENTJVXNETWORKMASTERDEVICE_H__
#define __CJVXSAWRAPPERELEMENTJVXNETWORKMASTERDEVICE_H__

#include "CjvxSaWrapperElementBase.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include "jvxQtSaNetworkMasterDevice.h"

class CjvxSaWrapperElementJvxNetworkMasterDevice: public CjvxSaWrapperElementBase 
{
private:


	jvxQtSaNetworkMasterDevice* uiRefTp;

	QLineEdit* le_host;
	QLineEdit* le_port;
	QPushButton* pb_connect;
	QLineEdit* le_perform;
	QLabel* stats[6];
	QCheckBox* cb_auto;
	QCheckBox* cb_start;
	QCheckBox* cb_log;
	QLineEdit* le_prof;
	QComboBox* cb_mode;
	QLineEdit* le_tech;

	Q_OBJECT

public:
	CjvxSaWrapperElementJvxNetworkMasterDevice(jvxQtSaNetworkMasterDevice* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc,
		IjvxAccessProperties* propRefIn, std::string propertyName, std::vector<std::string> paramLst,
		const std::string& tag_cp, const std::string& tag_key, const std::string& objectName, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementJvxNetworkMasterDevice();
	
	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL)override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL) override;

public slots:
	void newTextDestHost();
	void newTextDestPort();
	void pushedButtonGo();
	void autoConnectClicked(bool);
	void autoStartClicked(bool);
	void toggledMode(int);
	void toggledLog(bool);
};

#endif