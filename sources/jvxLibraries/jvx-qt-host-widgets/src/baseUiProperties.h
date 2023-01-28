#ifndef __BASEUIPROPERTIES__H__
#define __BASEUIPROPERTIES__H__

#include "jvx.h"

class baseUiProperties
{
protected:
	struct
	{
		jvxComponentIdentification tp;
		jvxSize idx;
		jvxSize handleIdx;
		jvxPropertyCategoryType category;
		jvxUInt64 allowStateMask;
		jvxUInt64 allowThreadingMask;
		jvxDataFormat format;
		jvxSize num;
		jvxPropertyAccessType accessType;
		jvxBool isValid;
		jvxPropertyDecoderHintType decTp;
		IjvxProperties* theProps;
		IjvxObject* theObj;
	} propertyParams;

public:
	baseUiProperties(jvxComponentIdentification tp, jvxSize idx, jvxPropertyCategoryType category, jvxUInt64 allowStateMask, 
		 jvxUInt64 allowThreadingMask, jvxDataFormat format, jvxSize num, jvxPropertyAccessType accessType, jvxPropertyDecoderHintType decTp,
		jvxSize handleIdx, IjvxProperties* theProps, IjvxObject* theObj)
	{
		propertyParams.tp = tp;
		propertyParams.idx = idx;
		propertyParams.category = category;
		propertyParams.allowStateMask = allowStateMask;
		propertyParams.allowThreadingMask = allowThreadingMask;
		propertyParams.format = format;
		propertyParams.num = num;
		propertyParams.accessType = accessType;
		propertyParams.decTp = decTp;
		propertyParams.theProps = theProps;
		propertyParams.theObj = theObj;
		propertyParams.handleIdx = handleIdx;
		propertyParams.isValid = false;
	};
};

#endif