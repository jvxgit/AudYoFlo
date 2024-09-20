#include "common/CjvxPropertiesTypeConvert.h"

CjvxPropertiesTypeConvert::CjvxPropertiesTypeConvert()
{
  memset(&v, 0, sizeof(v));
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

	case JVX_DATAFORMAT_32BIT_LE:
		switch (formIn)
		{
		case JVX_DATAFORMAT_DATA:
			v.vI32 = JVX_DATA2INT32(*(jvxData*)fldin);
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_U32BIT_LE:
			v.vI32 = *(jvxUInt32*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_SIZE:
			v.vI32 = *(jvxSize*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;
		}
		break;

	case JVX_DATAFORMAT_U32BIT_LE:
		switch (formIn)
		{
		case JVX_DATAFORMAT_DATA:
			v.vUI32 = JVX_DATA2UINT32(*(jvxData*)fldin);
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_U32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			v.vUI32 = *(jvxInt32*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_U32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_SIZE:
			v.vUI32 = *(jvxSize*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_U32BIT_LE;
			fldInStore = fldin;
			fldin = &v;
			break;


		}
		break;


	case JVX_DATAFORMAT_SIZE:
		switch (formIn)
		{
		case JVX_DATAFORMAT_DATA:
			v.vSize = JVX_DATA2SIZE(*(jvxData*)fldin);
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_SIZE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			v.vSize = *(jvxInt32*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_SIZE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			v.vSize = *(jvxInt16*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_SIZE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_U32BIT_LE:
			v.vSize = *(jvxUInt32*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_SIZE;
			fldInStore = fldin;
			fldin = &v;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			v.vSize = *(jvxUInt16*)fldin;
			formInStore = formIn;
			formIn = JVX_DATAFORMAT_SIZE;
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

	case JVX_DATAFORMAT_32BIT_LE:
		switch (formInStore)
		{
		case JVX_DATAFORMAT_DATA:
			if (convert && fldInStore)
			{
				*(jvxData*)fldInStore = v.vI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_U32BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxUInt32*)fldInStore = v.vI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_SIZE:
			if (convert && fldInStore)
			{
				*(jvxSize*)fldInStore = v.vI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;
		}
		break;

	case JVX_DATAFORMAT_U32BIT_LE:
		switch (formInStore)
		{
		case JVX_DATAFORMAT_DATA:
			if (convert && fldInStore)
			{
				*(jvxData*)fldInStore = v.vUI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxInt32*)fldInStore = v.vUI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_SIZE:
			if (convert && fldInStore)
			{
				*(jvxSize*)fldInStore = v.vUI32;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;
		}
		break;

	case JVX_DATAFORMAT_SIZE:
		switch (formInStore)
		{
		case JVX_DATAFORMAT_DATA:
			if (convert && fldInStore)
			{
				*(jvxData*)fldInStore = v.vSize;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxInt32*)fldInStore = v.vSize;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxInt16*)fldInStore = v.vSize;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_U32BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxUInt32*)fldInStore = v.vSize;
			}

			formIn = formInStore;
			formInStore = JVX_DATAFORMAT_NONE;
			fldin = fldInStore;
			fldInStore = nullptr;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			if (convert && fldInStore)
			{
				*(jvxUInt16*)fldInStore = v.vSize;
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
