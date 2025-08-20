#ifndef __CJVXPROPERTIESTYPECONVERT_H__
#define __CJVXPROPERTIESTYPECONVERT_H__

#include "jvx.h"

class CjvxPropertiesTypeConvert
{
public:
	union
	{
		jvxData vD;
		jvxInt8 vI8;
		jvxUInt8 vUI8;
		jvxInt16 vI16;
		jvxUInt16 vUI16;
		jvxInt32 vI32;
		jvxUInt32 vUI32;
		jvxInt64 vI64;
		jvxUInt64 vUI64;
		jvxSize vSize;
	} v;

	CjvxPropertiesTypeConvert();

	jvxDataFormat formInStore = JVX_DATAFORMAT_NONE;
	jvxHandle* fldInStore = nullptr;

	void convertFilterStart(jvxHandle*& fldin, jvxDataFormat formTarget, jvxDataFormat& formIn);
	void convertFilterStop(jvxHandle*& fldin, jvxDataFormat formTarget, jvxDataFormat& formIn, jvxBool convert);
	void convertFilterCancel();
};

#endif
