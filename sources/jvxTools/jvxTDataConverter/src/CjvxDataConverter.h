#ifndef __CJVXDATACONVERTER_H__
#define __CJVXDATACONVERTER_H__

#include "common/CjvxObject.h"

class CjvxDataConverter: public IjvxDataConverter, public CjvxObject
{
public:

	typedef enum
	{
		ERROR_ERROR,
		DATA_DATA,
		DATA_INT32,
		DATA_INT16,
		INT32_DATA,
		INT32_INT32,
		INT32_INT16,
		INT16_DATA,
		INT16_INT32,
		INT16_INT16
	} conversionType;
	
	
	CjvxDataConverter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	
	~CjvxDataConverter();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* hostRef)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare(jvxDataFormat inputFormat, jvxDataFormat outputFormat)override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION process(const jvxHandle* fldIn, jvxHandle* fldOut, jvxSize numSamples)override;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override;
	
#include "codeFragments/simplify/jvxObjects_simplify.h"

private:
	
	conversionType whichConversion;
};

#endif
