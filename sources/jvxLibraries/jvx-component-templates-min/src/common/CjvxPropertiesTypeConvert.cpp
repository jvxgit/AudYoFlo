#include "common/CjvxPropertiesTypeConvert.h"

CjvxPropertiesTypeConvert::CjvxPropertiesTypeConvert()
{
	memset(&v, sizeof(v), 0);
};

void 
CjvxPropertiesTypeConvert::convertFilterStart(jvxHandle*& fldin, jvxDataFormat formTarget, jvxDataFormat& formIn)
{
	if (
		(fldin == nullptr) || 
		(formTarget == formIn))
	{
		return;
	}

	// Check to observe first steps with this function
	assert(fldInStore == nullptr);

	switch (formTarget)
	{
	case JVX_DATAFORMAT_16BIT_LE:
		switch (formIn)
		{
		case JVX_DATAFORMAT_DATA:
			v.vI16 = JVX_DATA2INT16(*(jvxData*)fldin);
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_16BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			v.vI16 = *(jvxUInt16*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_16BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		switch (formIn)
		{
		case JVX_DATAFORMAT_DATA:
			v.vI16 = JVX_DATA2UINT16(*(jvxData*)fldin);
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_U16BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			v.vI16 = *(jvxInt16*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_U16BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;
		}
		break;
	}
}

void 
CjvxPropertiesTypeConvert::convertFilterStop(jvxHandle*& fldin, jvxDataFormat formTarget, jvxDataFormat& formIn, jvxBool convert)
{
	if (fldInStore == nullptr)
	{
		return;
	}

	switch (formTarget)
	{
	case JVX_DATAFORMAT_16BIT_LE:
		switch (formInStore)
		{
		case JVX_DATAFORMAT_DATA:
			if (convert  && fldInStore)
			{
				*(jvxData*)fldInStore = v.vI16;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			if(convert && fldInStore)
			{
				*(jvxUInt16*)fldInStore = v.vI16;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		switch (formInStore)
		{
		case JVX_DATAFORMAT_DATA:
			if (convert && fldInStore)
			{
				*(jvxData*)fldInStore = v.vUI16;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxInt16*)fldInStore = v.vUI16;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;
		}
		break;
	}
}

void 
CjvxPropertiesTypeConvert::convertFilterCancel()
{
	formInStore = JVX_DATAFORMAT_NONE;	
	fldInStore = nullptr;
}