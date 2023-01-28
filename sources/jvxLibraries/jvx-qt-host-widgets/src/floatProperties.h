#ifndef __FLOATPROPERTIES_H__
#define __FLOATPROPERTIES_H__

#include "baseUiProperties.h"
#include "ui_floatProperties.h"

class floatProperties: public QDialog, public Ui::Dialog_floatProps, protected baseUiProperties
{
	Q_OBJECT

public:
	
	floatProperties(jvxComponentIdentification tp, jvxSize idx, jvxPropertyCategoryType category, jvxUInt64 allowStateMask,
		 jvxUInt64 allowThreadingMask, jvxDataFormat format, jvxSize num, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decTp,
		jvxSize handleIdx, IjvxProperties* theProps, IjvxObject* theObj);

	void init();
	void updateWindow();

public slots:
	// Common
	void comboBox_decoder_activated(int id){updateWindow();};
	void anyRadioButtonClicked(){updateWindow();};
    
	// Specific
	void textEdited();
};

#endif
	