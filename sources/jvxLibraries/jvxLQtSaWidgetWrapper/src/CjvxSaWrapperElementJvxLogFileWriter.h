#ifndef __CjvwWrapperElementJvxLogFileWriter_H__
#define __CjvwWrapperElementJvxLogFileWriter_H__

#include "CjvxSaWrapperElementBase.h"
#include "jvxQtSaLogFileWriter.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

class CjvxSaWrapperElementJvxLogFileWriter: public CjvxSaWrapperElementBase 
{
private:

	std::string propertyPrefix;
	jvxQtSaLogFileWriter* uiRefTp;

	QCheckBox* theCb;
	QLineEdit* theLePrefix;
	QLineEdit* theLeFolder;
	QPushButton* thePbFolder;
	QProgressBar* thePb;
	QGroupBox* theGb;

	std::string defaultPrefix;
	std::string myTitle;

	Q_OBJECT

public:
	CjvxSaWrapperElementJvxLogFileWriter(jvxQtSaLogFileWriter* uiRefLoc, CjvxQtSaWidgetWrapper* backRefLoc, 
		IjvxAccessProperties* propRefIn, std::string propertyPrefix,
		std::vector<std::string> paramLst, const std::string& tag_cp, const std::string& tag_key,
		const std::string& objectName, const std::string& prefix, jvxBool verboseLoc, jvxSize uId,
		IjvxHiddenInterface* hostRef );
	~CjvxSaWrapperElementJvxLogFileWriter();
	
	virtual void clearUiElements()override;

	virtual void initializeUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType updateWindowUiElement(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL, jvxBool call_periodic_update = false) override;
	virtual void setPropertiesUiElement(jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual jvxErrorType setPropertiesDirect(jvxHandle* prop, jvxDataFormat form, jvxSize offset, jvxSize numElements, jvxCBitField whattoset = -1, jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL) override;
	virtual void updatePropertyStatus(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* globalId = NULL)override;

	virtual void trigger_updateWindow_periodic(jvxPropertyCallContext ccontext = JVX_WIDGET_RWAPPER_UPDATE_NORMAL, jvxSize* passedId = NULL) override;

public slots:
	void checkbox_toggled(bool);
	void editing_finished_prefix();
	void button_pushed_folder();
};

#endif
