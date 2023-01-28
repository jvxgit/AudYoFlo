#ifndef __STRINGFILENAMEPROPERTIES_H__
#define __STRINGFILENAMEPROPERTIES_H__

#include "baseUiProperties.h"
#include "ui_stringFilenameProperties.h"

class stringFilenameProperties: public QDialog, public Ui::Dialog_strFilenameProps, protected baseUiProperties
{
	Q_OBJECT

public:
	
	stringFilenameProperties(jvxComponentIdentification tp, jvxSize idx, jvxPropertyCategoryType category, jvxUInt64 allowStateMask,
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

	void selectFilename();

};

#endif
	